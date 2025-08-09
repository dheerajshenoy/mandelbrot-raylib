# mbray — Mandelbrot Visualizer in C++ with raylib

https://github.com/user-attachments/assets/6ed91aff-6d15-47b7-848a-68d152cccaec

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
ninja
sudo ninja install # if you want to install it system-wide
```

## Command-line Arguments

`mbray` supports several optional command-line arguments:

- `--color`
  Sets the color mode for rendering the Mandelbrot set.
  Supported values:
  - `bw` – Black on white background
  - `wb` – White on black background
  - `color` – HSV-based coloring
  Default: `bw`

- `--iter`
  Sets the maximum number of iterations used for each pixel in the Mandelbrot test.
  Higher values produce more detail but can slow down rendering.
  Default: `50`

- `--hud`
  Toggles the visibility of the Heads-Up Display (HUD).
  Accepts `true` or `false`.
  Default: `true`

- `--no-resize`
  On window size change, do not resize the mandelbrot set.
