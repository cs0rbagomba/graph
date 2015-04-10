cmake_minimum_required (VERSION 2.6)
project (PROJECT_GRAPH)

set (CXX_FLAGS "-std=c++11")
add_definitions(${CXX_FLAGS})

# set (CMAKE_CXX_COMPILER "/usr/bin/g++-4.9.2")
# set (CMAKE_CXX_COMPILER "/usr/bin/clang++-3.6.0")

add_subdirectory (test)
