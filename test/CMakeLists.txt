include_directories(../lib)

add_executable (
test_bin

graph/test_graph.cpp
graph/test_priority_queue.cpp
graph/test_quad_tree.cpp
graph/test_graph_algorithms.cpp
graph/test_marching_squares.cpp
graph/test_plaintext.cpp

test_main.cpp)

target_link_libraries(test_bin gcov)
