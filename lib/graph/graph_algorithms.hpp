#ifndef GRAPH_ALGORITHMS_HPP
#define GRAPH_ALGORITHMS_HPP

#include "graph.hpp"

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>

#include <utility>
#include <algorithm>
#include <functional>


namespace {

template <typename V, typename W>
inline V closestNode(const std::unordered_set<V>& q, const std::unordered_map<V, W>& dist)
{
  const typename std::unordered_set<V>::const_iterator smallest_it =
    std::min_element(q.begin(), q.end(),
      [&dist](const V& v1, const V& v2)
      { return !(  dist.find(v2) != dist.end() &&
                   ((dist.find(v1) == dist.end()) || (dist.at(v1) > dist.at(v2)))  ); } );

  return *smallest_it;
}

template <typename V>
std::vector<V> pathFromPrevList(const V& dest, std::unordered_map<V, V> prev)
{
  std::vector<V> retval;
  if (prev.find(dest) == prev.end())
    return retval;

  V it = dest;
  for (; prev.find(it) != prev.end() ; it = prev.at(it)) {
    retval.push_back(it);
  }
  retval.push_back(it);

  std::reverse(retval.begin(), retval.end());
  return retval;
}

} // anonym namespace


template <typename V, typename W>
std::vector<V>
dijkstra_shortest_path_to(const Graph<V>& graph,
                          const V& source,
                          const V& dest,
                          std::function<W(V, V)> distanceCompute)
{
  std::unordered_map<V, W> dist;
  std::unordered_map<V, V> prev;

  dist.emplace(source, W());
  std::unordered_set<V> q;
  for (const auto& v : graph.neighboursOf(source)) {
    q.insert(v);
    dist[v] = distanceCompute(source, v);
    prev[v] = source;
  }

  while (!q.empty()) {
    const V& u = closestNode<V, W>(q, dist);
    q.erase(u);
    if (u == dest)
      break;

    for (V v : graph.neighboursOf(u)) {
      const W d = distanceCompute(u, v);
      const W alt = dist.at(u) + d;

      if (dist.find(v) == dist.end()) { // new node
        dist.emplace(v, alt);
        prev.emplace(v, u);
        q.insert(v);
      } else if (alt < dist.at(v)) { // better route
        dist[v] = alt;
        prev[v] = u;
      }
    }
  }

  return pathFromPrevList(dest, prev);
}

#endif // GRAPH_ALGORITHMS_HPP
