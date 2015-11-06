cmake_minimum_required(VERSION 2.8.4)
project(gl_freetype)


find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})

#find_package(glfw REQUIRED)
#include_directories(${GLFW_INCLUDE_DIRS})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})

set(FREETYPE_INCLUDE_DIRS "/usr/include/freetype2/")
set(FREETYPE_LIBRARY "/usr/lib/x86_64-linux-gnu/libfreetype.so.6")
find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


set(HEADERS shader_utils/shader_utils.h text.h)
set(SOURCE_FILES main.cpp shader_utils/shader_utils.cpp text.cpp)
add_executable(gl_freetype ${SOURCE_FILES})


target_link_libraries(gl_freetype  ${GLFW_LIBRARIES} X11 Xxf86vm Xrandr pthread Xi ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${FREETYPE_LIBRARIES})

set(shaders "vertex_shader.glsl"
            "fragment_shader.glsl"
            "OpenSans-Regular.ttf"
            "FreeSans.ttf")

if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
    foreach(item IN LISTS shaders)
        configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/${item}
            ${CMAKE_CURRENT_BINARY_DIR}/${item}
            COPYONLY
        )
    endforeach()
endif()
