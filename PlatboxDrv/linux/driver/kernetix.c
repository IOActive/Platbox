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

#include "kernetix.h"

#include <linux/efi.h>
#include <asm/io.h>

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

///////////////////////////////////////////////////////////////////////////

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



static long int kernetix_ioctl(struct file *file, unsigned int cmd, unsigned long carg )
{
	void __user * p = (void __user *)carg;
	
	struct _kernel_write *kwrite = NULL;
	struct _kernel_read *kread = NULL;
  IO_PORT_CALL _io;
	unsigned long _cr3;

  printk(KERN_ALERT "==> The IOCTL for %#08x was called\n", cmd);

  switch (cmd)
  {		
      case KERNETIX_ABW:
      {
        printk(KERN_ALERT "The IOCTL for ABW %#08x was called :)\n", KERNETIX_ABW);
        kwrite = kzalloc(sizeof(struct _kernel_write), GFP_KERNEL);
        
            if ( copy_from_user(kwrite, p, sizeof(struct _kernel_write)) != 0)
              return -EINVAL;  
        /*
        __asm__("pushl  %eax\n\t"				
                  "mov    %cr0,%eax;\n\t"
                  "and    $0xFFFEFFFF,%eax;\n\t"				
                  "mov    %eax,%cr0;\n\t"  					
                  "popl   %eax"
        );		
            */
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
        printk(KERN_ALERT "The IOCTL for ABR %#08x was called :)\n", KERNETIX_ABR);  
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
        printk(KERN_ALERT "The IOCTL for CR3 %#08x was called :)\n", KERNETIX_CR3);
        
        //put_user( ((struct mm_struct *)current->mm)->pgd, (unsigned long **)p );
              
        printk(KERN_ALERT "Value of PGD Virtual Address --> %p\n",
             ((struct mm_struct *)current->mm)->pgd);
            
        __asm__("mov %%cr3, %%rax" : "=a" (_cr3));			       
        
        printk(KERN_ALERT "Value of CR3 (Physical Address) --> %p\n", _cr3);
            put_user( _cr3, (unsigned long **)p );
        
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

      case IOCTL_READ_MSR:
      {         
        
        READ_MSR_CALL *msr_call = (READ_MSR_CALL *) kzalloc(sizeof(READ_MSR_CALL), GFP_KERNEL);		  
        
        if ( copy_from_user(msr_call, p, sizeof(READ_MSR_CALL)) != 0)
          return -EINVAL; 

        __u64 val = 0;          
        _rdmsr(msr_call->msr, &val);     
        put_user( val, (unsigned long **)msr_call->result );

        printk(KERN_INFO "Requested MSR --> %08x = %llx\n", msr_call->msr , val);

        kfree(msr_call);
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

        put_user( val, (unsigned long **)pci_call->result );
        
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

        put_user( val, (unsigned long **)pci_call->result );
        
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

        put_user( val, (unsigned long **)pci_call->result );
        
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

        put_user( bar_size, (unsigned long **)pci_call->result );

        kfree(pci_call);
        break;        
      }

      case IOCTL_GET_EFI_MEMMAP_ADDRESS:

        UINT64 *addr_result;	  
        
        if ( copy_from_user(&addr_result, p, sizeof(UINT64)) != 0)
          return -EINVAL; 

        //printk(KERN_ALERT "efi_memmap: %016lx --> addr_result:[%016lx]\n", &efi.memmap, addr_result);
        put_user( (long unsigned int *) &efi.memmap, (unsigned long **)addr_result );    

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
  }

  return 0;
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
