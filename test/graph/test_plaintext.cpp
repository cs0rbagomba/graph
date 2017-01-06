#include <graph/graph_plaintext.hpp>

#include "../catch.hpp"

#include "fixture.hpp"

#include <cstdio> // remove file

inline int intCreator(const std::string& s) { return std::stoi(s); }
inline std::string intSerializer(int i) { return std::to_string(i); }

/// @note is there a smareter way to do this?
inline std::string s2s(const std::string& s) { return s; }


TEST_CASE( "Plain text import/export", "[IO]" ) {

  SECTION("Invalid files") {
    const std::string root_file("/root/graph_dump.txt");
    const Graph<int> g1;
    CHECK_THROWS ( writeGraphToPlainText(g1, root_file, intSerializer) );
    CHECK_THROWS ( readGraphFromPlainText<int>(root_file, intCreator) );
  }

  SECTION("vertices are strings") {
    const std::string fileName("/tmp/graph_dump.txt");
    const std::vector<std::string> v{ "one", "two", "three", "... and four"};
    Graph<std::string> g1(v);
    g1.addEdge(v[0], v[1]);
    g1.addEdge(v[0], v[2]);
    g1.addEdge(v[2], v[3]);

    writeGraphToPlainText(g1, fileName, s2s);
    const Graph<std::string> g2 = readGraphFromPlainText<std::string>(fileName, s2s);
    REQUIRE ( g1 == g2 );

    remove(fileName.c_str());
  }

  SECTION("vertices are float coordinates") {
    const std::string fileName("/tmp/graph_dump.txt");
    const std::vector<typename Graph<float2>::Edge>* edges = createEdges<float2>(4, 4);
    const Graph<float2> g1(*edges);
    writeGraphToPlainText(g1, fileName, float2serializer);
    const Graph<float2> g2 = readGraphFromPlainText<float2>(fileName, float2creator);
    REQUIRE ( g1 == g2 );

    remove(fileName.c_str());
    delete edges;
  }
}
