#include <graph/graph.hpp>

#include "../catch.hpp"

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
    auto edges = g.edges();
    REQUIRE( edges.size() == 3*2 );
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

struct float2 {
  float x, y;

  inline float2() : x(0.0), y(0.0) {}
  inline float2(float f1, float f2) : x(f1), y(f2) {}
};

inline bool operator ==(const float2& v1, const float2& v2) { return v1.x == v2.x && v1.y == v2.y; }

namespace std {
  template <>
  struct hash<float2>
  {
    std::size_t operator()(const float2& f2) const
    {
      std::size_t h1 = std::hash<float>()(f2.x);
      std::size_t h2 = std::hash<float>()(f2.y);
      return h1 ^ (h2 << 1);
    }
  };
}

int numberOfEdges(int N, int M) {
  return (N-2)*(M-2)*8             // inside vertices have 8
          + 4*3                    // corners have 3
          + (N-2)*2*5 + (M-2)*2*5; // sides has 5
}

TEST_CASE( "Graph performance", "[graph][performance]" ) {

  SECTION("Adding 1000x1000 vertices") {

    const std::size_t N = 1000;
    const std::size_t M = 1000;
    Graph<float2> g;
    for (std::size_t i = 0; i < N; ++i)
      for (std::size_t j = 0; j < M; ++j)
        g.addVertex(float2(i, j));

    REQUIRE( numberOfVertices(g) == N*M );
  }

  SECTION("Adding 1000x1000 vertices and MANY edges") {

    /** Creating a big (N rows and M columns) grid.
     *  Every vertex is connexted to it's 8 neighbours.
     *
     *  +-+-+-+
     *  |x+x+x|
     *  +x+x+x+
     *  |x+x+x|
     *  +-+-+-+
     */

    const std::size_t N = 1000;
    const std::size_t M = 1000;
    Graph<float2> g;

    // inside
    for (std::size_t i = 1; i < N-1; ++i)
      for (std::size_t j = 1; j < M-1; ++j) {
        g.addEdge(float2(i, j), float2(i-1, j-1));
        g.addEdge(float2(i, j), float2(i-1, j));
        g.addEdge(float2(i, j), float2(i-1, j+1));
        g.addEdge(float2(i, j), float2(i,   j-1));
        g.addEdge(float2(i, j), float2(i,   j+1));
        g.addEdge(float2(i, j), float2(i+1, j-1));
        g.addEdge(float2(i, j), float2(i+1, j));
        g.addEdge(float2(i, j), float2(i+1, j+1));
      }

    REQUIRE( numberOfVertices(g) == N*M );
//     REQUIRE( numberOfEdges(g) == numberOfEdges(N, M)*2 );
  }

}