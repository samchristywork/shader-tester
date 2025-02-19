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

## Dependencies

## Building

## License

This work is licensed under the GNU General Public License version 3 (GPLv3).

[<img src="https://s-christy.com/status-banner-service/GPLv3_Logo.svg" width="150" />](https://www.gnu.org/licenses/gpl-3.0.en.html)
