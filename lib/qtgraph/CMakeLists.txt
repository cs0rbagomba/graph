cmake_minimum_required (VERSION 2.6)
project (PROJECT_QGRAPH)

set (CXX_FLAGS "-Wall -Wextra -pedantic -Wshadow "
               "-Wpointer-arith -Wcast-qual "
               "-ggdb "
               "--std=c++0x " )

set ( CMAKE_CXX_COMPILER "/usr/bin/g++-4.9.2")
# set ( CMAKE_CXX_COMPILER "distcc")


add_definitions( ${CXX_FLAGS} )

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})
QT4_WRAP_CPP(qtgraph_HEADERS_MOC
# node.hpp
# edge.hpp
graphwidget.hpp)

include_directories(
../
/usr/include/libxml2
)

add_executable (qtgraph
main.cpp
edge.cpp
node.cpp
graphwidget.cpp
marching_squares.cpp
${qtgraph_HEADERS_MOC} )

target_link_libraries ( qtgraph ${QT_LIBRARIES} xml2 png)

add_custom_target (clean2
COMMAND
make clean &&
find -iname '*cmake*' -not -name CMakeLists.txt -exec rm -rf {} \\+ &&
rm Makefile &&
rm moc_*
)
