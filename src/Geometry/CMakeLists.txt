aux_source_directory(. geometry_src)
add_library(Geometry STATIC ${geometry_src})
include_directories(${PROJECT_SOURCE_DIR}/include)
