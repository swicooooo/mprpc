file(GLOB_RECURSE CXX_LIST "*.cpp" ../../src/)

add_executable(caller ${CXX_LIST} "caller.cc" "../user.pb.cc" "../../src/header.pb.cc")

# 直接使用项目下的动态库时，需要指定全名和先后顺序
add_dependencies(caller mprpc)
target_link_libraries(caller ${CMAKE_SOURCE_DIR}/lib/libmprpc.so protobuf mymuduo pthread)