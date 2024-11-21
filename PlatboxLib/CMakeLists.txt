cmake_minimum_required(VERSION 3.13)

# Maps to Visual Studio solution file (Tutorial.sln)
# The solution will have all targets (exe, lib, dll) 
# as Visual Studio projects (.vcproj)
project (PlatboxLib)

if (WIN32)
    set(CMAKE_CXX_STANDARD 20)
endif (WIN32)

include_directories (
    "${PROJECT_SOURCE_DIR}/inc"
    "${PROJECT_SOURCE_DIR}/inc/smm"
    "${PROJECT_SOURCE_DIR}/inc/intel"
    "${PROJECT_SOURCE_DIR}/inc/amd"
    "${PROJECT_SOURCE_DIR}/DeltaFuzz"
)

if (WIN32)
    include_directories ("${PROJECT_SOURCE_DIR}/inc/windows")
    set (PROJECT_SOURCES 
        src/msr.cpp
        src/pci.cpp
        src/io.cpp
        src/physmem.cpp
        src/UEFIVars.cpp
        src/global.cpp
        src/Util.cpp
        #src/_run_attempt.asm
        src/common_chipset.cpp
        
        src/windows/kernelHookCb.cpp
        src/windows/page_tables.cpp
        src/windows/acpidump.cpp

        src/intel/intel_chipset.cpp
        
        src/amd/amd_chipset.cpp
        src/amd/amd_spi.cpp
        src/amd/amd_acpi.cpp
        src/amd/amd_psp.cpp
        
        src/smm/smm_communicate.cpp
        src/smm/smi.cpp
        src/smm/smi_fuzz.cpp

        # src/ec/ite8638.cpp
    )
endif (WIN32)

if (UNIX)
    include_directories (
        "${PROJECT_SOURCE_DIR}/inc/linux"
        "${PROJECT_SOURCE_DIR}/../PlatboxDrv/linux/include"
    )
    set (PROJECT_SOURCES 
        src/msr.cpp
        src/pci.cpp
        src/io.cpp
        src/physmem.cpp
        src/UEFIVars.cpp
        src/global.cpp
        src/Util.cpp
        #src/_run_attempt.asm
        src/common_chipset.cpp        

        src/intel/intel_chipset.cpp
        
        src/amd/amd_chipset.cpp
        src/amd/amd_spi.cpp
        src/amd/amd_acpi.cpp
        src/amd/amd_psp.cpp
        
        src/smm/smm_communicate.cpp
        src/smm/smi.cpp
        src/smm/smi_fuzz.cpp
    )
endif (UNIX)

SET (PROJECT_ROOT "${PROJECT_SOURCE_DIR}")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")


add_subdirectory (DeltaFuzz)


set_target_properties(DeltaFuzzLib PROPERTIES LINKER_LANGUAGE CXX)

# static lib
add_library(platbox_lib STATIC  "${PROJECT_SOURCES}")

# link lib
target_link_libraries (platbox_lib  DeltaFuzzLib)
