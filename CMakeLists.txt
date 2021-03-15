cmake_minimum_required(VERSION 3.17)
project(RayTracer VERSION 1.0 LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
list(APPEND CMAKE_MODULE_PATH "C:/Program Files/OptiX/SDK/CMake")

include(cmake/InitUCMake.cmake)

find_package(OptiX REQUIRED)
find_package(CUDAToolkit REQUIRED)

Ubpa_InitUCMake()

Ubpa_InitProject()
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${CUDAToolkit_INCLUDE_DIRS})
include_directories(${OptiX_INCLUDE})

Ubpa_AddSubDirsRec(src)
 