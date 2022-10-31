cmake_minimum_required(VERSION 3.13)

# Maps to Visual Studio solution file (Tutorial.sln)
# The solution will have all targets (exe, lib, dll) 
# as Visual Studio projects (.vcproj)
project (PlatboxClient)


include_directories ("${PROJECT_SOURCE_DIR}/inc" "${PROJECT_SOURCE_DIR}/inc/smm" "${PROJECT_SOURCE_DIR}/inc/windows" "${PROJECT_SOURCE_DIR}/inc/linux" "${PROJECT_SOURCE_DIR}/inc/intel" "${PROJECT_SOURCE_DIR}/inc/amd" "${PROJECT_SOURCE_DIR}/DeltaFuzz")

SET (PROJECT_ROOT "${PROJECT_SOURCE_DIR}")

set (PROJECT_SOURCES 
    src/msr.cpp
    src/pci.cpp
    src/physmem.cpp
    src/PlatboxClient.cpp
    src/UEFIVars.cpp
    src/global.cpp
    src/Util.cpp
    src/_run_attempt.asm
    src/common_chipset.cpp
    
    src/windows/kernelHookCb.cpp
    src/windows/page_tables.cpp
    src/windows/acpidump.cpp

    src/intel/intel_chipset.cpp
    
    src/amd/amd_chipset.cpp
    
    src/smm/smm_communicate.cpp
    src/smm/smi_fuzz.cpp
    
)

add_subdirectory (DeltaFuzz)


set_target_properties(DeltaFuzzLib PROPERTIES LINKER_LANGUAGE C++)

# main executable
add_executable(platbox_cli  "${PROJECT_SOURCES}")



# link lib
target_link_libraries (platbox_cli  DeltaFuzzLib)


# Creates a folder "executables" and adds target 
# project (app.vcproj) under it
#set_property(TARGET app PROPERTY FOLDER "executables")
 
# Properties->General->Output Directory
#set_target_properties(app PROPERTIES
 #                     RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
 
# Adds logic to INSTALL.vcproj to copy app.exe to destination directory
#install (TARGETS app
 #        RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/_install)