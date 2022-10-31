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



#define SVC_NAME "PlatboxSvc"


#ifdef _WIN32
	#include "__win.h"
#elif __linux__
	#include "__linux.h"
#endif




/* UEFI Vars */ 
void list_uefi_variables();

/* Acpi dump */
void EnumerateACPITables();


void init_os_specifics();