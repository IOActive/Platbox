mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A x64 -S .. -B "build64"
cmake --build build64/ --target platbox_cli --config Release
cmake --build build64/ --target example --config Release
copy build64\PlatboxClient\Release\platbox_cli.exe ..\compiled\
copy build64\pocs\Release\example.exe ..\compiled\
cd ..