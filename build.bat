mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A x64 -S .. -B "build64"
cmake --build build64/ --target platbox_cli --config Release
cmake --build build64/ --config Release
copy build64\PlatboxClient\Release\platbox_cli.exe ..\compiled\
copy build64\pocs\Release\*.exe ..\compiled\pocs\
cd ..