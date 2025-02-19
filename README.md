![Banner](https://s-christy.com/sbs/status-banner.svg?icon=image/flare&hue=280&title=Shader%20Tester&description=An%20interactive%20OpenGL%20shader%20development%20tool)

## Overview

Shader Tester is an interactive tool for developing and previewing GLSL shaders
in real time. It is written in C++ and built on top of OpenGL 3.3, GLFW, and
Dear ImGui. The application renders a selectable 3D mesh with a selectable
shader and texture, and exposes a live-reload workflow so you can iterate on
shaders without restarting the program.

Custom uniforms declared in shader source files are automatically detected and
surfaced as controls in the UI, making it easy to expose parameters for
tweaking without writing any UI code. When a shader fails to compile or link,
the error log is displayed in the interface and the previous working program
continues to run.

<p align="center">
  <img src="./res/screenshot.png" />
</p>

## Features

- Real-time preview of GLSL vertex and fragment shaders on 3D geometry
- Live shader reloading with error display and graceful fallback
- Automatic detection and UI generation for custom uniforms (`float`, `int`, `bool`, `vec3`, `vec4`)
- Selection of built-in shaders: flat, perlin, ripple, mandelbrot, blend
- Selection of built-in meshes: box, circle, cone, cylinder, grid, icosphere, monkey, plane, torus, UV sphere
- Selection of built-in textures: gradient, checkerboard, rand
- Animated sky background rendered with a dedicated background shader
- First-person camera with mouse look and keyboard movement
- Dear ImGui interface with separate panels for shader, mesh, texture, and uniform controls
- OpenGL debug output for catching driver-level errors

## Dependencies

```
g++
libglfw3-dev
libglew-dev
libgl-dev
libassimp-dev
```

## Building

```
make          # standard build
make debug    # build with warnings and debug symbols
make release  # build with optimizations
make run      # build and run
make clean    # remove build artifacts
```

## License

This work is licensed under the GNU General Public License version 3 (GPLv3).

[<img src="https://s-christy.com/status-banner-service/GPLv3_Logo.svg" width="150" />](https://www.gnu.org/licenses/gpl-3.0.en.html)
