#include <graph/graph.hpp>

#include "../catch.hpp"

#include "fixture.hpp"

#include <map>
#include <type_traits>

TEST_CASE( "Graph creation", "[graph][data_structure]" ) {

  SECTION("rule of 3+2") {
    REQUIRE ( std::is_default_constructible<Graph<int> >::value == true);
    REQUIRE ( std::is_copy_constructible<Graph<int> >::value == true);
    REQUIRE ( std::is_move_constructible<Graph<int> >::value == true);

    Graph<int> g1 = { 1, 2, 3 };
    Graph<int> g2 = { 4, 5, 6 };
    Graph<int> g3(g1); // cctor
    REQUIRE ( g3 == g1 );

    // catch does not likes templates in REQUIRE
    constexpr bool is_assignable = std::is_assignable<Graph<int>, Graph<int> >::value;
    REQUIRE ( is_assignable == true);
    REQUIRE ( std::is_copy_assignable<Graph<int> >::value == true);
    REQUIRE ( std::is_move_assignable<Graph<int> >::value == true);

    g3 = g2; // assign
    REQUIRE ( g3 == g2 );

    Graph<int> g4 = g1; // cctor
    Graph<int> g5 = std::move(g1); // move ctor, g1 become invalid
    REQUIRE ( g4 == g5 );

    Graph<int> g6 = g2; // cctor
    g1 = std::move(g2); // move assign
    REQUIRE ( g1 == g6 );
  }

  SECTION("Initial state") {
    Graph<int> g;
    REQUIRE( empty(g) == true );
    REQUIRE( numberOfVertices(g) == 0 );
    REQUIRE( numberOfEdges(g) == 0 );
  }

  SECTION("Initializer list of vertices") {
    Graph<int> g = { 1, 2, 3 };
    REQUIRE( empty(g) == false );
    REQUIRE( numberOfVertices(g) == 3 );
    REQUIRE( numberOfEdges(g) == 0 );

    REQUIRE( contains(g, 1) == true );
    REQUIRE( contains(g, 2) == true );
    REQUIRE( contains(g, 3) == true );
  }

  SECTION("Initializer list of vertices") {
    Graph<int> g = { {1, 2}, {1, 3}, {3, 4} };
    REQUIRE( empty(g) == false );
    REQUIRE( numberOfVertices(g) == 4 );
    REQUIRE( numberOfEdges(g) == 3*2 );

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

    REQUIRE( contains(g2, 1) == true );
    REQUIRE( contains(g2, 2) == true );
    REQUIRE( contains(g2, 3) == true );
    REQUIRE( contains(g2, 4) == true );
  }
}

TEST_CASE( "Graph equality", "[graph][data_structure]" ) {

  SECTION("Simple comparisions") {
    Graph<int> g;
    Graph<int> g1 = { {1, 2}, {1, 3}, {3, 4} };
    Graph<int> g2 = { {1, 3}, {3, 4}, {1, 2} }; // shuffled g1
    Graph<int> g3 = { {3, 1}, {4, 3}, {1, 2} }; // shuffled g1 with switched src,dst
    REQUIRE( g == g );
    REQUIRE( g != g1 );
    REQUIRE( g1 == g2 );
    REQUIRE( g1 == g3 );
  }

}

TEST_CASE( "Graph adding vertices", "[graph][data_structure]" ) {

  SECTION("One element") {
    Graph<int> g;
    g.addVertex(1);
    REQUIRE( empty(g) == false );
    REQUIRE( numberOfVertices(g) == 1 );
    REQUIRE( numberOfEdges(g) == 0 );
  }

  SECTION("Removing the only element") {
    Graph<int> g;
    g.addVertex(1);
    g.removeVertex(1);
    REQUIRE( empty(g) == true );
    REQUIRE( numberOfVertices(g) == 0 );
    REQUIRE( numberOfEdges(g) == 0 );
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

  SECTION("Modify vertex to the same") {
    Graph<int> g;
    g.addVertex(1);
    REQUIRE( contains(g, 1) == true );

    g.modifyVertex(1, 1);
    REQUIRE( contains(g, 1) == true );
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

  SECTION("neighboursOf") {
    Graph<int> g = { {1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4} };
    REQUIRE( numberOfVertices(g) == 4 );
    REQUIRE( numberOfEdges(g) == 4*3 );
    const std::vector<int> n = g.neighboursOf(3);
    REQUIRE( std::find(n.begin(), n.end(), 1) != n.end() );
    REQUIRE( std::find(n.begin(), n.end(), 2) != n.end() );
    REQUIRE( std::find(n.begin(), n.end(), 4) != n.end() );
  }
}

TEST_CASE( "Graph adding edges", "[graph][data_structure]" ) {

  SECTION("One element") {
    Graph<int> g;
    g.addEdge(1, 2);
    REQUIRE( empty(g) == false );
    REQUIRE( numberOfVertices(g) == 2 );
    REQUIRE( numberOfEdges(g) == 1*2 );
  }

  SECTION("Removing the only edge") {
    Graph<int> g;
    g.addEdge(1, 2);
    g.removeEdge(1, 2);
    REQUIRE( empty(g) == false );
    REQUIRE( numberOfVertices(g) == 2 );
    REQUIRE( numberOfEdges(g) == 0 );
  }

  SECTION("Adding some edges, removing one by one") {
    Graph<int> g = { {1, 2}, {1, 3}, {3, 4} };
    REQUIRE( numberOfEdges(g) == 3*2 );
    g.removeEdge(1, 2);
    REQUIRE( numberOfEdges(g) == 2*2 );
    g.removeEdge(1, 3);
    REQUIRE( numberOfEdges(g) == 1*2 );
    g.removeEdge(3, 4);
    REQUIRE( numberOfEdges(g) == 0 );
  }

  SECTION("Adding some edges, one edge mentioned from both directions") {
    Graph<int> g = { {1, 2}, {2, 1} };
    REQUIRE( numberOfVertices(g) == 2 );
    REQUIRE( numberOfEdges(g) == 1*2 );
  }

  SECTION("Adding some edges, one edge mentioned from both directions v2") {
    Graph<int> g = { {1, 2}, {1, 3}, {1, 4},
                     {2, 1}, {2, 3}, {2, 4},
                     {3, 1}, {3, 2}, {3, 4},
                     {4, 1}, {4, 2}, {4, 3} };
    REQUIRE( numberOfVertices(g) == 4 );
    REQUIRE( numberOfEdges(g) == 4*3 );
  }

  SECTION("Adding some edges then clear") {
    Graph<int> g = { {1, 2}, {1, 3}, {3, 4} };
    g.clear();
    REQUIRE( numberOfVertices(g) == 0 );
  }

  SECTION("Removing unexisting edge") {
    Graph<int> g = { {1, 2}, {1, 3}, {3, 4} };
    REQUIRE( numberOfEdges(g) == 3*2 );
    g.removeEdge(1, 4);
    REQUIRE( numberOfEdges(g) == 3*2 );
  }

  SECTION("Removing edge with unexisting source") {
    Graph<int> g = { {1, 2}, {1, 3}, {3, 4} };
    REQUIRE( numberOfEdges(g) == 3*2 );
    g.removeEdge(5, 4);
    REQUIRE( numberOfEdges(g) == 3*2 );
  }

  SECTION("Removing edge with unexisting destination") {
    Graph<int> g = { {1, 2}, {1, 3}, {3, 4} };
    REQUIRE( numberOfEdges(g) == 3*2 );
    g.removeEdge(1, 5);
    REQUIRE( numberOfEdges(g) == 3*2 );
  }

  SECTION("connected vertices") {
    Graph<int> g = { {1, 2}, {1, 3}, {3, 4} };
    REQUIRE( connected(g, 1, 2) == true );
    REQUIRE( connected(g, 1, 3) == true );
    REQUIRE( connected(g, 3, 4) == true );
    REQUIRE( connected(g, 1, 4) == false );
    REQUIRE( connected(g, 2, 3) == false );
  }

  SECTION("set edges") {
    Graph<int> g = { {1, 2}, {1, 3}, {3, 4} };
    g.setEdges(4, {1, 2, 5});
    REQUIRE( connected(g, 4, 1) == true );
    REQUIRE( connected(g, 4, 2) == true );
    REQUIRE( connected(g, 4, 3) == false ); // not anymore
    REQUIRE( connected(g, 4, 4) == false );
    REQUIRE( connected(g, 4, 5) == true );
  }

  SECTION("get array of edges") {
    Graph<int> g = { {1, 2}, {1, 3}, {3, 4} };
    auto e = edges(g);
    REQUIRE( e.size() == 3*2 );
  }
}

TEST_CASE( "Graph specials", "[graph][data_structure]" ) {

  SECTION("not directed") {
    Graph<int> g;
    g.addEdge(1, 2);
    REQUIRE( connected(g, 1, 2) == true );
    REQUIRE( connected(g, 2, 1) == true );
  }

  SECTION("no multiedges") {
    Graph<int> g;
    g.addEdge(1, 2);
    REQUIRE( connected(g, 1, 2) == true );
    REQUIRE( connected(g, 2, 1) == true );
    REQUIRE( numberOfEdges(g) == 2 );

    g.addEdge(1, 2);
    g.addEdge(1, 2);
    REQUIRE( numberOfEdges(g) == 2 );

    g.addEdge(2, 1);
    REQUIRE( numberOfEdges(g) == 2 );
  }

  SECTION("no self-edegs") {
    Graph<int> g;
    g.addEdge(1, 2);
    REQUIRE( connected(g, 1, 2) == true );
    REQUIRE( connected(g, 2, 1) == true );
    REQUIRE( numberOfEdges(g) == 2 );

    g.addEdge(1, 1);
    REQUIRE( numberOfEdges(g) == 2 );

    g.addEdge(2, 2);
    REQUIRE( numberOfEdges(g) == 2 );
  }
}

TEST_CASE( "Graph std::string vertices", "[graph][data_structure]" ) {

  SECTION("Initial state") {
    Graph<std::string> g;
    REQUIRE( empty(g) == true );
  }

  SECTION("Adding some edges") {
    Graph<std::string> g;
    g.addEdge("alfa", "beta");
    g.addEdge("alfa", "omega");
    g.addEdge("beta", "omega");

    REQUIRE( empty(g) == false );
    REQUIRE( numberOfVertices(g) == 3 );
    REQUIRE( numberOfEdges(g) == 3*2 );
  }

}


class A {
public:
  A(const std::string& s) : m_s(s) {}
  std::string getS() const { return m_s; }
  bool operator==(const A& o) const { return m_s == o.m_s; }
private:
  std::string m_s;
};

namespace std {
template <>
struct hash<A> {
  std::size_t operator()(const A& a) const {
    return std::hash<std::string>()(a.getS());
  }
};
}

TEST_CASE( "Graph custom vertices", "[graph][data_structure]" ) {

  SECTION("Initial state") {
    Graph<A> g;
    REQUIRE( empty(g) == true );
  }

  SECTION("Adding some edges") {
    Graph<A> g;
    g.addEdge(A("alfa"), A("beta"));
    g.addEdge(A("alfa"), A("omega"));
    g.addEdge(A("beta"), A("omega"));
    REQUIRE( empty(g) == false );
    REQUIRE( numberOfVertices(g) == 3 );
    REQUIRE( numberOfEdges(g) == 3*2 );
  }

}

TEST_CASE( "Graph adding/removing with more data", "[graph][data_structure]" ) {

  SECTION("Adding some edges") {
    Graph<int> g = { {1, 2}, {1, 3}, {1, 4}, {2, 4}, {3, 4} };
    REQUIRE( numberOfVertices(g) == 4 );
    REQUIRE( numberOfEdges(g) == 5*2 );

    REQUIRE( connected(g, 1, 2) == true );
    REQUIRE( connected(g, 1, 3) == true );
    REQUIRE( connected(g, 1, 4) == true );
    REQUIRE( connected(g, 2, 1) == true );
    REQUIRE( connected(g, 2, 4) == true );
    REQUIRE( connected(g, 3, 1) == true );
    REQUIRE( connected(g, 3, 4) == true );
    REQUIRE( connected(g, 4, 1) == true );
    REQUIRE( connected(g, 4, 2) == true );
    REQUIRE( connected(g, 4, 3) == true );
  }

  SECTION("Adding some edges, removing 1 vertex") {
    Graph<int> g = { {1, 2}, {1, 3}, {1, 4}, {2, 4}, {3, 4} };
    g.removeVertex(1);
    REQUIRE( numberOfVertices(g) == 3 );
    REQUIRE( numberOfEdges(g) == 2*2 );
    REQUIRE( connected(g, 1, 2) == false );
    REQUIRE( connected(g, 1, 3) == false );
    REQUIRE( connected(g, 1, 4) == false );
    REQUIRE( connected(g, 2, 1) == false );
    REQUIRE( connected(g, 2, 4) == true );
    REQUIRE( connected(g, 3, 1) == false );
    REQUIRE( connected(g, 3, 4) == true );
    REQUIRE( connected(g, 4, 1) == false );
    REQUIRE( connected(g, 4, 2) == true );
    REQUIRE( connected(g, 4, 3) == true );
  }

  SECTION("Adding some edges, removing 1 unexisting vertex") {
    Graph<int> g = { {1, 2}, {1, 3}, {1, 4}, {2, 4}, {3, 4} };
    g.removeVertex(5);
    REQUIRE( numberOfVertices(g) == 4 );
    REQUIRE( numberOfEdges(g) == 5*2 );

    REQUIRE( connected(g, 1, 2) == true );
    REQUIRE( connected(g, 1, 3) == true );
    REQUIRE( connected(g, 1, 4) == true );
    REQUIRE( connected(g, 2, 1) == true );
    REQUIRE( connected(g, 2, 4) == true );
    REQUIRE( connected(g, 3, 1) == true );
    REQUIRE( connected(g, 3, 4) == true );
    REQUIRE( connected(g, 4, 1) == true );
    REQUIRE( connected(g, 4, 2) == true );
    REQUIRE( connected(g, 4, 3) == true );
  }

  SECTION("Adding some edges, removing 1 edge") {
    Graph<int> g = { {1, 2}, {1, 3}, {1, 4}, {2, 4}, {3, 4} };
    REQUIRE( numberOfVertices(g) == 4 );
    REQUIRE( numberOfEdges(g) == 5*2 );

    g.removeEdge(2, 4);
    REQUIRE( numberOfVertices(g) == 4 );
    REQUIRE( numberOfEdges(g) == 4*2 );
    REQUIRE( connected(g, 1, 2) == true );
    REQUIRE( connected(g, 1, 3) == true );
    REQUIRE( connected(g, 1, 4) == true );
    REQUIRE( connected(g, 2, 1) == true );
    REQUIRE( connected(g, 2, 4) == false );
    REQUIRE( connected(g, 3, 1) == true );
    REQUIRE( connected(g, 3, 4) == true );
    REQUIRE( connected(g, 4, 1) == true );
    REQUIRE( connected(g, 4, 2) == false );
    REQUIRE( connected(g, 4, 3) == true );
  }

  SECTION("Adding some edges, removing 1 unexisting edge") {
    Graph<int> g = { {1, 2}, {1, 3}, {1, 4}, {2, 4}, {3, 4} };
    REQUIRE( numberOfVertices(g) == 4 );
    REQUIRE( numberOfEdges(g) == 5*2 );

    g.removeEdge(3, 2);
    REQUIRE( numberOfVertices(g) == 4 );
    REQUIRE( numberOfEdges(g) == 5*2 );

    REQUIRE( connected(g, 1, 2) == true );
    REQUIRE( connected(g, 1, 3) == true );
    REQUIRE( connected(g, 1, 4) == true );
    REQUIRE( connected(g, 2, 1) == true );
    REQUIRE( connected(g, 2, 4) == true );
    REQUIRE( connected(g, 3, 1) == true );
    REQUIRE( connected(g, 3, 4) == true );
    REQUIRE( connected(g, 4, 1) == true );
    REQUIRE( connected(g, 4, 2) == true );
    REQUIRE( connected(g, 4, 3) == true );
  }

  SECTION("Adding some edges, modifying 1 vertex") {
    Graph<int> g = { {1, 2}, {1, 3}, {1, 4}, {2, 4}, {3, 4} };
    REQUIRE( numberOfVertices(g) == 4 );
    REQUIRE( numberOfEdges(g) == 5*2 );

    g.modifyVertex(1, 5);
    REQUIRE( numberOfVertices(g) == 4 );
    REQUIRE( numberOfEdges(g) == 5*2 );
    REQUIRE( contains(g, 1) == false );
    REQUIRE( contains(g, 5) == true );

    REQUIRE( connected(g, 1, 2) == false );
    REQUIRE( connected(g, 1, 3) == false );
    REQUIRE( connected(g, 1, 4) == false );
    REQUIRE( connected(g, 2, 1) == false );
    REQUIRE( connected(g, 2, 4) == true );
    REQUIRE( connected(g, 3, 1) == false );
    REQUIRE( connected(g, 3, 4) == true );
    REQUIRE( connected(g, 4, 1) == false );
    REQUIRE( connected(g, 4, 2) == true );
    REQUIRE( connected(g, 4, 3) == true );

    REQUIRE( connected(g, 5, 2) == true );
    REQUIRE( connected(g, 5, 3) == true );
    REQUIRE( connected(g, 5, 4) == true );
    REQUIRE( connected(g, 2, 5) == true );
    REQUIRE( connected(g, 3, 5) == true );
    REQUIRE( connected(g, 4, 5) == true );
  }

  SECTION("Adding some edges, modifying 1 unexisting vertex") {
    Graph<int> g = { {1, 2}, {1, 3}, {1, 4}, {2, 4}, {3, 4} };
    REQUIRE( numberOfVertices(g) == 4 );
    REQUIRE( numberOfEdges(g) == 5*2 );

    g.modifyVertex(5, 6);
    REQUIRE( numberOfVertices(g) == 4 );
    REQUIRE( numberOfEdges(g) == 5*2 );
    REQUIRE( contains(g, 5) == false );
    REQUIRE( contains(g, 6) == false );
  }
}


TEST_CASE_METHOD(Fixture<float2>, "Graph performance", "[graph][data_structure][performance]" ) {

  constexpr std::size_t number_of_rows = 100;
  constexpr std::size_t number_of_columns = 100;
  constexpr std::size_t number_of_vertices = number_of_rows * number_of_columns;
  constexpr std::size_t number_of_edges = numberOfEdges(number_of_rows, number_of_columns);

  Fixture<float2>::initOnce(number_of_rows, number_of_columns);


  SECTION("Adding vertices") {
    const std::vector<float2> vertices = Fixture<float2>::getVertices();
    Graph<float2> g(vertices);
    REQUIRE( numberOfVertices(g) == number_of_vertices );
  }

  SECTION("Adding edges") {
    const std::vector<typename Graph<float2>::Edge> edges = Fixture<float2>::getEdges();
    Graph<float2> g(edges);
    REQUIRE( numberOfEdges(g) == number_of_edges );
  }

  SECTION("teardown") {
    Fixture<float2>::tearDown();
  }
}

