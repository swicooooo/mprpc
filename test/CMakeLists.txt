file(GLOB_RECURSE CXX_LIST "*.cc")

add_executable(test ${CXX_LIST})

target_link_libraries(test )