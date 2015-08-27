#ifndef PRIORITY_QUEUE_HPP
#define PRIORITY_QUEUE_HPP

#include <map> // std::m_map
#include <algorithm> // std::find_if

/** @brief Priority Queu with top, push, pop, modifyKey.
 *
 * The priority queue based Dijkstra (shortest path) algorith requires the
 * modifyKey functionality, which the std::priority_queue does not have.
 *
 * @note modifyKey is very ineffective, since std::map is not a
 * bidirectional map, looking up an element based on value is linear.
 *
 * @todo replace std::map with a Fibonacci heap to improve performance.
 */

template <
  typename Key,
  typename T,
  typename Compare = std::less<Key>,
  typename Allocator = std::allocator<std::pair<const Key, T> >
>
class PriorityQueue
{
public:

  PriorityQueue() : m_map() {}

  // capacity
  size_t size() const noexcept { return m_map.size(); }
  bool empty() const noexcept { return m_map.empty(); }

  // lookup
  std::pair<Key, T> top() const noexcept { return *m_map.begin(); }

  // modifiers
  void pop() { m_map.erase(m_map.begin()); }
  void push(const Key& key, const T& value) { m_map.emplace(key, value); }
  bool modifyKey(const Key& key, const T& value, const Key& new_key) {

    const auto eq_it = m_map.equal_range(key);
    if (eq_it.first == m_map.end()) return false;

    const auto it = std::find_if(eq_it.first, eq_it.second, [&value](const std::pair<const Key, T> & p) { return p.second == value; } );
    if (it == m_map.end()) return false;

    auto v = it->second; // take a copy
    m_map.erase(it);
    m_map.emplace(new_key, v);
    return true;
  }

private:
  std::multimap<Key, T, Compare, Allocator> m_map;
};

#endif // PRIORITY_QUEUE_HPP
