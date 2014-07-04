#ifndef GRAPHWD_HPP
#define GRAPHWD_HPP

#include <unordered_map>
#include <vector>
#include <set>

#include <algorithm>

// weighed, directed

template <typename V,
          typename E = int>
class GraphWD {

public:

  typedef size_t size_type;
  typedef V value_type;
  typedef V* pointer;
  typedef const V* const_pointer;
  typedef V& reference;
  typedef const V& const_reference;
  typedef std::ptrdiff_t difference_type;

  typedef E weight_type;
  typedef const E& const_weight_reference;

private:

  struct EdgeTo;

  // @todo switch to std::unordered_map<V, std::pair<std::vector<V>, std::vector<E>>> for quicker neighbours & weights
  // also turning the graph into unordered, weighted, with no multi & self edges by default
  typedef std::unordered_map<V, std::vector<EdgeTo> > v_container;
  typedef typename v_container::iterator v_iterator;
  typedef typename v_container::const_iterator v_const_iterator;

  struct EdgeTo {

    EdgeTo() = delete;
    EdgeTo(v_iterator destination, const_weight_reference weight = weight_type());
    EdgeTo(const EdgeTo& o) : m_destination(o.m_destination), m_weight(o.m_weight) {}
    EdgeTo& operator=(EdgeTo o) { swap(o); return *this; }
    void swap(EdgeTo& o);
    bool operator==(const EdgeTo& o) const;

    v_iterator m_destination;
    weight_type m_weight;
  };

public:

  struct Edge {

  public:

    Edge() : source(), destination(), weight() {}
    Edge(const_reference source, const_reference destination, const_weight_reference weight);
    Edge(const Edge& o);
    ~Edge() {}
    Edge& operator=(Edge o) { swap(o); return *this; }
    void swap(Edge& o);
    bool operator==(const Edge& o) const { return source == o.source && destination == o.destination && weight == o.weight; }

    value_type source;
    value_type destination;
    weight_type weight;
  };

  typedef Edge* edge_pointer;
  typedef Edge& edge_reference;


  GraphWD(bool isdirected = true) : m_directed(isdirected), m_vertices() {}
  GraphWD(const GraphWD<V, E>& o) : m_directed(o.m_directed), m_vertices(o.m_vertices) {}
  GraphWD(std::initializer_list<V> vertex_list);
  GraphWD(std::initializer_list<Edge> edge_list);

  GraphWD<V, E>& operator=(GraphWD<V, E> o) { swap(o); return *this; }
  void swap(GraphWD& o) { std::swap (m_directed, o.m_directed); std::swap(m_vertices, o.m_vertices); }

  // Properties
  bool directed() const { return m_directed; }

  //  Capacity
  bool empty() const  { return m_vertices.empty(); }
  size_type numberOfVertices() const { return m_vertices.size(); }
  size_type numberOfEdges() const;


  /// @todo rename Vertex & Edge?
  // Modifiers
  void clear() { m_vertices.clear(); }

  void addVertex(const_reference data);
  void removeVertex(const_reference data);
  void addEdge(const_reference source, const_reference destination, const_weight_reference weight = weight_type());
  void removeEdge(const_reference source, const_reference destination, const_weight_reference weight = weight_type());
  void removeEdges(const_reference source, const_reference destination);

  // Lookup
  bool contains(const_reference data) const { return m_vertices.find(data) != m_vertices.end(); }
  std::vector<value_type> vertices() const;
  std::vector<value_type> neighboursOf(const_reference data) const;
  std::vector<weight_type> weights(const_reference source, const_reference destination) const;
  std::vector<Edge> edges() const;

  // iterators

  class vertex_iterator : public std::iterator<std::forward_iterator_tag,
                                               value_type,
                                               difference_type,
                                               pointer,
                                               reference>
  {
  friend class GraphWD;

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
  const_iterator cbegin() const { return const_iterator(m_vertices.begin()); }
  iterator end() { return iterator(m_vertices.end()); }
  const_iterator cend() const { return const_iterator(m_vertices.end()); }

private:

  static void eraseEdge(typename std::vector<EdgeTo>& v, const_reference data);
  static void eraseEdge(typename std::vector<EdgeTo>& v, const_reference data, const_weight_reference weight);

  bool m_directed;
  v_container m_vertices;
};


// Edge

template <typename V, typename E>
inline GraphWD<V, E>::Edge::Edge(const_reference s, const_reference d, const_weight_reference w)
  : source(s)
  , destination(d)
  , weight(w)
{}

template <typename V, typename E>
inline GraphWD<V, E>::Edge::Edge(const Edge& o)
  : source(o.source)
  , destination(o.destination)
  , weight(o.weight)
{}

template <typename V, typename E>
inline void GraphWD<V, E>::Edge::swap(Edge& o)
{
  std::swap(source, o.source);
  std::swap(destination, o.destination);
  std::swap(weight, o.weight);
}


// EdgeTo
template <typename V, typename E>
inline GraphWD<V, E>::EdgeTo::EdgeTo(v_iterator destination, const_weight_reference weight)
  : m_destination(destination)
  , m_weight(weight)
{}

template <typename V, typename E>
inline void GraphWD<V, E>::EdgeTo::swap(EdgeTo& o)
{
  std::swap(m_destination, o.m_destination);
  std::swap(m_weight, o.m_weight);
}

template <typename V, typename E>
inline bool GraphWD<V, E>::EdgeTo::operator==(const EdgeTo& other) const
{
  return m_destination == other.m_destination &&
         m_weight == other.m_weight;
}

// GraphWD

template <typename V, typename E>
GraphWD<V, E>::GraphWD(std::initializer_list<V> vertex_list)
  : GraphWD<V, E>()
{
  for(const V& v : vertex_list)
    addVertex(v);
}

template <typename V, typename E>
GraphWD<V, E>::GraphWD(std::initializer_list<Edge> edge_list)
  : GraphWD<V, E>()
{
  for (const Edge& e : edge_list )
    addEdge(e.source, e.destination, e.weight);
}

template <typename V, typename E>
inline typename GraphWD<V, E>::size_type GraphWD<V, E>::numberOfEdges() const
{
  int sum = 0;
  for (const auto& v : m_vertices)
    sum += v.second.size();

  return sum;
}

template <typename V, typename E>
inline void GraphWD<V, E>::addVertex(const_reference data)
{
  if (m_vertices.find(data) != m_vertices.end())
    return;

  std::pair<V, std::vector<GraphWD<V, E>::EdgeTo> > p(data, std::vector<EdgeTo>());
  m_vertices.insert(p);
}

template <typename V, typename E>
inline void GraphWD<V, E>::removeVertex(const_reference data)
{
  v_iterator it = m_vertices.find(data);
  if (it == m_vertices.end())
    return;

  if (m_directed)
    for (auto &v : m_vertices)
        eraseEdge(v.second, data);
  else
    for (EdgeTo& n : it->second)
      eraseEdge(n.m_destination->second, data);

  m_vertices.erase(it);
}

template <typename V, typename E>
void GraphWD<V, E>::addEdge(const_reference source, const_reference destination, const_weight_reference weight)
{
  addVertex(source);
  addVertex(destination);

  v_iterator source_it = m_vertices.find(source);
  v_iterator destination_it = m_vertices.find(destination);

  source_it->second.push_back(GraphWD<V, E>::EdgeTo(destination_it, weight));
  if (!m_directed && source != destination)
    destination_it->second.push_back(GraphWD<V, E>::EdgeTo(source_it, weight));
}

template <typename V, typename E>
inline void GraphWD<V, E>::removeEdge(const_reference source, const_reference destination, const_weight_reference weight)
{
  v_iterator source_it = m_vertices.find(source);
  if (source_it == m_vertices.end())
    return;

  v_iterator destination_it = m_vertices.find(destination);
  if (destination_it == m_vertices.end())
    return;

  eraseEdge(source_it->second, destination, weight);
  if (!m_directed)
    eraseEdge(destination_it->second, source, weight);
}

template <typename V, typename E>
inline void GraphWD<V, E>::removeEdges(const_reference source, const_reference destination)
{
  v_iterator source_it = m_vertices.find(source);
  if (source_it == m_vertices.end())
    return;

  v_iterator destination_it = m_vertices.find(destination);
  if (destination_it == m_vertices.end())
    return;

  eraseEdge(source_it->second, destination);
  if (!m_directed)
    eraseEdge(destination_it->second, source);
}

template <typename V, typename E>
inline std::vector<typename GraphWD<V, E>::value_type> GraphWD<V, E>::vertices() const
{
  std::vector<value_type> retval;
  for (const auto& v : m_vertices)
    retval.push_back(v.first);

  return retval;
}

template <typename V, typename E>
std::vector<typename GraphWD<V, E>::value_type> GraphWD<V, E>::neighboursOf(const_reference data) const
{
  typename std::vector<value_type> retval;
  v_const_iterator vertex_it = m_vertices.find(data);
  if (vertex_it == m_vertices.end() || vertex_it->second.empty())
    return retval;

  std::set<value_type> tmp;
  for (const EdgeTo& e : vertex_it->second)
    if (tmp.insert(e.m_destination->first).second)
      retval.push_back(e.m_destination->first);

  return retval;
}

template <typename V, typename E>
std::vector<E> GraphWD<V, E>::weights(const_reference source, const_reference destination) const
{
  std::vector<E> retval;
  v_const_iterator vertex_it = m_vertices.find(source);
  if (vertex_it == m_vertices.end())
    return retval;

  if (m_vertices.find(destination) == m_vertices.end())
    return retval;

  for (const EdgeTo& e : vertex_it->second)
    if (e.m_destination->first == destination)
      retval.push_back(e.m_weight);

  return retval;
}

template <typename V, typename E>
inline std::vector<typename GraphWD<V, E>::Edge> GraphWD<V, E>::edges() const
{
  std::vector<typename GraphWD<V, E>::Edge> retval;
  for (const auto& v : m_vertices)
    for (const auto& e : v.second)
      retval.push_back(GraphWD<V, E>::Edge(v.first, (e.m_destination)->first, e.m_weight));

  return retval;
}

template <typename V, typename E>
void GraphWD<V, E>::eraseEdge(typename std::vector<EdgeTo>& v, const_reference data) {
  v.erase(std::remove_if(v.begin(), v.end(),
                         [&data](const EdgeTo& e) { return e.m_destination->first == data; } ),
          v.end());
}

template <typename V, typename E>
void GraphWD<V, E>::eraseEdge(typename std::vector<EdgeTo>& v, const_reference data, const_weight_reference weight ) {
    v.erase(std::remove_if(v.begin(), v.end(),
                           [&data, &weight](const EdgeTo& e) { return e.m_destination->first == data && e.m_weight == weight; } ),
            v.end());
}

#endif // GRAPHWD_HPP
