#include "graph.hpp"

#include <stdexcept>
#include <fstream>

// format: 1 line = 1 node
// first line followed by it's neighbours.
// separator is an empty new line
// std::string vertexSerializer(const V& v) converts V into std::string
// V vertexCreator(const std::string&s) converts std::string to V

template <typename V, typename F>
Graph<V> readGraphFromPlainText(const std::string& filename, F vertexCreator)
{
  std::ifstream file(filename);
  if (!file.good())
    throw std::runtime_error("Failed to open " + filename + " to read.");

  Graph<V> g;
  std::string line;
  while (std::getline(file, line))
    if (!line.empty())
      break;

  bool new_entry = true;
  V current_vertex;
  while (std::getline(file, line)) {
    if (line.empty()) {
      new_entry = true;
    } else {
      if (new_entry) {
        current_vertex = vertexCreator(line);
        g.addVertex(current_vertex);
        new_entry = false;
      } else {
        g.addEdge(current_vertex, vertexCreator(line));
      }
    }
  }
  return g;
}

template <typename V, typename F>
void writeGraphToPlainText(const Graph<V>& g, const std::string& filename, F vertexSerializer)
{
  std::ofstream file;
  file.open (filename);
  if (!file.is_open())
    throw std::runtime_error("Failed to open " + filename + " to write.");

  for (const auto cit : g) {
    file << vertexSerializer(cit) << std::endl;
    for (const auto cit2 : g.neighboursOf(cit))
      file << vertexSerializer(cit2) << std::endl;

    file << std::endl;
  }
  file.close();
}
