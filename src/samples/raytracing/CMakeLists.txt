if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    include_directories(${CMAKE_SOURCE_DIR}/external/glfw/include)
    link_directories(${CMAKE_SOURCE_DIR}/external/glfw)
    link_directories(${CMAKE_SOURCE_DIR}/external/embree/lib_win64)
else()
    find_package(glfw3 REQUIRED)
    find_package(Threads REQUIRED)
    include_directories(${GLFW_INCLUDE_DIRS})
    link_directories(${CMAKE_SOURCE_DIR}/external/embree/lib)
endif()

find_package(OpenMP)

set(RAYTRACING_EMBREE ${CMAKE_SOURCE_DIR}/external/CrossRT/EmbreeRT.cpp)

set(RENDER_SOURCE
        ${CMAKE_SOURCE_DIR}/external/LiteScene/scene_mgr.cpp
        ../../render/scene_mgr_loaders.cpp
        ../../render/render_imgui.cpp
        simple_render.cpp
        simple_render_rt.cpp
        raytracing.cpp)

set(GENERATED_SOURCE
        ${CMAKE_SOURCE_DIR}/external/CrossRT/VulkanRTX.cpp
        raytracing_generated.cpp
        raytracing_generated_ds.cpp
        raytracing_generated_init.cpp)

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
  link_directories("${CMAKE_SOURCE_DIR}/external/CrossRT/lib_win64")
  set(EMBREE_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/external/CrossRT/lib_win64/embree3.lib)
elseif(CMAKE_SYSTEM_NAME STREQUAL Linux)
  find_package(Threads REQUIRED)
  set(EMBREE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/CrossRT/lib)
  link_directories(${EMBREE_DIR})
  set(EMBREE_LIBS ${EMBREE_DIR}/libembree3.so 
                  ${EMBREE_DIR}/libtbb.so)
endif()

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize-address-use-after-scope -fno-omit-frame-pointer -fsanitize=leak -fsanitize=undefined -fsanitize=bounds-strict")

add_executable(raytracing main.cpp ../../utils/glfw_window.cpp
        ${RAYTRACING_EMBREE}
        ${VK_UTILS_SRC}
        ${SCENE_LOADER_SRC}
        ${RENDER_SOURCE}
        ${IMGUI_SRC}
        ${GENERATED_SOURCE})

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    set_target_properties(raytracing PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

    target_link_libraries(raytracing PRIVATE project_options
                          volk glfw3 project_warnings
                          ${RAYTRACING_EMBREE_LIBS})

    add_custom_command(TARGET raytracing POST_BUILD COMMAND ${CMAKE_COMMAND}
            -E copy_directory "${PROJECT_SOURCE_DIR}/external/embree/bin_win64" $<TARGET_FILE_DIR:raytracing>)
else()
    target_link_libraries(raytracing PRIVATE project_options
                          volk glfw project_warnings
                          Threads::Threads dl ${RAYTRACING_EMBREE_LIBS}) #
endif()

if(OpenMP_CXX_FOUND)
    target_link_libraries(raytracing PUBLIC OpenMP::OpenMP_CXX)
endif()