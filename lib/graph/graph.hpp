#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <unordered_map>
#include <vector>

#include <algorithm>
#include <utility>

/**
  the graph is:
  - not weighed
  - not directed. There are 2 edges added for each connection, both direction
  - no multi/self edges

  - Stored as an \ref std::unordered_map map where the keys are vertices and values are \ref std::vector of edges.
    The multimap is picked since \ref neighboursOf is the most critical operation.
    @note maybe \ref std::set is a better option for the edges?

  - V expected to be cheap to copy
  - V should have operator== and be hashable (for the internal std::unordered_map):
  ~~~{.cpp}
    namespace std {
      template <>
      struct hash<A> {
        std::size_t operator()(const A& a) const { return ...; }
      };
    }
  ~~~
*/
template <typename V>
class Graph {

public:

  typedef size_t size_type;
  typedef V value_type;
  typedef V* pointer;
  typedef const V* const_pointer;
  typedef V& reference;
  typedef const V& const_reference;
  typedef std::ptrdiff_t difference_type;

private:

  typedef std::vector<V> edge_container;
  typedef std::unordered_map<V, edge_container> v_container;
  typedef typename v_container::iterator v_iterator;
  typedef typename v_container::const_iterator v_const_iterator;

public:

  struct Edge {
    Edge(const_reference s, const_reference d) : source(s), destination(d) {}
    bool operator==(const Edge& o) const { return source == o.source && destination == o.destination; }

    value_type source;
    value_type destination;
  };

  Graph() : m_vertices() {}
  Graph(std::initializer_list<V> vertex_list);
  Graph(const std::vector<V>& vertex_list);
  Graph(std::initializer_list<Edge> edge_list);
  Graph(const std::vector<Edge>& edge_list);
  bool operator==(const Graph& o) const;
  bool operator!=(const Graph& o) const { return !(*this == o); }

  void addVertex(const_reference data);
  void removeVertex(const_reference data);
  void modifyVertex(const_reference old_data, const_reference new_data);

  void addEdge(const_reference source, const_reference destination);
  void setEdges(const_reference source, const std::vector<value_type>& destinations);
  void removeEdge(const_reference source, const_reference destination);

  std::vector<value_type> vertices() const;

  void clear() noexcept { m_vertices.clear(); }
  const std::vector<value_type>& neighboursOf(const_reference data) const;


  class vertex_iterator : public std::iterator<std::forward_iterator_tag,
                                               value_type,
                                               difference_type,
                                               pointer,
                                               reference>
  {
  friend class Graph;

  public:
    typedef vertex_iterator self_type;
    typedef vertex_iterator& reference_self_type;
    typedef const vertex_iterator& const_reference_self_type;

    const_reference operator*() { return m_it->first; }
    const_pointer operator->() { return &m_it->first; }
    self_type &operator++() { ++m_it; return *this; }
    self_type operator++(int) { self_type tmp(*this); ++(*this); return tmp; }
    self_type operator+(difference_type n) { self_type tmp(*this); tmp.pos_ += n; return tmp; }
    self_type &operator+=(difference_type n) {  m_it += n; return *this; }
    bool operator==(const_reference_self_type o) const { return m_it == o.m_it; }
    bool operator!=(const_reference_self_type o) const { return !(*this == o); }

  private:
    vertex_iterator(v_iterator it) : m_it(it) {}
    vertex_iterator(v_const_iterator it) : m_it(it) {}

    v_const_iterator m_it;
  };

  typedef vertex_iterator iterator;
  typedef const vertex_iterator const_iterator;

  iterator begin() noexcept { return iterator(m_vertices.begin()); }
  iterator begin() const noexcept { return iterator(m_vertices.begin()); }
  const_iterator cbegin() const noexcept { return const_iterator(m_vertices.begin()); }
  iterator end() noexcept { return iterator(m_vertices.end()); }
  iterator end() const noexcept { return iterator(m_vertices.end()); }
  const_iterator cend() const noexcept { return const_iterator(m_vertices.end()); }

private:

  std::vector<value_type>& nonConstNeighboursOf(const_reference data);
  static void eraseEdge(edge_container& v, const_reference data);
  v_iterator addVertexAndReturnIterator(const_reference data);

  v_container m_vertices;
};

// Free functions

template <typename V>
inline bool empty(const Graph<V>& g) noexcept { return g.vertices().empty(); }

template <typename V>
inline typename Graph<V>::size_type numberOfVertices(const Graph<V>& g) noexcept { return  g.vertices().size(); }

template <typename V>
inline typename Graph<V>::size_type numberOfEdges(const Graph<V>& g) { return  edges(g).size(); }

template <typename V>
inline bool contains(const Graph<V>& g, typename Graph<V>::const_reference data) {
  const auto v = g.vertices();
  return std::find(v.begin(), v.end(), data) != v.end();
}

template <typename V>
inline bool connected(const Graph<V>& g, typename Graph<V>::const_reference source, typename Graph<V>::const_reference destination) {
  const auto n = g.neighboursOf(source);
  return std::find(n.begin(), n.end(), destination) != n.end();
}

template <typename V>
inline std::vector<typename Graph<V>::Edge> edges(const Graph<V>& g)
{
  std::vector<typename Graph<V>::Edge> retval;
  for (const auto& v : g.vertices())
    for (const auto& e : g.neighboursOf(v))
      retval.emplace_back(typename Graph<V>::Edge(v, e));

  return retval;
}

template <typename V>
inline Graph<V> disjointUnion(const Graph<V>& a, const Graph<V>& b) {
  Graph<V> g(a);
  for (const auto& e : edges(b))
    g.addEdge(e.source, e.destination);

  return g;
}


// Graph implementation

template <typename V>
inline Graph<V>::Graph(std::initializer_list<V> vertex_list)
  : Graph<V>()
{
  for(const auto& v : vertex_list)
    addVertex(v);
}

template <typename V>
inline Graph<V>::Graph(const std::vector<V>& vertex_list)
  : Graph<V>()
{
  for(const auto& v : vertex_list)
    addVertex(v);
}

template <typename V>
inline Graph<V>::Graph(std::initializer_list<Edge> edge_list)
  : Graph<V>()
{
  for (const auto& e : edge_list )
    addEdge(e.source, e.destination);
}

template <typename V>
inline Graph<V>::Graph(const std::vector<Edge>& edge_list)
  : Graph<V>()
{
  for (const auto& e : edge_list )
    addEdge(e.source, e.destination);
}

template <typename V>
inline bool Graph<V>::operator==(const Graph& o) const
{
  if (numberOfVertices(*this) != numberOfVertices(o))
    return false;

  for (const auto& v : m_vertices) {
    const auto& o_v(o.neighboursOf(v.first));
    if (v.second.size() != o_v.size())
      return false;

    // compare 2 unordered vectors of unique elements...very costy
    /// @note should vector be replaced with set?
    auto v_copy(v.second);
    auto o_v_copy(o_v);
    sort(v_copy.begin(), v_copy.end());
    sort(o_v_copy.begin(), o_v_copy.end());
    if (v_copy != o_v_copy)
      return false;
  }
  return true;
}


template <typename V>
inline void Graph<V>::addVertex(const_reference data)
{
  addVertexAndReturnIterator(data);
}

template <typename V>
inline void Graph<V>::removeVertex(const_reference data)
{
  const auto number_of_removed_elements = m_vertices.erase(data);
  if (number_of_removed_elements > 0)
    for (auto &v : m_vertices)
      eraseEdge(v.second, data);
}

template <typename V>
inline void Graph<V>::modifyVertex(const_reference old_data, const_reference new_data)
{
  if (old_data == new_data)
    return;

  auto neighbours = neighboursOf(old_data);
  for (auto &v : neighbours) {
    auto& n = nonConstNeighboursOf(v);
    auto n_it = std::find(n.begin(), n.end(), old_data);
    *n_it = new_data;
  }

  const auto number_of_removed_elements = m_vertices.erase(old_data);
  if (number_of_removed_elements > 0)
    m_vertices.emplace(new_data, neighbours);
}

template <typename V>
inline void Graph<V>::addEdge(const_reference source, const_reference destination)
{
  if (source == destination) // no self-edges
    return;

  auto n = neighboursOf(source); // no multiedges
  if (std::find(n.begin(), n.end(), destination) != n.end())
    return;

  auto source_it = addVertexAndReturnIterator(source);
  source_it->second.push_back(destination);
  auto destination_it = addVertexAndReturnIterator(destination);
  destination_it->second.push_back(source);
}

template <typename V>
inline void Graph<V>::setEdges(const_reference source, const std::vector<value_type>& destinations)
{
  auto source_it = addVertexAndReturnIterator(source);
  source_it->second.clear();
  source_it->second = destinations;
}

template <typename V>
inline void Graph<V>::removeEdge(const_reference source, const_reference destination)
{
  auto source_it = m_vertices.find(source);
  if (source_it == m_vertices.end())
    return;

  auto destination_it = m_vertices.find(destination);
  if (destination_it == m_vertices.end())
    return;

  eraseEdge(source_it->second, destination);
  eraseEdge(destination_it->second, source);
}

template <typename V>
inline std::vector<typename Graph<V>::value_type> Graph<V>::vertices() const
{
  std::vector<value_type> retval;
  for (const auto& v : m_vertices)
    retval.push_back(v.first);

  return retval;
}

template <typename V>
inline const std::vector<V>& Graph<V>::neighboursOf(const_reference data) const
{
  static std::vector<V> empty;
  auto vertex_it = m_vertices.find(data);
  if (vertex_it == m_vertices.end())
    return empty;
  else
    return vertex_it->second;
}

template <typename V>
inline std::vector<V>& Graph<V>::nonConstNeighboursOf(const_reference data)
{
  auto vertex_it = m_vertices.find(data);
  return vertex_it->second;
}

template <typename V>
inline void Graph<V>::eraseEdge(edge_container& v, const_reference data) {
  v.erase(std::remove_if(v.begin(), v.end(),
                         [&data](const_reference d) { return d == data; }),
          v.end());
}

template <typename V>
inline typename Graph<V>::v_iterator Graph<V>::addVertexAndReturnIterator(const_reference data)
{
  return m_vertices.emplace(data, edge_container()).first;
}
#endif // GRAPH_HPP
