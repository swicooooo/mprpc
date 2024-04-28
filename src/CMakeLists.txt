file(GLOB_RECURSE CXX_LIST "*.cpp" src/)

add_library(mprpc SHARED ${CXX_LIST} "user.pb.cc" "header.pb.cc")

target_link_libraries(mprpc mymuduo pthread protobuf)