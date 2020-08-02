cmake_minimum_required(VERSION 3.17)
project(RayTracer VERSION 1.0)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) 

set(CMAKE_DEBUG_POSTFIX _d)
add_subdirectory(src)