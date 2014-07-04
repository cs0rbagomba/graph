# set(CMAKE_CXX_FLAGS " -Wall -Wextra -pedantic -Weffc++ -Wshadow --std=c++0x  -ggdb")
# set(CMAKE_CXX_COMPILER "/usr/lib/colorgcc/bin/g++")
# set(CMAKE_CXX_COMPILER "/usr/bin/g++-4.8.2")

# set(CMAKE_CXX_COMPILER "/home/denes/bin/colorgcc/g++")

set (CXX_FLAGS "-Wall -Wextra -pedantic -Weffc++ -Wshadow "
#                "-Wpointer-arith -Wcast-qual
               "-std=c++11 "
               "-ggdb -fprofile-arcs -ftest-coverage "
               "-fopenmp -D_GLIBCXX_PARALLEL"
               )

add_definitions( ${CXX_FLAGS} )

set (CMAKE_CXX_COMPILER "/usr/bin/g++-4.9.0")

find_package(CxxTest)

# if ( NOT CXXTEST_FOUND )
#   message(FATAL_ERROR "Package CXXTEST required, but not found!")
# else()
if(CXXTEST_FOUND)

  include_directories(${CXXTEST_INCLUDE_DIR}
                      ../lib
                      /usr/include/libxml2)

  enable_testing()

  CXXTEST_ADD_TEST(test_graph
                   generated_main.cpp
                   graph/test_graph.hpp)

  target_link_libraries(test_graph
                        gcov
                        gomp
                        xml2
                        )

endif()


add_custom_target( test
                    DEPENDS test_graph
                    COMMAND ./tools/run_test.sh ./test_graph
)