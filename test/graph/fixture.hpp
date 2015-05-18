#ifndef GRAPH_TEST_FIXTURE_HPP
#define GRAPH_TEST_FIXTURE_HPP

#include <cmath>

struct float2 {
  float x, y;

  inline float2() : x(0.0), y(0.0) {}
  inline float2(float f1, float f2) : x(f1), y(f2) {}
};

inline bool operator ==(const float2& v1, const float2& v2) { return v1.x == v2.x && v1.y == v2.y; }
inline float pow2(float f) { return f*f; }
inline float dist(const float2& v1, const float2& v2) { return sqrt(pow2(v2.x - v1.x) + pow2(v2.y - v1.y)); }
// inline float dist(const float2& v1, const float2& v2) { return abs(v2.x - v1.x) + abs(v2.y - v1.y); }

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

  class distanceOf2float2s : public std::function<float(float2, float2)>
  {
  public:
    float operator()(float2 a, float2 b) { return dist(a, b); }
  };
}

constexpr std::size_t numberOfEdges(std::size_t number_of_rows, std::size_t number_of_columns) {
  return (number_of_rows-2)*(number_of_columns-2) *8                // inside vertices have 8
          + 4 *3                                                    // corners have 3
          + ((number_of_rows-2)*2 + (number_of_columns-2)*2) *5;    // sides has 5
}

template <typename V>
std::vector<V>* createVertices(std::size_t number_of_rows, std::size_t number_of_columns) {
  std::vector<V>* retval = new std::vector<V>();
  const std::size_t number_of_edges = number_of_rows * number_of_columns;
  retval->reserve(number_of_edges);

  for (std::size_t c = 0; c < number_of_columns; ++c)
    for (std::size_t r = 0; r < number_of_rows; ++r)
      retval->push_back(V(r, c));

  return retval;
}

template <typename V>
std::vector<typename Graph<V>::Edge>* createEdges(std::size_t number_of_rows, std::size_t number_of_columns) {
  std::vector<typename Graph<V>::Edge>* retval = new std::vector<typename Graph<V>::Edge>();
  const std::size_t number_of_edges = numberOfEdges(number_of_rows, number_of_columns);
  retval->reserve(number_of_edges);

  // inside
  for (std::size_t c = 1; c < number_of_columns-1; ++c)
    for (std::size_t r = 1; r < number_of_rows-1; ++r) {
      retval->push_back(typename Graph<V>::Edge(float2(r, c), float2(r-1, c-1)));
      retval->push_back(typename Graph<V>::Edge(float2(r, c), float2(r-1, c)));
      retval->push_back(typename Graph<V>::Edge(float2(r, c), float2(r-1, c+1)));
      retval->push_back(typename Graph<V>::Edge(float2(r, c), float2(r,   c-1)));
      retval->push_back(typename Graph<V>::Edge(float2(r, c), float2(r,   c+1)));
      retval->push_back(typename Graph<V>::Edge(float2(r, c), float2(r+1, c-1)));
      retval->push_back(typename Graph<V>::Edge(float2(r, c), float2(r+1, c)));
      retval->push_back(typename Graph<V>::Edge(float2(r, c), float2(r+1, c+1)));
    }

    // top & bottom row
    for (std::size_t r = 1; r < number_of_rows-1; ++r) {
      retval->push_back(typename Graph<V>::Edge(float2(r, 0), float2(r-1, 0)));
      retval->push_back(typename Graph<V>::Edge(float2(r, 0), float2(r+1, 0)));
      retval->push_back(typename Graph<V>::Edge(float2(r, 0), float2(r-1, 1)));
      retval->push_back(typename Graph<V>::Edge(float2(r, 0), float2(r,   1)));
      retval->push_back(typename Graph<V>::Edge(float2(r, 0), float2(r+1, 1)));

      retval->push_back(typename Graph<V>::Edge(float2(r, number_of_columns-1), float2(r-1, number_of_columns-1)));
      retval->push_back(typename Graph<V>::Edge(float2(r, number_of_columns-1), float2(r+1, number_of_columns-1)));
      retval->push_back(typename Graph<V>::Edge(float2(r, number_of_columns-1), float2(r-1, number_of_columns-1-1)));
      retval->push_back(typename Graph<V>::Edge(float2(r, number_of_columns-1), float2(r,   number_of_columns-1-1)));
      retval->push_back(typename Graph<V>::Edge(float2(r, number_of_columns-1), float2(r+1, number_of_columns-1-1)));
    }

    // left & right column
    for (std::size_t c = 1; c < number_of_columns-1; ++c) {
      retval->push_back(typename Graph<V>::Edge(float2(0, c), float2(0, c-1)));
      retval->push_back(typename Graph<V>::Edge(float2(0, c), float2(0, c+1)));
      retval->push_back(typename Graph<V>::Edge(float2(0, c), float2(1, c-1)));
      retval->push_back(typename Graph<V>::Edge(float2(0, c), float2(1, c)));
      retval->push_back(typename Graph<V>::Edge(float2(0, c), float2(1, c+1)));

      retval->push_back(typename Graph<V>::Edge(float2(number_of_rows-1, c), float2(number_of_rows-1,   c-1)));
      retval->push_back(typename Graph<V>::Edge(float2(number_of_rows-1, c), float2(number_of_rows-1,   c+1)));
      retval->push_back(typename Graph<V>::Edge(float2(number_of_rows-1, c), float2(number_of_rows-1-1, c-1)));
      retval->push_back(typename Graph<V>::Edge(float2(number_of_rows-1, c), float2(number_of_rows-1-1, c)));
      retval->push_back(typename Graph<V>::Edge(float2(number_of_rows-1, c), float2(number_of_rows-1-1, c+1)));
    }

  // corners
  retval->push_back(typename Graph<V>::Edge(float2(0, 0), float2(0, 1)));
  retval->push_back(typename Graph<V>::Edge(float2(0, 0), float2(1, 0)));
  retval->push_back(typename Graph<V>::Edge(float2(0, 0), float2(1, 1)));

  retval->push_back(typename Graph<V>::Edge(float2(number_of_rows-1, 0), float2(number_of_rows-2, 0)));
  retval->push_back(typename Graph<V>::Edge(float2(number_of_rows-1, 0), float2(number_of_rows-2, 1)));
  retval->push_back(typename Graph<V>::Edge(float2(number_of_rows-1, 0), float2(number_of_rows-1,   1)));

  retval->push_back(typename Graph<V>::Edge(float2(0, number_of_columns-1), float2(0, number_of_columns-2)));
  retval->push_back(typename Graph<V>::Edge(float2(0, number_of_columns-1), float2(1, number_of_columns-2)));
  retval->push_back(typename Graph<V>::Edge(float2(0, number_of_columns-1), float2(1, number_of_columns-1)));

  retval->push_back(typename Graph<V>::Edge(float2(number_of_rows-1, number_of_columns-1), float2(number_of_rows-1, number_of_columns-2)));
  retval->push_back(typename Graph<V>::Edge(float2(number_of_rows-1, number_of_columns-1), float2(number_of_rows-2, number_of_columns-1)));
  retval->push_back(typename Graph<V>::Edge(float2(number_of_rows-1, number_of_columns-1), float2(number_of_rows-2, number_of_columns-2)));

  return retval;
}

/** Creating a big (number_of_rows rows and number_of_columns columns) grid.
 *  Every vertex is connexted to it's 8 neighbours.
 *
 *  +-+-+-+
 *  |x+x+x|
 *  +x+x+x+
 *  |x+x+x|
 *  +-+-+-+
 */
template <typename V>
class Fixture {
public:

  static void initOnce(std::size_t number_of_rows, std::size_t number_of_columns) {
    if (m_initialized)
      return;

    m_vertices = createVertices<float2>(number_of_rows, number_of_columns);
    m_edges = createEdges<float2>(number_of_rows, number_of_columns);
    m_initialized = true;
  }

  static void tearDown() {
    delete m_vertices;
    m_vertices = 0;
    delete m_edges;
    m_edges = 0;
    m_initialized = false;
  }

  static const std::vector<V> getVertices() { return *m_vertices; }
  static const std::vector<typename Graph<V>::Edge> getEdges() { return *m_edges; }

private:
  static bool m_initialized;
  static std::vector<V>* m_vertices;
  static std::vector<typename Graph<V>::Edge>* m_edges;
};

template<class V> bool Fixture<V>::m_initialized = false;
template<class V> std::vector<V>* Fixture<V>::m_vertices = 0;
template<class V> std::vector<typename Graph<V>::Edge>* Fixture<V>::m_edges = 0;

#endif // GRAPH_TEST_FIXTURE_HPP
