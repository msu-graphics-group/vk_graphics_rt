#run_slicer.sh
curr_directory=$(pwd)
kslicer_dir="../../kernel_slicer" 
kslicer_exe="cmake-build-release/kslicer" 
cd $kslicer_dir && ./$kslicer_exe "$curr_directory/src/samples/raytracing/raytracing.cpp" -mainClass RayTracer -pattern rtv -shaderCC glsl -megakernel 1 -stdlibfolder TINYSTL -I$curr_directory/external IncludeToShaders -I$curr_directory/src ExcludeFromShaders -I$curr_directory/src/render ExcludeFromShaders -DKERNEL_SLICER -v
