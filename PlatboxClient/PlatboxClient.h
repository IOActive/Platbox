// Written by Enrique Nissim (IOActive) 06/2018


#pragma once
#include "global.h"
#include "pci.h"
#include "common_chipset.h"
#include "Util.h"
#include "physmem.h"
#include "msr.h"
#include "smm_communicate.h"
#include "smi_fuzz.h"
#include "UEFIVars.h"



void parse_handle_physmem_operation(int argc, char **argv);
void parse_handle_map2user_operation(int argc, char **argv);
void parse_handle_alloc_operation(int argc, char **argv);
void parse_handle_free_memory_operation(int argc, char **argv);
void parse_handle_mem_dump_operation(int argc, char **argv);


void parse_bus_device_function(char *line, UINT *bus, UINT *device, UINT *function);
void parse_handle_pci_operation(int argc, char **argv);



