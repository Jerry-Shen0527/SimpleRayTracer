aux_source_directory(. brdf_src)
add_library(BRDF STATIC ${brdf_src})
include_directories(${PROJECT_SOURCE_DIR}/include)
