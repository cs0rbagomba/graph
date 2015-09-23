#include <graph/graph.hpp>
#include <graph/graph_algorithms.hpp>

#include "../catch.hpp"

#include "fixture.hpp"

#include <iostream>

void printPath(std::size_t number_of_rows,
               std::size_t number_of_columns,
               float2 source,
               float2 destination,
               const std::vector<float2>& path) {

  std::cout << "Path size: " << path.size()  << std::endl;

  std::cout << "Edges: ";
  for (const auto& e : path)
    std::cout << "(" << e.x << "," << e.y << ") ";
  std::cout << std::endl;

  for (std::size_t c = 0; c < number_of_columns; ++c) {
    for (std::size_t r = 0; r < number_of_rows; ++r) {
      const float2 p(r, c);
      if (p == source)
        std::cout << "s ";
      else if (p == destination)
        std::cout << "d ";
      else {
        const bool found = std::find(path.begin(), path.end(), p) != path.end();
        std::cout << (found ? "1 " : ". ");
      }
    }
    std::cout << std::endl;
  }
}


TEST_CASE("Graph algorithms, small", "[graph][algorithm][dijkstra]" ) {

  SECTION("distance") {
      const float2 source(1, 2);
      const float2 destination(12, 13);
      const float euclidan_distance = sqrt(pow(source.x - destination.x, 2) + pow(source.y - destination.y, 2));
      REQUIRE( std::distanceOf2float2s()(source, destination) == euclidan_distance );
    }

  SECTION("empty graph") {
    Graph<int> g;
    const int source(0);
    const int destination(1);
    const std::vector<int> shortestPath = dijkstra_shortest_path_to(g, source, destination, std::distanceOf2ints());
    REQUIRE( shortestPath.size() == 0 );
  }

  SECTION("nonexisting destination") {
    Graph<int> g = { {1, 2}, {1, 3}, {1, 4}, {2, 4}, {3, 4} };
    const int source(1);
    const int destination(10);
    const std::vector<int> shortestPath = dijkstra_shortest_path_to(g, source, destination, std::distanceOf2ints());
    REQUIRE( shortestPath.size() == 0 );
  }

  SECTION("nonexisting source") {
    Graph<int> g = { {1, 2}, {1, 3}, {1, 4}, {2, 4}, {3, 4} };
    const int source(10);
    const int destination(1);
    const std::vector<int> shortestPath = dijkstra_shortest_path_to(g, source, destination, std::distanceOf2ints());
    REQUIRE( shortestPath.size() == 0 );
  }

  SECTION("not connected source and destination") {
    Graph<int> g = { {1, 2}, {3, 4} };
    const int source(1);
    const int destination(4);
    const std::vector<int> shortestPath = dijkstra_shortest_path_to(g, source, destination, std::distanceOf2ints());
    REQUIRE( shortestPath.size() == 0 );
  }

  SECTION("Simple") {
    constexpr std::size_t number_of_rows = 3;
    constexpr std::size_t number_of_columns = 3;
    const std::vector<typename Graph<float2>::Edge>* edges = createEdges<float2>(number_of_rows, number_of_columns);
    Graph<float2> g(*edges);

    const float2 source(0, 0);
    const float2 destination(2, 2);
    const std::vector<float2> shortestPath = dijkstra_shortest_path_to(g, source, destination, std::distanceOf2float2s());

    const float euclidan_distance = sqrt(pow(source.x - destination.x, 2) + pow(source.y - destination.y, 2));
    REQUIRE( std::distanceOf2float2s()(source, destination) == euclidan_distance );

    REQUIRE( shortestPath.size() == 3 );
    REQUIRE( shortestPath[0] == float2(0, 0) );
    REQUIRE( shortestPath[1] == float2(1, 1) );
    REQUIRE( shortestPath[2] == float2(2, 2) );

    delete edges;
  }

}

TEST_CASE_METHOD(Fixture<float2>, "Graph algorithms, big graph", "[graph][algorithm][dijkstra][performance]" ) {

  constexpr std::size_t number_of_rows = 1000;
  constexpr std::size_t number_of_columns = number_of_rows;

  Fixture<float2>::initOnce(number_of_rows, number_of_columns);

  SECTION("Simple") {
    const std::vector<typename Graph<float2>::Edge> edges = Fixture<float2>::getEdges();
    Graph<float2> g(edges);

    const float2 source(0, 0);
    const float2 destination(number_of_rows-1, number_of_columns-1);
    const std::vector<float2> shortestPath = dijkstra_shortest_path_to(g, source, destination, std::distanceOf2float2s());

    REQUIRE( shortestPath.size() == number_of_rows);
    for (std::size_t i = 0; i < number_of_rows; ++i)
      REQUIRE( shortestPath[i] == float2(i, i) );
  }

  SECTION("teardown") {
    Fixture<float2>::tearDown();
  }

}
