if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    include_directories(${CMAKE_SOURCE_DIR}/external/glfw/include)
    link_directories(${CMAKE_SOURCE_DIR}/external/glfw)
else()
    find_package(glfw3 REQUIRED)
    find_package(Threads REQUIRED)
    include_directories(${GLFW_INCLUDE_DIRS})
endif()


include_directories(${CMAKE_SOURCE_DIR}/external/embree)
link_directories(${CMAKE_SOURCE_DIR}/external/embree/lib)

find_package(OpenMP)

set(RAYTRACING_EMBREE
        ../../render/EmbreeRT.cpp)

set(RAYTRACING_EMBREE_LIBS
        embree3 embree_sse42 embree_avx embree_avx2 lexers simd sys tasking)

set(RENDER_SOURCE
        ../../render/scene_mgr.cpp
        ../../render/render_imgui.cpp
        simple_render.cpp
        raytracing.cpp
        )

add_executable(raytracing main.cpp ../../utils/glfw_window.cpp ${RAYTRACING_EMBREE} ${VK_UTILS_SRC} ${SCENE_LOADER_SRC} ${RENDER_SOURCE} ${IMGUI_SRC})

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    set_target_properties(raytracing PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

    target_link_libraries(raytracing PRIVATE project_options
                          volk glfw3 project_warnings)
else()
    target_link_libraries(raytracing PRIVATE project_options
                          volk glfw project_warnings
                          Threads::Threads dl ${RAYTRACING_EMBREE_LIBS}) #
endif()

if(OpenMP_CXX_FOUND)
    target_link_libraries(raytracing PUBLIC OpenMP::OpenMP_CXX)
endif()