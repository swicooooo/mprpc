file(GLOB_RECURSE CXX_LIST "*.cc" "../src/*.cpp")
list(REMOVE_ITEM CXX_LIST ${CMAKE_CURRENT_SOURCE_DIR}/../src/main.cpp)

add_executable(test ${CXX_LIST})

target_link_libraries(test mymuduo pthread)