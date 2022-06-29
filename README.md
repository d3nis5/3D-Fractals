# Visualization of 3D Fractals
Application for 3D fractal visualization. Fractal is defined by procedural function which is rendered by raymarching in OpenGL compute shader. Parameters of the fractal and rendering can be changed in GUI.

![Screenshot](https://github.com/d3nis5/3D-Fractals/blob/main/screenshot.jpg)

## Features
- Temporal anti-aliasing
- Soft shadows
- Ambient occlusion approximation
- Simple GUI

## Requirements
- [CMake](https://cmake.org/)
- C++14
- OpenGL 4.3 or higher

## Dependencies
- [glad](https://github.com/Dav1dde/glad) (included in this repository)
- [GLFW](https://github.com/glfw/glfw) (v3.2)
- [GLM](https://github.com/g-truc/glm) (v0.9.9.8)
- [Dear ImGui](https://github.com/ocornut/imgui) (v1.80)

All the dependencies are managed by [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake). There is no need to download anything outside of this repository.
