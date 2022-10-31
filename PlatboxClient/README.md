## Compilation Steps

### Linux

Just use make

### Windows

```
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A x64 -S .. -B "build64"
cmake --build build64/ --target platbox_cli
```

#### Release Build

```
cmake -G "Visual Studio 17 2022" -A x64 -S .. -B "build64" 
cmake --build build64/ --target platbox_cli --config Release
```