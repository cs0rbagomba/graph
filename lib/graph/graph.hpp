#ifndef GRAPHWD_HPP
#define GRAPHWD_HPP

#include <unordered_map>
#include <vector>

#include <algorithm>
#include <utility>

// not weighed, not directed
// multiedges, self edges are not checked
// V expected to be cheap to store aggregate

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
    Edge() : source(), destination() {}
    Edge(const_reference s, const_reference d) : source(s), destination(d) {}
    Edge(const Edge& o) : source(o.source), destination(o.destination) {}
    Edge& operator=(Edge o) { swap(o); return *this; }
    void swap(Edge& o) { std::swap(source, o.source); std::swap(destination, o.destination); }
    bool operator==(const Edge& o) const { return source == o.source && destination == o.destination; }

    value_type source;
    value_type destination;
  };

  Graph() : m_vertices() {}
  Graph(const Graph<V>& o) : m_vertices(o.m_vertices) {}
  Graph(std::initializer_list<V> vertex_list);
  Graph(std::initializer_list<Edge> edge_list);

  Graph<V>& operator=(Graph<V> o) { swap(o); return *this; }
  void swap(Graph& o) { std::swap(m_vertices, o.m_vertices); }

  //  Capacity
  bool empty() const  { return m_vertices.empty(); }
  size_type numberOfVertices() const { return m_vertices.size(); }
  size_type numberOfEdges() const;

  // Modifiers
  void clear() { m_vertices.clear(); }

  void addVertex(const_reference data);
  void removeVertex(const_reference data);
  void modifyVertex(const_reference old_data, const_reference new_data);

  void addEdge(const_reference source, const_reference destination);
  void setEdges(const_reference source, const std::vector<value_type>& destinations);
  void removeEdge(const_reference source, const_reference destination);

  // Lookup
  bool contains(const_reference data) const { return m_vertices.find(data) != m_vertices.end(); }
  std::vector<value_type> vertices() const;
  std::vector<value_type> neighboursOf(const_reference data) const;
  std::vector<Edge> edges() const;

  // iterator
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

    vertex_iterator() : m_it() {}
    ~vertex_iterator() {}
    vertex_iterator(const_reference_self_type o) : m_it(o.m_it) {}
    reference_self_type operator=(self_type o) { swap(o); return *this; }
    void swap(reference_self_type o) { std::swap(m_it, o.m_it); }

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

  iterator begin() { return iterator(m_vertices.begin()); }
  iterator begin() const { return iterator(m_vertices.begin()); }
  const_iterator cbegin() const { return const_iterator(m_vertices.begin()); }
  iterator end() { return iterator(m_vertices.end()); }
  iterator end() const { return iterator(m_vertices.end()); }
  const_iterator cend() const { return const_iterator(m_vertices.end()); }

private:

  std::vector<value_type>& nonConstNeighboursOf(const_reference data);
  static void eraseEdge(edge_container& v, const_reference data);

  v_container m_vertices;
};


// Graph

template <typename V>
inline Graph<V>::Graph(std::initializer_list<V> vertex_list)
  : Graph<V>()
{
  for(const V& v : vertex_list)
    addVertex(v);
}

template <typename V>
inline Graph<V>::Graph(std::initializer_list<Edge> edge_list)
  : Graph<V>()
{
  for (const Edge& e : edge_list )
    addEdge(e.source, e.destination, e.weight);
}

template <typename V>
inline typename Graph<V>::size_type Graph<V>::numberOfEdges() const
{
  int sum = 0;
  for (const auto& v : m_vertices)
    sum += v.second.size();

  return sum;
}

template <typename V>
inline void Graph<V>::addVertex(const_reference data)
{
  if (m_vertices.find(data) != m_vertices.end())
    return;

  std::pair<V, edge_container> p(data, edge_container());
  m_vertices.insert(p);
}

template <typename V>
inline void Graph<V>::removeVertex(const_reference data)
{
  v_iterator it = m_vertices.find(data);
  if (it == m_vertices.end())
    return;

  for (auto &v : m_vertices)
    eraseEdge(v.second, data);

  m_vertices.erase(it);
}

template <typename V>
inline void Graph<V>::modifyVertex(const_reference old_data, const_reference new_data)
{
  if (old_data == new_data)
    return;

  v_iterator it = m_vertices.find(old_data);
  if (it == m_vertices.end())
    return;

  std::vector<value_type> neighbours = neighboursOf(old_data);
  for (auto &v : neighbours) {
    std::vector<value_type>& n_v = nonConstNeighboursOf(v);
    typename std::vector<value_type>::iterator n_it = std::find(n_v.begin(), n_v.end(), old_data);
    *n_it = new_data;
  }

  m_vertices.erase(it);
  std::pair<V, edge_container> p(new_data, neighbours);
  m_vertices.insert(p);
}

template <typename V>
inline void Graph<V>::addEdge(const_reference source, const_reference destination)
{
  addVertex(source);
  addVertex(destination);

  v_iterator source_it = m_vertices.find(source);
  v_iterator destination_it = m_vertices.find(destination);

  source_it->second.push_back(destination);
  destination_it->second.push_back(source);
}

template <typename V>
inline void Graph<V>::setEdges(const_reference source, const std::vector<value_type>& destinations)
{
  addVertex(source);
  v_iterator source_it = m_vertices.find(source);

  source_it->second.clear();
  source_it->second = destinations;
}

template <typename V>
inline void Graph<V>::removeEdge(const_reference source, const_reference destination)
{
  v_iterator source_it = m_vertices.find(source);
  if (source_it == m_vertices.end())
    return;

  v_iterator destination_it = m_vertices.find(destination);
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
inline std::vector<V> Graph<V>::neighboursOf(const_reference data) const
{
  v_const_iterator vertex_it = m_vertices.find(data);
  if (vertex_it == m_vertices.end())
    return std::vector<V>();
  else
    return vertex_it->second;
}

template <typename V>
inline std::vector<V>& Graph<V>::nonConstNeighboursOf(const_reference data)
{
  v_iterator vertex_it = m_vertices.find(data);
  return vertex_it->second;
}

template <typename V>
inline std::vector<typename Graph<V>::Edge> Graph<V>::edges() const
{
  std::vector<typename Graph<V>::Edge> retval;
  for (const auto& v : m_vertices)
    for (const auto& e : v.second)
      retval.push_back(Graph<V>::Edge(v.first, e));

  return retval;
}

template <typename V>
inline void Graph<V>::eraseEdge(edge_container& v, const_reference data) {
  v.erase(std::remove(v.begin(), v.end()), v.end());
}


#endif // GRAPHWD_HPP
