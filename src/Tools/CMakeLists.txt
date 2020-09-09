aux_source_directory(. tools_src)
add_library(Tools STATIC ${tools_src})
include_directories(${PROJECT_SOURCE_DIR}/include)
