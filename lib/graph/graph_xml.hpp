#include <graph.hpp>

#include <stdexcept>
#include <fstream>

#include <libxml/parser.h>
#include <libxml/tree.h>

namespace {

template <typename V, typename F>
void readVertices(Graph<V>& g,  F vertexCreator, const xmlNodePtr root_element)
{
  for (xmlNodePtr cur_node = root_element->children; cur_node; cur_node = cur_node->next)
    if (cur_node->type == XML_ELEMENT_NODE) {

      const std::string vertex( reinterpret_cast<const char*>(cur_node->properties->children->content));
      V v = vertexCreator(vertex);

      std::vector<V> edges;
      for (xmlNodePtr cur_kid = cur_node->children; cur_kid; cur_kid = cur_kid->next)
        if (cur_kid->type == XML_ELEMENT_NODE && cur_kid->children->type == XML_TEXT_NODE) {
          const std::string e( reinterpret_cast<const char*>(cur_kid->children->content));
          edges.push_back(vertexCreator(e));
        }

      g.setEdges(v, edges);
    }
}

} // anonym namespace

template <typename V, typename F>
Graph<V> readGraphFromXML(const std::string& filename, F vertexCreator)
{
  xmlDocPtr doc = xmlReadFile(filename.c_str(), NULL, 0);
  if (doc == NULL)
    throw std::runtime_error("Failed to parse " + filename);

  xmlNodePtr root_element = NULL;
  root_element = xmlDocGetRootElement(doc);

  Graph<V> g;
  readVertices(g, vertexCreator, root_element);

  xmlFreeDoc(doc);
  xmlCleanupParser();
  return g;
}

template <typename V, typename F>
void writeGraphToXML(const Graph<V>& g, const std::string& filename, F vertexSerializer)
{
  std::ofstream file;
  file.open (filename);
  if (!file.is_open())
    throw std::runtime_error("Failed to open " + filename + " to write.");

  file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
  file << "<graph>" << std::endl;

  for (const auto cit : g) {
    const std::string v = vertexSerializer(cit);
    file << "<vertex pos=\"" << v << "\">" << std::endl;
    for (const auto cit2 : g.neighboursOf(cit)) {
      const std::string n = vertexSerializer(cit2);
      file << "  <edge>" << v << "</edge>" << std::endl;
    }
    file << "</vertex>" << std::endl;
  }

  file << "</graph>" << std::endl;
  file.close();
}
