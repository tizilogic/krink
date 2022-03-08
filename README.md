# krink - custom G2 lib on top of Kinc

[![Android (OpenGL)](https://github.com/tizilogic/krink/actions/workflows/android-opengl.yml/badge.svg?branch=master)](https://github.com/tizilogic/krink/actions/workflows/android-opengl.yml)
[![Android (Vulkan)](https://github.com/tizilogic/krink/actions/workflows/android-vulkan.yml/badge.svg?branch=master)](https://github.com/tizilogic/krink/actions/workflows/android-vulkan.yml)
[![iOS (Metal)](https://github.com/tizilogic/krink/actions/workflows/ios-metal.yml/badge.svg?branch=master)](https://github.com/tizilogic/krink/actions/workflows/ios-metal.yml)
[![iOS (OpenGL)](https://github.com/tizilogic/krink/actions/workflows/ios-opengl.yml/badge.svg?branch=master)](https://github.com/tizilogic/krink/actions/workflows/ios-opengl.yml)
[![Linux on ARM (OpenGL)](https://github.com/tizilogic/krink/actions/workflows/linux-arm.yml/badge.svg?branch=master)](https://github.com/tizilogic/krink/actions/workflows/linux-arm.yml)
[![Linux x64 (OpenGL)](https://github.com/tizilogic/krink/actions/workflows/linux-opengl.yml/badge.svg?branch=master)](https://github.com/tizilogic/krink/actions/workflows/linux-opengl.yml)
[![Linux x64 (Vulkan)](https://github.com/tizilogic/krink/actions/workflows/linux-vulkan.yml/badge.svg?branch=master)](https://github.com/tizilogic/krink/actions/workflows/linux-vulkan.yml)
[![macOS (Metal)](https://github.com/tizilogic/krink/actions/workflows/macos-metal.yml/badge.svg?branch=master)](https://github.com/tizilogic/krink/actions/workflows/macos-metal.yml)
[![macOS (OpenGL)](https://github.com/tizilogic/krink/actions/workflows/macos-opengl.yml/badge.svg?branch=master)](https://github.com/tizilogic/krink/actions/workflows/macos-opengl.yml)
[![UWP](https://github.com/tizilogic/krink/actions/workflows/uwp.yml/badge.svg?branch=master)](https://github.com/tizilogic/krink/actions/workflows/uwp.yml)
[![Windows (Direct3D 9)](https://github.com/tizilogic/krink/actions/workflows/windows-direct3d9.yml/badge.svg?branch=master)](https://github.com/tizilogic/krink/actions/workflows/windows-direct3d9.yml)
[![Windows (Direct3D 11)](https://github.com/tizilogic/krink/actions/workflows/windows-direct3d11.yml/badge.svg?branch=master)](https://github.com/tizilogic/krink/actions/workflows/windows-direct3d11.yml)
[![Windows (Direct3D 12)](https://github.com/tizilogic/krink/actions/workflows/windows-direct3d12.yml/badge.svg?branch=master)](https://github.com/tizilogic/krink/actions/workflows/windows-direct3d12.yml)
[![Windows (OpenGL)](https://github.com/tizilogic/krink/actions/workflows/windows-opengl.yml/badge.svg?branch=master)](https://github.com/tizilogic/krink/actions/workflows/windows-opengl.yml)
[![Windows (Vulkan)](https://github.com/tizilogic/krink/actions/workflows/windows-vulkan.yml/badge.svg?branch=master)](https://github.com/tizilogic/krink/actions/workflows/windows-vulkan.yml)
[![Documentation](https://img.shields.io/badge/docs-docsforge-blue)](http://krink.docsforge.com/)

ECS driven library that provides G2 functionality similar to the one
[Kha](https://github.com/Kode/Kha) provides, to render textures, fonts and SDF shapes using
[Kinc](https://github.com/Kode/Kinc).

While technically it should be very portable (given it's based on
[Kinc](https://github.com/Kode/Kinc)), principal focus is put on supporting iOS and Android.

## Note

This is very much a work in progress in a very early stage and the API is very likely to
change/break as work on it continues.
