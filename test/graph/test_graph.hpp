#include <graph/graph.hpp>

#include "/home/denes/projects/Catch/single_include/catch.hpp"

#include <algorithm>
#include <iostream>
#include <map>


TEST_CASE( "Graph creation", "[graph][data_structure]" ) {

  SECTION("Initial state") {
    Graph<int> g;
    REQUIRE( empty(g) == true );
    REQUIRE( numberOfVertices(g) == 0 );
    REQUIRE( numberOfEdges(g) == 0 );
    REQUIRE( g.edges().size() == 0 );
    REQUIRE( g.vertices().size() == 0 );
  }

  SECTION("Initializer list of vertices") {
    Graph<int> g = { 1, 2, 3 };
    REQUIRE( empty(g) == false );
    REQUIRE( numberOfVertices(g) == 3 );
    REQUIRE( numberOfEdges(g) == 0 );
    REQUIRE( g.edges().size() == 0 );
    REQUIRE( g.vertices().size() == 3 );

    REQUIRE( contains(g, 1) == true );
    REQUIRE( contains(g, 2) == true );
    REQUIRE( contains(g, 3) == true );
  }

  SECTION("Initializer list of vertices") {
    Graph<int> g = { {1, 2}, {1, 3}, {3, 4} };
    REQUIRE( empty(g) == false );
    REQUIRE( numberOfVertices(g) == 4 );
    REQUIRE( numberOfEdges(g) == 3*2 );
    REQUIRE( g.edges().size() == 3*2 );
    REQUIRE( g.vertices().size() == 4 );

    REQUIRE( contains(g, 1) == true );
    REQUIRE( contains(g, 2) == true );
    REQUIRE( contains(g, 3) == true );
    REQUIRE( contains(g, 4) == true );
  }

  SECTION("copy ctor") {
    Graph<int> g = { {1, 2}, {1, 3}, {3, 4} };
    Graph<int> g2(g);
    REQUIRE( empty(g2) == false );
    REQUIRE( numberOfVertices(g2) == 4 );
    REQUIRE( numberOfEdges(g2) == 3*2 );
    REQUIRE( g2.edges().size() == 3*2 );
    REQUIRE( g2.vertices().size() == 4 );

    REQUIRE( contains(g2, 1) == true );
    REQUIRE( contains(g2, 2) == true );
    REQUIRE( contains(g2, 3) == true );
    REQUIRE( contains(g2, 4) == true );
  }

  SECTION("Assignment operator") {
    Graph<int> g = { {1, 2}, {1, 3}, {3, 4} };
    Graph<int> g2;
    g2.clear();
    g2 = g;
    REQUIRE( empty(g2) == false );
    REQUIRE( numberOfVertices(g2) == 4 );
    REQUIRE( numberOfEdges(g2) == 3*2 );
    REQUIRE( g2.edges().size() == 3*2 );
    REQUIRE( g2.vertices().size() == 4 );

    REQUIRE( contains(g2, 1) == true );
    REQUIRE( contains(g2, 2) == true );
    REQUIRE( contains(g2, 3) == true );
    REQUIRE( contains(g2, 4) == true );
  }
}

TEST_CASE( "Graph adding vertices", "[graph][data_structure]" ) {

  SECTION("One element") {
    Graph<int> g;
    g.addVertex(1);
    REQUIRE( empty(g) == false );
    REQUIRE( numberOfVertices(g) == 1 );
    REQUIRE( numberOfEdges(g) == 0 );
    REQUIRE( g.edges().size() == 0 );
    REQUIRE( g.vertices().size() == 1 );
  }

  SECTION("Removing the only element") {
    Graph<int> g;
    g.addVertex(1);
    g.removeVertex(1);
    REQUIRE( empty(g) == true );
    REQUIRE( numberOfVertices(g) == 0 );
    REQUIRE( numberOfEdges(g) == 0 );
    REQUIRE( g.edges().size() == 0 );
    REQUIRE( g.vertices().size() == 0 );
  }

  SECTION("Adding some vertices, removing one by one") {
    Graph<int> g;
    g.addVertex(1);
    REQUIRE( numberOfVertices(g) == 1 );
    g.addVertex(2);
    REQUIRE( numberOfVertices(g) == 2 );
    g.addVertex(3);
    REQUIRE( numberOfVertices(g) == 3 );
    g.removeVertex(1);
    REQUIRE( numberOfVertices(g) == 2 );
    g.removeVertex(2);
    REQUIRE( numberOfVertices(g) == 1 );
    g.removeVertex(3);
    REQUIRE( numberOfVertices(g) == 0 );
  }

  SECTION("Adding some vertices then clear") {
    Graph<int> g;
    g.addVertex(1);
    g.addVertex(2);
    g.addVertex(3);
    REQUIRE( numberOfVertices(g) == 3 );
    g.clear();
    REQUIRE( numberOfVertices(g) == 0 );
  }

  SECTION("Removing unexisting vertex") {
    Graph<int> g;
    g.addVertex(1);
    REQUIRE( numberOfVertices(g) == 1 );
    g.removeVertex(2);
    REQUIRE( numberOfVertices(g) == 1 );
  }

  SECTION("Contains vertex") {
    Graph<int> g;
    g.addVertex(1);
    g.addVertex(3);
    REQUIRE( contains(g, 1) == true );
    REQUIRE( contains(g, 2) == false );
    REQUIRE( contains(g, 3) == true );
  }

  SECTION("Modify vertex") {
    Graph<int> g;
    g.addVertex(1);
    REQUIRE( contains(g, 1) == true );
    REQUIRE( contains(g, 2) == false );

    g.modifyVertex(1, 2);
    REQUIRE( contains(g, 1) == false );
    REQUIRE( contains(g, 2) == true );
  }

  SECTION("get array of vertices") {
    Graph<int> g;
    g.addVertex(1);
    g.addVertex(2);
    g.addVertex(3);
    std::vector<int> v = g.vertices();
    REQUIRE( v.size() == 3 );
    REQUIRE( std::find(v.begin(), v.end(), 1) != v.end() );
    REQUIRE( std::find(v.begin(), v.end(), 2) != v.end() );
    REQUIRE( std::find(v.begin(), v.end(), 3) != v.end() );
  }

  SECTION("iterate over vertices") {
    Graph<int> g;
    g.addVertex(1);
    g.addVertex(2);
    g.addVertex(3);

    std::map<int, int> found;
    for (auto it : g) {
      REQUIRE( contains(g, it) == true );
      ++found[it];
    }
    REQUIRE( found.size() == 3 );
  }
}

TEST_CASE( "Graph adding edges", "[graph][data_structure]" ) {

  SECTION("One element") {
    Graph<int> g;
    g.addEdge(1, 2);
    REQUIRE( empty(g) == false );
    REQUIRE( numberOfVertices(g) == 2 );
    REQUIRE( numberOfEdges(g) == 1*2 );
    REQUIRE( g.edges().size() == 1*2 );
    REQUIRE( g.vertices().size() == 2 );
  }

  SECTION("Removing the only edge") {
    Graph<int> g;
    g.addEdge(1, 2);
    g.removeEdge(1, 2);
    REQUIRE( empty(g) == false );
    REQUIRE( numberOfVertices(g) == 2 );
    REQUIRE( numberOfEdges(g) == 0 );
    REQUIRE( g.edges().size() == 0 );
    REQUIRE( g.vertices().size() == 2 );
  }
/*
  SECTION("Adding some edges, removing one by one") {
    Graph<int> g;
    g.addVertex(1);
    REQUIRE( numberOfVertices(g) == 1 );
    g.addVertex(2);
    REQUIRE( numberOfVertices(g) == 2 );
    g.addVertex(3);
    REQUIRE( numberOfVertices(g) == 3 );
    g.removeVertex(1);
    REQUIRE( numberOfVertices(g) == 2 );
    g.removeVertex(2);
    REQUIRE( numberOfVertices(g) == 1 );
    g.removeVertex(3);
    REQUIRE( numberOfVertices(g) == 0 );
  }

  SECTION("Adding some vertices then clear") {
    Graph<int> g;
    g.addVertex(1);
    g.addVertex(2);
    g.addVertex(3);
    REQUIRE( numberOfVertices(g) == 3 );
    g.clear();
    REQUIRE( numberOfVertices(g) == 0 );
  }

  SECTION("Removing unexisting vertex") {
    Graph<int> g;
    g.addVertex(1);
    REQUIRE( numberOfVertices(g) == 1 );
    g.removeVertex(2);
    REQUIRE( numberOfVertices(g) == 1 );
  }

  SECTION("Contains vertex") {
    Graph<int> g;
    g.addVertex(1);
    g.addVertex(3);
    REQUIRE( contains(g, 1) == true );
    REQUIRE( contains(g, 2) == false );
    REQUIRE( contains(g, 3) == true );
  }

  SECTION("Modify vertex") {
    Graph<int> g;
    g.addVertex(1);
    REQUIRE( contains(g, 1) == true );
    REQUIRE( contains(g, 2) == false );

    g.modifyVertex(1, 2);
    REQUIRE( contains(g, 1) == false );
    REQUIRE( contains(g, 2) == true );
  }

  SECTION("get array of vertices") {
    Graph<int> g;
    g.addVertex(1);
    g.addVertex(2);
    g.addVertex(3);
    std::vector<int> v = g.vertices();
    REQUIRE( v.size() == 3 );
    REQUIRE( std::find(v.begin(), v.end(), 1) != v.end() );
    REQUIRE( std::find(v.begin(), v.end(), 2) != v.end() );
    REQUIRE( std::find(v.begin(), v.end(), 3) != v.end() );
  }

  SECTION("iterate over vertices") {
    Graph<int> g;
    g.addVertex(1);
    g.addVertex(2);
    g.addVertex(3);

    std::map<int, int> found;
    for (auto it : g) {
      REQUIRE( contains(g, it) == true );
      ++found[it];
    }
    REQUIRE( found.size() == 3 );
  }*/
}