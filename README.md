# Yet Another Rendering API
This project is a simple abstraction layer on top of d3d12(and vulkan in the future) that makes them more similar to d3d11 in complexity, written in c99.

Here is the status of some notable things being worked on.
| Features  | Status |
| - | - |
| Automatically sync command lists  | :heavy_check_mark: |
| Automatically sync presents  | :heavy_check_mark: |
| Automatically sync resource states  | :heavy_check_mark: |
| Automatically sync resource deletion | :heavy_check_mark: |
| Multithreading support  | :x: |
| Hardware Raytracing support  | :x: |
| PSO Caching support  | :x: |
| CPP compatibility  | :x: |
| D3D12 backend  | :heavy_check_mark: |
| Vulkan backend  | :x: |

# Tutorial
## Install
To install, download the main yara.h header and the accompanying backend files(ie, yara_d3d12.h and yara_d3d12.c for d3d12) and add them to your project.
## Usage
To use it, add the wanted backend .c file to be compiled and linked and then you should be able to start writing code using the library.
You can find examples on how to use it at https://erichederstedt.github.io/GPUTechniques/ or https://github.com/erichederstedt/GPUTechniquesCode.
