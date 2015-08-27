#ifndef GRAPH_ALGORITHMS_HPP
#define GRAPH_ALGORITHMS_HPP

#include "graph.hpp"
#include "priority_queue.hpp"

#include <vector>
#include <unordered_map>

#include <queue>
#include <set>

#include <utility>
#include <algorithm>
#include <functional>
#include <type_traits>


namespace {

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
  PriorityQueue<W, V> q;
  for (const auto& v : graph.neighboursOf(source)) {
    const W d = distanceCompute(source, v);
    q.push(d, v);
    dist_prev.emplace(v, std::pair<W, V>(d, source));
  }

  while (!q.empty()) {
    const V u = q.top().second;
    q.pop();

    if (u == dest)
      break;

    for (const auto& v : graph.neighboursOf(u)) {
      const W d = distanceCompute(u, v);
      const W alt = dist_prev.at(u).first + d;

      auto& v_ref = dist_prev[v];
      if (v_ref.first == W()) { // new node
        v_ref = std::pair<W, V>(alt, u);
        q.push(alt, v);
      } else {
        const W prev_d = v_ref.first;
        if (alt < prev_d) { // better route
          v_ref = std::pair<W, V>(alt, u);
          q.modifyKey(prev_d, v, alt);
        }
      }
    }
  }

  return pathFromPrevList(dest, dist_prev);
}


#endif // GRAPH_ALGORITHMS_HPP
