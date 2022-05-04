# krink

[![Android (OpenGL)](https://github.com/tizilogic/krink/actions/workflows/android-opengl.yml/badge.svg?branch=main)](https://github.com/tizilogic/krink/actions/workflows/android-opengl.yml)
[![Android (Vulkan)](https://github.com/tizilogic/krink/actions/workflows/android-vulkan.yml/badge.svg?branch=main)](https://github.com/tizilogic/krink/actions/workflows/android-vulkan.yml)
[![FreeBSD (OpenGL)](https://github.com/tizilogic/krink/actions/workflows/freebsd-opengl.yml/badge.svg?branch=main)](https://github.com/tizilogic/krink/actions/workflows/freebsd-opengl.yml)
[![iOS (Metal)](https://github.com/tizilogic/krink/actions/workflows/ios-metal.yml/badge.svg?branch=main)](https://github.com/tizilogic/krink/actions/workflows/ios-metal.yml)
[![iOS (OpenGL)](https://github.com/tizilogic/krink/actions/workflows/ios-opengl.yml/badge.svg?branch=main)](https://github.com/tizilogic/krink/actions/workflows/ios-opengl.yml)
[![Linux on ARM (OpenGL)](https://github.com/tizilogic/krink/actions/workflows/linux-arm-opengl.yml/badge.svg?branch=main)](https://github.com/tizilogic/krink/actions/workflows/linux-arm-opengl.yml)
[![Linux x64 (OpenGL)](https://github.com/tizilogic/krink/actions/workflows/linux-opengl.yml/badge.svg?branch=main)](https://github.com/tizilogic/krink/actions/workflows/linux-opengl.yml)
[![Linux x64 (Vulkan)](https://github.com/tizilogic/krink/actions/workflows/linux-vulkan.yml/badge.svg?branch=main)](https://github.com/tizilogic/krink/actions/workflows/linux-vulkan.yml)
[![macOS (Metal)](https://github.com/tizilogic/krink/actions/workflows/macos-metal.yml/badge.svg?branch=main)](https://github.com/tizilogic/krink/actions/workflows/macos-metal.yml)
[![macOS (OpenGL)](https://github.com/tizilogic/krink/actions/workflows/macos-opengl.yml/badge.svg?branch=main)](https://github.com/tizilogic/krink/actions/workflows/macos-opengl.yml)
[![UWP](https://github.com/tizilogic/krink/actions/workflows/uwp.yml/badge.svg?branch=main)](https://github.com/tizilogic/krink/actions/workflows/uwp.yml)
[![Windows (Direct3D 9)](https://github.com/tizilogic/krink/actions/workflows/windows-direct3d9.yml/badge.svg?branch=main)](https://github.com/tizilogic/krink/actions/workflows/windows-direct3d9.yml)
[![Windows (Direct3D 11)](https://github.com/tizilogic/krink/actions/workflows/windows-direct3d11.yml/badge.svg?branch=main)](https://github.com/tizilogic/krink/actions/workflows/windows-direct3d11.yml)
[![Windows (Direct3D 12)](https://github.com/tizilogic/krink/actions/workflows/windows-direct3d12.yml/badge.svg?branch=main)](https://github.com/tizilogic/krink/actions/workflows/windows-direct3d12.yml)
[![Windows (OpenGL)](https://github.com/tizilogic/krink/actions/workflows/windows-opengl.yml/badge.svg?branch=main)](https://github.com/tizilogic/krink/actions/workflows/windows-opengl.yml)
[![Windows (Vulkan)](https://github.com/tizilogic/krink/actions/workflows/windows-vulkan.yml/badge.svg?branch=main)](https://github.com/tizilogic/krink/actions/workflows/windows-vulkan.yml)
[![HTML5 (WebGL)](https://github.com/tizilogic/krink/actions/workflows/html5-webgl.yml/badge.svg?branch=main)](https://github.com/tizilogic/krink/actions/workflows/html5-webgl.yml)
[![HTML5 (WebGPU)](https://github.com/tizilogic/krink/actions/workflows/html5-webgpu.yml/badge.svg?branch=main)](https://github.com/tizilogic/krink/actions/workflows/html5-webgpu.yml)
[![Documentation](https://img.shields.io/badge/docs-docsforge-blue)](http://krink.docsforge.com/)

## About

**krink** is a lightweight, ultra portable framework built on top of
[Kinc](https://github.com/Kode/Kinc), written in C using [C99](https://en.wikipedia.org/wiki/C99).
It adds:
 * a 2D rendering API very similar to what [Kha](https://github.com/Kode/Kha) provides but with additional capability to render 2D SDF shapes
 * ECS integration
 * Nuklear built in for UI
 * Integrated Memory Allocator
 * ...

**krink** is being developed with a focus to target mobile devices primarily but is none the less
extremely portable, successfully running on a wide range of devices, operating systems, CPU
architectures and graphics APIs *(see the workflow badges above for an almost complete list of
supported platforms where krink apps can be deployed)*

### Game Console support

> From the [Kinc](https://github.com/Kode/Kinc) Readme:
>
> Console support is implemented via separate plugins because the code can not be provided publicly
> \- contact Robert for details if you are interested in it

## Features

### Graphics 2 API

A Canvas type rendering API to draw textures, geometric shapes, text and SDF shapes
*(because we all know the best rectangles have rounded corners! :monocle_face: )*

### ECS

**krink** ships with the powerful and awesome ECS library
[flecs](https://github.com/SanderMertens/flecs), complete with built in flecs modules to make use of
the 2D rendering capabilities **krink** provides.

> *The use of flecs is optional and has to be manually initialized.*

### UI

**krink** contains a slightly modified version of the
[Nuklear](https://github.com/Immediate-Mode-UI/Nuklear) immediate mode UI library that can be
optionally used for user interfaces, rendered directly using the `graphics2` API from **krink**.

> *The use of Nuklear is optional and has to be manually initialized.*

### Integrated Memory Allocator

**krink** currently uses a [TLSF](https://github.com/mattconte/tlsf) implementation to manage memory
at runtime, since I prefer not to *beg* the OS for more memory too often. I do plan to make this an
optional feature in the future, so users can decide whether to use the TLSF memory allocator or
simply use the one provided by `<stdlib.h>`

### Documentation

A large part of the API of **krink** is documentented using Doxygen style comments and the
documentation can be automatically generated using the provided `Doxyfile` using
[Doxygen](https://www.doxygen.nl) and running `doxygen Doxyfile` in the base directory of **krink**.

> Eventually I hope to find the time to enable propper automatic API docs generation to either be
> self hosted somewhere or using a service like DocsForge

### Samples

As of now there are no separate samples to demonstrate how to use **krink**, but there are small
test programs located in the [Tests](Tests) folder that should get you started with krink and as a
wise friend of mine often says:

"Have a look at the code, it's the most accurate form of API documentation" /s

## Note

> While I'd like as much feedback as I can get to improve **krink**, I feel it is important to state
> that this is very much a work in progress and still in a very early stage where the API is still
> being extended and potentially breaking changes to it can (and likely will) happen.

## Contribute

In the hope of not having deterred everyone, feel free to:
 * open an [Issue](https://github.com/tizilogic/krink/issues)
 * make a [Pull Request](https://github.com/tizilogic/krink/pulls)
 * start a [Discussion](https://github.com/tizilogic/krink/discussions)
 * gain access to the [Kode Discord Server](http://discord.kode.tech/) *Disclaimer: I'm just a member there and don't currently see any need to open a separate Discord server for krink*
 * follow/contact/DM me on Twitter [@tizilogic](https://twitter.com/tizilogic)
