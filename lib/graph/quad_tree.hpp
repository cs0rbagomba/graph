#ifndef QUAD_TREE_HPP
#define QUAD_TREE_HPP

#include <vector>
#include <utility> // move
#include <cassert>
#include <cmath> // std::fabs

// From wikipedia: http://en.wikipedia.org/wiki/Quadtree#Pseudo_code

/**
  P shall have x and y members and ctor with signature of: P(x, y)

   ~~~{.cpp}
  struct XY {
    float x;
    float y;
    XY(float _x, float _y) {...}
  }
  ~~~
 */

// Axis-aligned bounding box with half dimension and center
template <typename P>
struct AABB
{
  const P m_center;
  const typename P::value_type m_halfDimension;

  constexpr AABB(const P& center, typename P::value_type halfDimension)
    : m_center(center)
    , m_halfDimension(halfDimension) {}

  bool containsPoint(const P& p) const {
    return (std::fabs(m_center.x - p.x) <= m_halfDimension) &&
           (std::fabs(m_center.y - p.y) <= m_halfDimension);
  }

  bool intersectsAABB(const AABB& other) const {
      return (std::fabs(m_center.x - other.m_center.x) <= m_halfDimension + other.m_halfDimension) &&
             (std::fabs(m_center.y - other.m_center.y) <= m_halfDimension + other.m_halfDimension);
  }
};

template <typename P>
class QuadTree {
public:

  QuadTree(const AABB<P>& boundary)
    : m_boundary(boundary)
    , m_points()
    , m_northWest(0)
    , m_northEast(0)
    , m_southWest(0)
    , m_southEast(0) { m_points.reserve(m_node_capacity); }

  QuadTree(const QuadTree& other)
    : m_boundary (other.m_boundary)
    , m_points   (other.m_points)
    , m_northWest(other.m_northWest)
    , m_northEast(other.m_northEast)
    , m_southWest(other.m_southWest)
    , m_southEast(other.m_southEast) {}

  QuadTree(QuadTree&& other)
    : m_boundary (std::move(other.m_boundary))
    , m_points   (std::move(other.m_points))
    , m_northWest(std::move(other.m_northWest))
    , m_northEast(std::move(other.m_northEast))
    , m_southWest(std::move(other.m_southWest))
    , m_southEast(std::move(other.m_southEast)) {}

  QuadTree& operator=(const QuadTree other) { swap(other); return *this; }
  void swap(QuadTree& other) {
    std::swap(m_boundary,  other.m_boundary);
    std::swap(m_points,    other.m_points);
    std::swap(m_northWest, other.m_northWest);
    std::swap(m_northEast, other.m_northEast);
    std::swap(m_southWest, other.m_southWest);
    std::swap(m_southEast, other.m_southEast);
  }

  ~QuadTree() {
    delete m_northWest;
    delete m_northEast;
    delete m_southWest;
    delete m_southEast;
  }

  bool insert(const P& p) {
    // Ignore objects that do not belong in this quad tree
    if (!m_boundary.containsPoint(p))
      return false; // object cannot be added

    // If there is space in this quad tree, add the object here
    if (m_points.size() < m_node_capacity) {
      m_points.push_back(p);
      return true;
    }

    // Otherwise, subdivide and then add the point to whichever node will accept it
    if (m_northWest == 0)
      subdivide();

    if (m_northWest->insert(p)) return true;
    if (m_northEast->insert(p)) return true;
    if (m_southWest->insert(p)) return true;
    if (m_southEast->insert(p)) return true;

    // Otherwise, the point cannot be inserted for some unknown reason (this should never happen)
    assert(!"Programming error");
    return false;
  }

   // create four children that fully divide this quad into four quads of equal area
  void subdivide() {
    const typename P::value_type h = m_boundary.m_halfDimension / 2;
    const typename P::value_type x = m_boundary.m_center.x;
    const typename P::value_type y = m_boundary.m_center.y;
    m_northWest = new QuadTree<P>(AABB<P>(P(x - h, y - h), h));
    m_northEast = new QuadTree<P>(AABB<P>(P(x + h, y - h), h));
    m_southWest = new QuadTree<P>(AABB<P>(P(x - h, y + h), h));
    m_southEast = new QuadTree<P>(AABB<P>(P(x + h, y + h), h));
  }

  std::vector<P> queryRange(const AABB<P>& range) const {

    // Prepare an array of results
    std::vector<P> pointsInRange;

    // Automatically abort if the range does not intersect this quad
    if (!m_boundary.intersectsAABB(range))
      return pointsInRange; // empty list

    // Check objects at this quad level
    for (std::size_t p = 0; p < m_points.size(); ++p)
      if (range.containsPoint(m_points[p]))
        pointsInRange.push_back(m_points[p]);

    // Terminate here, if there are no children
    if (m_northWest == 0)
      return pointsInRange;

    // Otherwise, add the points from the children
    const std::vector<P> nw = m_northWest->queryRange(range);
    pointsInRange.insert(pointsInRange.end(), nw.begin(), nw.end());
    const std::vector<P> ne = m_northEast->queryRange(range);
    pointsInRange.insert(pointsInRange.end(), ne.begin(), ne.end());
    const std::vector<P> sw = m_southWest->queryRange(range);
    pointsInRange.insert(pointsInRange.end(), sw.begin(), sw.end());
    const std::vector<P> se = m_southEast->queryRange(range);
    pointsInRange.insert(pointsInRange.end(), se.begin(), se.end());

    return pointsInRange;
  }

  std::vector<P> points() const {

    std::vector<P> retval(m_points.begin(), m_points.end());

    // Terminate here, if there are no children
    if (m_northWest == 0)
      return retval;

    // Otherwise, add the points from the children
    const std::vector<P> nw = m_northWest->points();
    retval.insert(retval.end(), nw.begin(), nw.end());
    const std::vector<P> ne = m_northEast->points();
    retval.insert(retval.end(), ne.begin(), ne.end());
    const std::vector<P> sw = m_southWest->points();
    retval.insert(retval.end(), sw.begin(), sw.end());
    const std::vector<P> se = m_southEast->points();
    retval.insert(retval.end(), se.begin(), se.end());

    return retval;
  }


private:

  // Arbitrary constant to indicate how many elements can be stored in this quad tree node
  static const std::size_t m_node_capacity = 4;

  // Axis-aligned bounding box stored as a center with half-dimensions
  // to represent the boundaries of this quad tree
  const AABB<P> m_boundary;

  // Points in this quad tree node
  std::vector<P> m_points;

  // Children
  QuadTree* m_northWest;
  QuadTree* m_northEast;
  QuadTree* m_southWest;
  QuadTree* m_southEast;
};


#endif // QUAD_TREE_HPP
