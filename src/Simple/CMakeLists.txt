aux_source_directory(. simple_src)
include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(main ${simple_src})
link_directories(main ${PROJECT_SOURCE_DIR}/lib)
target_link_libraries(main Integrator)
target_link_libraries(main Geometry)
target_link_libraries(main Tools)

