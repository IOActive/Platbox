mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A x64 -S .. -B "build64"
cmake --build build64/ --target platbox_cli --config Release
cd ..