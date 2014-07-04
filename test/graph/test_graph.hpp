#include <cxxtest/TestSuite.h>

#include <graph/graphwd.hpp>

// #include <graph/graph_algorithms.hpp>
// #include <graph/mesh_algorithms.hpp>
// #include <graph/graph_xml.hpp>
// #include <graph/common.hpp>

namespace std {
  template <>
  struct hash<std::pair<float,float> >
  {
    std::size_t operator()(const std::pair<float,float>& k) const {
      std::size_t h1 = std::hash<float>()(k.first);
      std::size_t h2 = std::hash<float>()(k.second);
      return h1 ^ (h2 << 1);
    }
  };
}



namespace test { class TestSubsystem1DummyClassSuite; }

class test::TestSubsystem1DummyClassSuite : public CxxTest::TestSuite
{

public:

  void test_trivial()
  {
    GraphWD<std::string> g(false);
    TS_ASSERT_EQUALS ( g.directed(), false );

    GraphWD<int> g1;
    TS_ASSERT_EQUALS ( g1.directed(), true );

    GraphWD<float, std::string> g2;
    TS_ASSERT_EQUALS ( g2.empty(), true );

    GraphWD<std::pair<float, float> ,float> g3;
    TS_ASSERT_EQUALS ( g3.numberOfVertices(), 0 );
    TS_ASSERT_EQUALS ( g3.numberOfEdges(), 0 );
    TS_ASSERT_EQUALS ( g3.contains(std::make_pair(2.4, 5.567)), false );
    TS_ASSERT_EQUALS ( g3.vertices().size(), 0 );
    TS_ASSERT_EQUALS ( g3.edges().size(), 0 );
    TS_ASSERT_EQUALS ( g3.empty(), true );
  }

  void test_basic_vertices()
  {
    GraphWD<std::string> g;

    g.addVertex("a");
    g.addVertex("b");
    g.addVertex("c");
    TS_ASSERT_EQUALS ( g.vertices().size(), 3 );

    g.addVertex("d");
    g.addVertex("d");
    TS_ASSERT_EQUALS ( g.vertices().size(), 4 );

    g.removeVertex("f");
    TS_ASSERT_EQUALS ( g.vertices().size(), 4 );

    g.removeVertex("c");
    TS_ASSERT_EQUALS ( g.vertices().size(), 3 );
    g.removeVertex("c");
    TS_ASSERT_EQUALS ( g.vertices().size(), 3 );

    for(const std::string& s : g.vertices())
      g.removeVertex(s);

    TS_ASSERT_EQUALS ( g.vertices().size(), 0 );

    GraphWD<std::string> g2 = { "a", "b", "c" };
    TS_ASSERT_EQUALS ( g2.vertices().size(), 3 );

  }

  void test_basic_edges()
  {
    GraphWD<std::string> g = { "a", "a",  "b", "c", "d", "e" };
    g.addEdge("a", "a", 1);
    g.addEdge("a", "b", 2);
    g.addEdge("a", "c", 3);

    g.addEdge("c", "d", 4);

    TS_ASSERT_EQUALS ( g.edges().size(), 4 );
    TS_ASSERT_EQUALS ( g.neighboursOf("a").size(), 3 );
    TS_ASSERT_EQUALS ( g.neighboursOf("b").size(), 0 );
    TS_ASSERT_EQUALS ( g.neighboursOf("c").size(), 1 );
    TS_ASSERT_EQUALS ( g.neighboursOf("d").size(), 0 );
    TS_ASSERT_EQUALS ( g.neighboursOf("e").size(), 0 );

   std::vector<std::string> n = g.neighboursOf("a");
    TS_ASSERT_DIFFERS ( find(n.begin(), n.end(), "a"), n.end() );
    TS_ASSERT_DIFFERS ( find(n.begin(), n.end(), "b"), n.end() );
    TS_ASSERT_DIFFERS ( find(n.begin(), n.end(), "c"), n.end() );
    TS_ASSERT_EQUALS ( find(n.begin(), n.end(), "d"), n.end() );
    TS_ASSERT_EQUALS ( find(n.begin(), n.end(), "e"), n.end() );

    std::vector<int> w = g.weights("a", "b");
    TS_ASSERT_EQUALS ( w.size(), 1 );
    TS_ASSERT_EQUALS ( w[0], 2 );

    // undirected
    GraphWD<std::string> g1(false);
    g1.addVertex("a");
    g1.addVertex("b");
    g1.addVertex("c");
    g1.addVertex("d");
    g1.addVertex("e");

    g1.addEdge("a", "a", 1);
    g1.addEdge("a", "b", 2);
    g1.addEdge("a", "c", 3);
    g1.addEdge("c", "d", 4);

    TS_ASSERT_EQUALS ( g1.edges().size(), 4*2 - 1 );
    TS_ASSERT_EQUALS ( g1.neighboursOf("a").size(), 3 );
    TS_ASSERT_EQUALS ( g1.neighboursOf("b").size(), 1 );
    TS_ASSERT_EQUALS ( g1.neighboursOf("c").size(), 2 );
    TS_ASSERT_EQUALS ( g1.neighboursOf("d").size(), 1 );
    TS_ASSERT_EQUALS ( g1.neighboursOf("e").size(), 0 );
  }

  void test_list_ctor_from_vertices()
  {
    GraphWD<std::string> g = { "a", "a",  "b", "c", "d", "e" };

    std::vector<std::string> v = g.vertices();

    TS_ASSERT_EQUALS ( v.size(), 5 );
  }

  void test_move_ctor()
  {

    GraphWD<std::string> g = {
      {"a", "a", 1},
      {"a", "b", 2},
      {"a", "c", 3},
      {"c", "d", 4} };

    GraphWD<std::string> g1;
    g1 = g;

    TS_ASSERT_EQUALS ( g.edges().size(), 4 );
    TS_ASSERT_EQUALS ( g.neighboursOf("a").size(), 3 );
    TS_ASSERT_EQUALS ( g.neighboursOf("b").size(), 0 );
    TS_ASSERT_EQUALS ( g.neighboursOf("c").size(), 1 );
    TS_ASSERT_EQUALS ( g.neighboursOf("d").size(), 0 );

    TS_ASSERT_EQUALS ( g1.edges().size(), 4 );
    TS_ASSERT_EQUALS ( g1.neighboursOf("a").size(), 3 );
    TS_ASSERT_EQUALS ( g1.neighboursOf("b").size(), 0 );
    TS_ASSERT_EQUALS ( g1.neighboursOf("c").size(), 1 );
    TS_ASSERT_EQUALS ( g1.neighboursOf("d").size(), 0 );
  }

  void test_list_ctor_from_edges()
  {

    GraphWD<std::string> g = {
      {"a", "a", 1},
      {"a", "b", 2},
      {"a", "c", 3},
      {"c", "d", 4} };

    TS_ASSERT_EQUALS ( g.vertices().size(), 4 );
    TS_ASSERT_EQUALS ( g.edges().size(), 4 );

    TS_ASSERT_EQUALS ( g.neighboursOf("a").size(), 3 );
    TS_ASSERT_EQUALS ( g.neighboursOf("b").size(), 0 );
    TS_ASSERT_EQUALS ( g.neighboursOf("c").size(), 1 );
    TS_ASSERT_EQUALS ( g.neighboursOf("d").size(), 0 );
    TS_ASSERT_EQUALS ( g.neighboursOf("e").size(), 0 );
  }

  void test_adding_and_removing_vertices()
  {
    GraphWD<std::string> g;
    g.addVertex("a");
    g.addVertex("b");
    g.addVertex("c");
    g.addVertex("d");
    g.addVertex("e");
    g.addVertex("f");
    g.addVertex("g");

    g.addEdge("a", "a", 1);
    g.addEdge("a", "b", 2);
    g.addEdge("a", "c", 3);
    g.addEdge("c", "d", 4);
    g.addEdge("c", "f", 5);
    g.addEdge("f", "e", 6);

    TS_ASSERT_EQUALS ( g.vertices().size(), 7 );
    TS_ASSERT_EQUALS ( g.edges().size(), 6 );

    g.removeVertex("c");

    TS_ASSERT_EQUALS ( g.vertices().size(), 7-1 );
    TS_ASSERT_EQUALS ( g.edges().size(), 6-3 );

    std::vector<GraphWD<std::string>::Edge> es = g.edges();
//     TS_ASSERT_EQUALS ( es[0].source, "a" );
//     TS_ASSERT_EQUALS ( es[0].destination, "a" );
//     TS_ASSERT_EQUALS ( es[0].weight, 1 );
//
//     TS_ASSERT_EQUALS ( es[1].source, "a" );
//     TS_ASSERT_EQUALS ( es[1].destination, "b" );
//     TS_ASSERT_EQUALS ( es[1].weight, 2 );
//
//     TS_ASSERT_EQUALS ( es[2].source, "f" );
//     TS_ASSERT_EQUALS ( es[2].destination, "e" );
//     TS_ASSERT_EQUALS ( es[2].weight, 6 );
  }

  void test_removing_edges()
  {

    GraphWD<std::string> g;
    g.addVertex("a");
    g.addVertex("b");
    g.addVertex("c");
    g.addVertex("d");
    g.addVertex("e");
    g.addVertex("f");
    g.addVertex("g");

    g.addEdge("a", "a", 1);
    g.addEdge("a", "b", 21); // multiple to b
    g.addEdge("a", "b", 22);
    g.addEdge("a", "b", 23);
    g.addEdge("a", "c", 3);
    g.addEdge("c", "d", 4);
    g.addEdge("c", "f", 5);
    g.addEdge("f", "e", 6);

    g.removeEdge("c", "d", 4);
    g.removeEdge("e", "f", 1234);

    g.removeEdges("a", "b");
    g.removeEdges("invalid", "b");

    TS_ASSERT_EQUALS( g.vertices().size(), 7 );
    TS_ASSERT_EQUALS ( g.edges().size(), 4 );

//     int j; GraphWD<std::string>::edge_iterator e_it;
//     for (j = 0, e_it = g.edge_begin();
//          e_it != g.edge_end();
//          ++e_it, ++j)
//       switch (j) {
//         case 0: TS_ASSERT_EQUALS (e_it->source, "a");
//                 TS_ASSERT_EQUALS (e_it->destination, "a");
//                 TS_ASSERT_EQUALS (e_it->weight, 1);
//                 break;
//         case 1: TS_ASSERT_EQUALS (e_it->source, "a");
//                 TS_ASSERT_EQUALS (e_it->destination, "c");
//                 TS_ASSERT_EQUALS (e_it->weight, 3);
//                 break;
//         case 2: TS_ASSERT_EQUALS (e_it->source, "c");
//                 TS_ASSERT_EQUALS (e_it->destination, "f");
//                 TS_ASSERT_EQUALS (e_it->weight, 5);
//                 break;
//         case 3: TS_ASSERT_EQUALS (e_it->source, "f");
//                 TS_ASSERT_EQUALS (e_it->destination, "e");
//                 TS_ASSERT_EQUALS (e_it->weight, 6);
//                 break;
//     }
  }

  void test_iterator()
  {

    GraphWD<std::string> g;
    g.addVertex("a");
    g.addVertex("b");
    g.addVertex("c");
    g.addVertex("d");
    g.addVertex("e");
    g.addVertex("f");
    g.addVertex("g");

    g.addEdge("a", "a", 1);
    g.addEdge("a", "b", 2);
    g.addEdge("a", "c", 3);
    g.addEdge("c", "d", 4);
    g.addEdge("c", "f", 5);
    g.addEdge("f", "e", 6);

//     int i; GraphWD<std::string>::vertex_iterator v_it;
//     for (i = 0, v_it = g.begin();
//          v_it != g.end();
//          ++v_it, ++i)
//       switch (i) {
//         case 0: TS_ASSERT_EQUALS (*v_it, "a"); break;
//         case 1: TS_ASSERT_EQUALS (*v_it, "b"); break;
//         case 2: TS_ASSERT_EQUALS (*v_it, "c"); break;
//         case 3: TS_ASSERT_EQUALS (*v_it, "d"); break;
//         case 4: TS_ASSERT_EQUALS (*v_it, "e"); break;
//         case 5: TS_ASSERT_EQUALS (*v_it, "f"); break;
//         case 6: TS_ASSERT_EQUALS (*v_it, "g"); break;
//       }

//     int j; GraphWD<std::string>::edge_iterator e_it;
//     for (j = 0, e_it = g.edge_begin();
//          e_it != g.edge_end();
//          ++e_it, ++j)
//       switch (j) {
//         case 0: TS_ASSERT_EQUALS (e_it->source, "a");
//                 TS_ASSERT_EQUALS (e_it->destination, "a");
//                 TS_ASSERT_EQUALS (e_it->weight, 1);
//                 break;
//         case 1: TS_ASSERT_EQUALS (e_it->source, "a");
//                 TS_ASSERT_EQUALS (e_it->destination, "b");
//                 TS_ASSERT_EQUALS (e_it->weight, 2);
//                 break;
//         case 2: TS_ASSERT_EQUALS (e_it->source, "a");
//                 TS_ASSERT_EQUALS (e_it->destination, "c");
//                 TS_ASSERT_EQUALS (e_it->weight, 3);
//                 break;
//         case 3: TS_ASSERT_EQUALS (e_it->source, "c");
//                 TS_ASSERT_EQUALS (e_it->destination, "d");
//                 TS_ASSERT_EQUALS (e_it->weight, 4);
//                 break;
//         case 4: TS_ASSERT_EQUALS (e_it->source, "c");
//                 TS_ASSERT_EQUALS (e_it->destination, "f");
//                 TS_ASSERT_EQUALS (e_it->weight, 5);
//                 break;
//         case 5: TS_ASSERT_EQUALS (e_it->source, "f");
//                 TS_ASSERT_EQUALS (e_it->destination, "e");
//                 TS_ASSERT_EQUALS (e_it->weight, 6);
//                 break;
//       }

//     g.removeVertex("c");
//
//     for (i = 0, v_it = g.vertex_begin();
//          v_it != g.vertex_end();
//          ++v_it, ++i)
//       switch (i) {
//         case 0: TS_ASSERT_EQUALS (*v_it, "a"); break;
//         case 1: TS_ASSERT_EQUALS (*v_it, "b"); break;
//         case 2: TS_ASSERT_EQUALS (*v_it, "d"); break;
//         case 3: TS_ASSERT_EQUALS (*v_it, "e"); break;
//         case 4: TS_ASSERT_EQUALS (*v_it, "f"); break;
//         case 5: TS_ASSERT_EQUALS (*v_it, "g"); break;
//       }
//
//     for (j = 0, e_it = g.edge_begin();
//          e_it != g.edge_end();
//          ++e_it, ++j)
//       switch (j) {
//         case 0: TS_ASSERT_EQUALS (e_it->source, "a");
//                 TS_ASSERT_EQUALS (e_it->destination, "a");
//                 TS_ASSERT_EQUALS (e_it->weight, 1);
//                 break;
//         case 1: TS_ASSERT_EQUALS (e_it->source, "a");
//                 TS_ASSERT_EQUALS (e_it->destination, "b");
//                 TS_ASSERT_EQUALS (e_it->weight, 2);
//                 break;
//         case 2: TS_ASSERT_EQUALS (e_it->source, "f");
//                 TS_ASSERT_EQUALS (e_it->destination, "e");
//                 TS_ASSERT_EQUALS (e_it->weight, 6);
//                 break;
//       }
  }

  void test_wights()
  {

    GraphWD<std::string> g;
    g.addVertex("a");
    g.addVertex("b");
    g.addVertex("c");
    g.addVertex("d");
    g.addVertex("e");
    g.addVertex("f");
    g.addVertex("g");

    g.addEdge("a", "a", 1);
    g.addEdge("a", "b", 2);
    g.addEdge("a", "c", 3);
    g.addEdge("c", "d", 4);
    g.addEdge("c", "f", 5);
    g.addEdge("f", "e", 6);

    TS_ASSERT_EQUALS ( g.weights("a", "a").size(), 1 );
    TS_ASSERT_EQUALS ( g.weights("a", "b").size(), 1 );
    TS_ASSERT_EQUALS ( g.weights("a", "c").size(), 1 );
    TS_ASSERT_EQUALS ( g.weights("c", "d").size(), 1 );
    TS_ASSERT_EQUALS ( g.weights("f", "e").size(), 1 );

    TS_ASSERT_EQUALS ( g.weights("a", "d").size(), 0 );
    TS_ASSERT_EQUALS ( g.weights("a", "f").size(), 0 );
    TS_ASSERT_EQUALS ( g.weights("a", "e").size(), 0 );
    TS_ASSERT_EQUALS ( g.weights("a", "d").size(), 0 );
    TS_ASSERT_EQUALS ( g.weights("d", "e").size(), 0 );
    TS_ASSERT_EQUALS ( g.weights("b", "f").size(), 0 );

    // directed
    TS_ASSERT_EQUALS ( g.weights("a", "a").size(), 1 );
    TS_ASSERT_EQUALS ( g.weights("b", "b").size(), 0 );
    TS_ASSERT_EQUALS ( g.weights("c", "a").size(), 0 );
    TS_ASSERT_EQUALS ( g.weights("d", "c").size(), 0 );
    TS_ASSERT_EQUALS ( g.weights("f", "c").size(), 0 );
    TS_ASSERT_EQUALS ( g.weights("e", "f").size(), 0 );
  }

//   /// @todo put algorithm tests to another suite
//   void test_dijkstra()
//   {
//
//     // from http://en.wikipedia.org/wiki/File:Dijkstra_Animation.gif
//     GraphWDWD<int> d(false);
//     d.addVertex(1);
//     d.addVertex(2);
//     d.addVertex(3);
//     d.addVertex(4);
//     d.addVertex(5);
//     d.addVertex(6);
//
//     d.addVertex(7); // unconnected extra
//     d.addVertex(8);
//     d.addVertex(9);
//
//     d.addEdge(1, 2, 7);
//     d.addEdge(1, 3, 9);
//     d.addEdge(1, 6, 14);
//     d.addEdge(2, 3, 10);
//     d.addEdge(2, 4, 15);
//     d.addEdge(3, 4, 11);
//     d.addEdge(3, 6, 2);
//     d.addEdge(4, 5, 6);
//     d.addEdge(5, 6, 9);
//
//     d.addEdge(8, 9, 9);
//
//     std::pair<std::unordered_map< int, int>, std::unordered_map< int, int> > shortest_path = dijkstra_shortest_path_v2(d, 1);
//
//     TS_ASSERT_EQUALS ( shortest_path.first[1], 0 );
//     TS_ASSERT_EQUALS ( shortest_path.first[2], 7 );
//     TS_ASSERT_EQUALS ( shortest_path.first[3], 9 );
//     TS_ASSERT_EQUALS ( shortest_path.first[4], 20 );
//     TS_ASSERT_EQUALS ( shortest_path.first[5], 20 );
//     TS_ASSERT_EQUALS ( shortest_path.first[6], 11 );
//     TS_ASSERT_EQUALS ( shortest_path.first.size(), 6 );
//
//     TS_ASSERT_EQUALS ( shortest_path.second[2], 1 );
//     TS_ASSERT_EQUALS ( shortest_path.second[3], 1 );
//     TS_ASSERT_EQUALS ( shortest_path.second[4], 3 );
//     TS_ASSERT_EQUALS ( shortest_path.second[5], 6 );
//     TS_ASSERT_EQUALS ( shortest_path.second[6], 3 );
//     TS_ASSERT_EQUALS ( shortest_path.second.size(), 5 );
//   }
//
//   /// @todo put XML tests to another suite
//   void test_read_from_XML()
//   {
//
//     GraphWD< std::pair<float, float> > g_xml = readGraphWD<float>("/home/denes/projects/graph/lib/graph/graph_example.xml");
// //     for (std::pair<float, float> v : g_xml.vertices())
// //       std::cout << TToStr(v) << std::endl;
// //
// //     for (GraphWD< std::pair<float, float> >::Edge e : g_xml.edges())
// //       std::cout << TToStr(e.source) << " -> " << TToStr(e.destination) << std::endl;
//   }

//   {
   //   std::vector< std::pair<float, float> > points = {
//     {1, 1},
//   };

//   GraphWD< std::pair<float, float> > g_d = delaunay_triangulation(points);
//   }


};
