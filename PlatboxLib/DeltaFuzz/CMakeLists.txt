cmake_minimum_required(VERSION 3.13)

# Maps to Visual Studio solution file (Tutorial.sln)
# The solution will have all targets (exe, lib, dll) 
# as Visual Studio projects (.vcproj)
project (DeltaFuzzLib)


# static lib
add_library(DeltaFuzzLib STATIC "DeltaFuzz.c")

