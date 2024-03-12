# 2dcurves
A simple OpenGL application to draw 2d curves.

For now, only for Windows with MSVC and only Bézier curves are supported.

## Build
```
git clone https://github.com/manuelruivodeoliveira/2dcurves.git
cd 2dcurves
cmake -S . -B build
cmake --build build
```

You can then run the application normally, with
`./build/Debug/2dcurves.exe`