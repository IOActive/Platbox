### Steps to compile

```
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A x64 -S .. -B "build64"
cmake --build . --target DeltaFuzz
```