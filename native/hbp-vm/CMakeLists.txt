cmake_minimum_required(VERSION 3.16)
project(hbp_vm_3d LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.3 REQUIRED)

add_executable(hbp_vm_3d hbp_vm_3d.cpp)
target_link_libraries(hbp_vm_3d PRIVATE OpenGL::GL GLEW::GLEW glfw)

if(MSVC)
  target_compile_options(hbp_vm_3d PRIVATE /W4 /permissive-)
else()
  target_compile_options(hbp_vm_3d PRIVATE -Wall -Wextra -Wpedantic)
endif()
