#include <graph.hpp>

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>

#include <algorithm>


namespace {

template <typename G>
inline typename G::weight_type min_dist_between(const G& graph,
                                         typename G::const_reference destination,
                                         typename G::const_reference source)
{
  const std::vector<typename G::weight_type>& ws = graph.weights(destination, source);
  const typename std::vector<typename G::weight_type>::const_iterator it = std::min_element(ws.begin(), ws.end());
  return *it;
}

template <typename V, typename W>
inline V smallest_distance_to_graph(const std::unordered_set<V>& q, const std::unordered_map<V, W>& dist)
{
  const typename std::unordered_set<V>::const_iterator smallest_it =
    std::min_element(q.begin(), q.end(),
      [&dist](const V& v1, const V& v2)
      { return !(dist.find(v2) != dist.end() && ( (dist.find(v1) == dist.end()) || (dist.at(v1) > dist.at(v2)))); } );

  return *smallest_it;
}

template <typename V, typename W>
std::vector<V> pathFromPrevList(const V& dest, std::unordered_map<V, V> prev)
{
  std::vector<V> retval;

  retval.push_back(dest);
  for (V it = dest; prev.find(it) != prev.end() ; it = prev.at(it))
    retval.push_back(prev[it]);

  std::reverse(retval.begin(), retval.end());
  return retval;
}

} // anonym namespace



// template <typename G>
// std::pair <std::unordered_map<typename G::value_type, typename G::weight_type>,
//            std::unordered_map<typename G::value_type, typename G::value_type> >
// dijkstra_shortest_path(const G& graph, typename G::const_reference source)
// {
//   typedef typename G::value_type V;
//   typedef typename G::weight_type W;
//
//   std::unordered_map<V, W> dist; /// @todo -> std::map < W, V > ?
//   std::unordered_map<V, V> prev;
//
//   dist[source] = V();
//
//   std::unordered_set<V> q;
//   for (const V& v : graph.vertices())
//     q.insert(v);
//
//   while (!q.empty()) {
//     const V& u = smallest_distance_to_graph<V, W>(q, dist);
//     q.erase(u);
//     if (dist.find(u) == dist.end())
//       continue;
//
//     for (V v : graph.neighboursOf(u)) {
//       const W alt = dist.at(u) + min_dist_between(graph, u, v);
//       if (dist.find(v) == dist.end() || alt < dist.at(v)) {
//         dist[v] = alt;
//         prev[v] = u;
//       }
//     }
//   }
//
//   return std::make_pair(dist, prev);
// }
//
// template <typename G>
// std::pair <std::unordered_map<typename G::value_type, typename G::weight_type>,
//            std::unordered_map<typename G::value_type, typename G::value_type> >
// dijkstra_shortest_path_v2(const G& graph, typename G::const_reference source)
// {
//   typedef typename G::value_type V;
//   typedef typename G::weight_type W;
//
//   std::unordered_map<V, W> dist;
//   std::unordered_map<V, V> prev;
//
//   dist[source] = V();
//
//   std::unordered_set<V> q;
//   q.insert(source);
//
//   const std::vector<V>& s_n = graph.neighboursOf(source);
//   std::copy(s_n.begin(), s_n.end(), std::inserter(q, q.end()));
//
//   while (!q.empty()) {
//     const V& u = smallest_distance_to_graph<V, W>(q, dist);
//     q.erase(u);
//
//     for (V v : graph.neighboursOf(u)) {
//       const W alt = dist.at(u) + min_dist_between(graph, u, v);
//       if (dist.find(v) == dist.end() || alt < dist.at(v)) {
//         dist[v] = alt;
//         prev[v] = u;
//       }
//
//       if (dist.find(v) == dist.end())
//         q.insert(v);
//     }
//   }
//
//   return std::make_pair(dist, prev);
// }

template <typename G>
std::vector<typename G::value_type>
dijkstra_shortest_path_to(const G& graph,
                          typename G::const_reference source,
                          typename G::const_reference dest)
{
  typedef typename G::value_type V;
  typedef typename G::weight_type W;

  std::unordered_map<V, W> dist; /// @todo into std::priority_queue<std::pair<V< W>>
  std::unordered_map<V, V> prev;

  dist[source] = V();

  std::unordered_set<V> q;
  q.insert(source);

  const std::vector<V>& s_n = graph.neighboursOf(source);
  std::copy(s_n.begin(), s_n.end(), std::inserter(q, q.end()));

  while (!q.empty()) {
    const V& u = smallest_distance_to_graph<V, W>(q, dist);
    q.erase(u);
    if (u == dest)
      break;

    for (V v : graph.neighboursOf(u)) {
      const W alt = dist.at(u) + min_dist_between(graph, u, v);
      if (dist.find(v) == dist.end() || alt < dist.at(v)) {
        dist[v] = alt;
        prev[v] = u;
      }

      if (dist.find(v) == dist.end())
        q.insert(v);
    }
  }

  return pathFromPrevList(dest, prev);
}
