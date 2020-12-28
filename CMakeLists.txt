cmake_minimum_required(VERSION 3.17)
project(RayTracer VERSION 1.0)


set(CMAKE_CXX_STANDARD 17)


include(cmake/InitUCMake.cmake)
Ubpa_InitUCMake()

Ubpa_InitProject()
include_directories(${PROJECT_SOURCE_DIR}/include)

Ubpa_AddSubDirsRec(src)
   