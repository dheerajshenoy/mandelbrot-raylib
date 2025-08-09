# mbray — Mandelbrot Visualizer in C++ with raylib

**mbray** is an interactive Mandelbrot set visualizer written in modern C++ using [raylib](https://www.raylib.com/).
It renders the fractal in real time, supports smooth zooming, panning, and multiple color modes.

## Features

- Realtime rendering with multithreading
- Zoom and pan with mouse wheel and WASD keys
- Color modes: **Black–White**, **White–Black**, and **Color**
- HUD display for zoom level
- Reset view instantly

## Controls

| Key / Mouse       | Action                                  |
|-------------------|-----------------------------------------|
| **W / A / S / D** | Move view up / left / down / right       |
| **Mouse Wheel**   | Zoom in/out                              |
| **H**             | Toggle HUD display                      |
| **C**             | Cycle color modes                       |
| **R**             | Reset to default view                   |
| **ESC**           | Quit                                     |

## Build Instructions

### Dependencies

- C++23 capable compiler (GCC, Clang, MSVC)
- raylib development libraries
- CMake (recommended)
- Ninja (or make) (build dependency)

### Build with CMake

```bash
git clone https://github.com/yourname/mbray.git
cd mbray
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
```
