cmake_minimum_required(VERSION 3.13)

# Maps to Visual Studio solution file (Tutorial.sln)
# The solution will have all targets (exe, lib, dll) 
# as Visual Studio projects (.vcproj)
project (example)

include_directories (
    "${PROJECT_SOURCE_DIR}/../PlatboxLib/inc"
    "${PROJECT_SOURCE_DIR}/../PlatboxLib/inc/smm"
    "${PROJECT_SOURCE_DIR}/../PlatboxLib/inc/intel"
    "${PROJECT_SOURCE_DIR}/../PlatboxLib/inc/amd"
    "${PROJECT_SOURCE_DIR}/../PlatboxLib/DeltaFuzz"
)

if (WIN32)
    include_directories ("${PROJECT_SOURCE_DIR}/../PlatboxLib/inc/windows")
endif (WIN32)

if (UNIX)
    include_directories (
        "${PROJECT_SOURCE_DIR}/../PlatboxLib/inc/linux"
        "${PROJECT_SOURCE_DIR}/../PlatboxDrv/linux/include"
    )
endif (UNIX)

SET (PROJECT_ROOT "${PROJECT_SOURCE_DIR}")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

set (PROJECT_SOURCES 
    poc.cpp
)

# main executable
add_executable(example  "${PROJECT_SOURCES}")

# link lib
target_link_libraries (example  platbox_lib)
