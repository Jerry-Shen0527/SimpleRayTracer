aux_source_directory(. integrator_src)
add_library(Integrator STATIC ${integrator_src})
include_directories(${PROJECT_SOURCE_DIR}/include)
