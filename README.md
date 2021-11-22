# Ray tracing sample using Vulkan API
This project provides ray tracing applications sample 


## Build

Build as you would any other CMake-based project. For example on Linux:
```
cd vk_graphics_rt
mkdir build
cd build
cmake ..
make -j8
```

On Windows you can directly open CMakeLists.txt as a project in recent versions of Visual Studio. Mingw-w64 building is also supported.

Executable will be built in *bin* subdirectory - *vk_graphics_rt/bin/renderer*

## Run 

* Execute "./raytracing" from the "bin" directory 
* Select rendering mode via '1','2','3' buttons 
* If you don't have support for hardware ray tracing, set "ENABLE_HARDWARE_RT = false" in simple_renderer.h
* If you are going to work with this sample via kernel_slicer, edit appropriate paths in 'run_slicer.sh' file or use VS Code config for this sample from [kernel_slicer](https://github.com/Ray-Tracing-Systems/kernel_slicer) repo. 

## Dependencies
### Vulkan 
SDK can be downloaded from https://vulkan.lunarg.com/

### GLFW 
https://www.glfw.org/

Linux - you can install it as a package on most systems, for example: 
```
sudo apt-get install libglfw3-dev
```

Windows - you don't need to do anything, binaries and headers are included in this repo (see external/glfw directory).

### Additional dependencies 
Can be installed by running provided clone_dependencies.bat script. So, if you clone this repo into vk_graphics_rt directory:

```
cd vk_graphics_basic
clone_dependencies.bat
```

Or you can get them manually:

#### volk:
```
cd vk_graphics_rt/external
git clone --depth 1 https://github.com/zeux/volk.git
```

#### imgui:
```
cd vk_graphics_rt/external
git clone --depth 1 https://github.com/ocornut/imgui.git
```

#### vkutils:
```
cd vk_graphics_rt/external
git clone --depth 1 https://github.com/msu-graphics-group/vk-utils.git vkutils
```

### Single header libraries
This project also uses LiteMath and stb_image which are included in this repo.

https://github.com/msu-graphics-group/LiteMath

https://github.com/nothings/stb

