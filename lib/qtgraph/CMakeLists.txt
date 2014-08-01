cmake_minimum_required (VERSION 2.6)
project (PROJECT_QGRAPH)

set (CXX_FLAGS "-Wall -Wextra -pedantic -Wshadow "
               "-Wpointer-arith -Wcast-qual "
               "-ggdb "
               "--std=c++0x " )

add_definitions( ${CXX_FLAGS} )

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})
QT4_WRAP_CPP(qtgraph_HEADERS_MOC node.hpp edge.hpp graphwidget.hpp)

add_executable ( qtgraph main.cpp edge.cpp node.cpp graphwidget.cpp ${qtgraph_HEADERS_MOC} )
target_link_libraries ( qtgraph ${QT_LIBRARIES} )
