#ifndef GRAPH_ALGORITHMS_HPP
#define GRAPH_ALGORITHMS_HPP

#include "graph.hpp"

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <set>

#include <utility>
#include <algorithm>
#include <functional>
#include <type_traits>


namespace {

template <typename V, typename W>
inline V closestNode(const std::unordered_set<V>& q, const std::unordered_map<V, std::pair<W, V> >& dist_prev)
{
  const typename std::unordered_set<V>::const_iterator smallest_it =
    std::min_element(q.begin(), q.end(),
      [&dist_prev](const V& v1, const V& v2)
      { const auto v1_it = dist_prev.find(v1);
        const auto v2_it = dist_prev.find(v2);
        return !(  v2_it != dist_prev.end() && ((v1_it == dist_prev.end()) || (v1_it->second.first > v2_it->second.first))  ); } );

  return *smallest_it;
}

template <typename V, typename W>
std::vector<V> pathFromPrevList(const V& dest, const std::unordered_map<V, std::pair<W, V> >& dist_prev)
{
  std::vector<V> retval;
  if (dist_prev.find(dest) == dist_prev.end())
    return retval;

  V it = dest;
  for (; it != V() ; it = dist_prev.at(it).second) {
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
  std::unordered_map<V, std::pair<W, V> > dist_prev;

  dist_prev.emplace(source, std::pair<W, V>(W(), V()));
  std::unordered_set<V> q;
  for (const auto& v : graph.neighboursOf(source)) {
    q.insert(v);
    dist_prev.emplace(v, std::pair<W, V>(distanceCompute(source, v), source));
  }

  while (!q.empty()) {
    const V& u = closestNode<V, W>(q, dist_prev);
    q.erase(u);
    if (u == dest)
      break;

    for (const auto& v : graph.neighboursOf(u)) {
      const W d = distanceCompute(u, v);
      const W alt = dist_prev.at(u).first + d;

      if (dist_prev.find(v) == dist_prev.end()) { // new node
        dist_prev.emplace(v, std::pair<W, V>(alt, u));
        q.insert(v);
      } else if (alt < dist_prev.at(v).first) { // better route
        dist_prev[v] = std::pair<W, V>(alt, u);
      }
    }
  }

  return pathFromPrevList(dest, dist_prev);
}


#endif // GRAPH_ALGORITHMS_HPP
