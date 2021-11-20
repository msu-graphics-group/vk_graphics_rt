#!/bin/sh
glslangValidator -V kernel_InitEyeRay.comp -o kernel_InitEyeRay.comp.spv -DGLSL -I.. -I/home/frol/PROG/kernel_slicer/apps/LiteMath 
glslangValidator -V kernel_RayTrace.comp -o kernel_RayTrace.comp.spv -DGLSL -I.. -I/home/frol/PROG/kernel_slicer/apps/LiteMath 
