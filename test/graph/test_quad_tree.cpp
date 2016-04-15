#include <graph/quad_tree.hpp>

#include "../catch.hpp"

#include "fixture.hpp"

#include <iostream>

TEST_CASE( "Quad tree, AABB", "[quad_tree][data_structure][AABB]" ) {

  const float2 center(0, 0);
  constexpr float halfDimension(10);
  const AABB<float2> boundary(center, halfDimension);

  SECTION("containsPoint in") {
    REQUIRE ( boundary.containsPoint(center) == true );

    REQUIRE ( boundary.containsPoint(float2(center.x - halfDimension, center.y)) == true );
    REQUIRE ( boundary.containsPoint(float2(center.x + halfDimension, center.y)) == true );
    REQUIRE ( boundary.containsPoint(float2(center.x, center.y - halfDimension)) == true );
    REQUIRE ( boundary.containsPoint(float2(center.x, center.y + halfDimension)) == true );

    REQUIRE ( boundary.containsPoint(float2(center.x - halfDimension, center.y - halfDimension)) == true );
    REQUIRE ( boundary.containsPoint(float2(center.x - halfDimension, center.y + halfDimension)) == true );
    REQUIRE ( boundary.containsPoint(float2(center.x + halfDimension, center.y - halfDimension)) == true );
    REQUIRE ( boundary.containsPoint(float2(center.x + halfDimension, center.y + halfDimension)) == true );
  }

  SECTION("containsPoint out") {
    constexpr float halfDimension_1 = halfDimension + 1.0;
    REQUIRE ( boundary.containsPoint(float2(center.x - halfDimension_1, center.y)) == false );
    REQUIRE ( boundary.containsPoint(float2(center.x + halfDimension_1, center.y)) == false );
    REQUIRE ( boundary.containsPoint(float2(center.x, center.y - halfDimension_1)) == false );
    REQUIRE ( boundary.containsPoint(float2(center.x, center.y + halfDimension_1)) == false );

    REQUIRE ( boundary.containsPoint(float2(center.x - halfDimension_1, center.y - halfDimension_1)) == false );
    REQUIRE ( boundary.containsPoint(float2(center.x - halfDimension_1, center.y + halfDimension_1)) == false );
    REQUIRE ( boundary.containsPoint(float2(center.x + halfDimension_1, center.y - halfDimension_1)) == false );
    REQUIRE ( boundary.containsPoint(float2(center.x + halfDimension_1, center.y + halfDimension_1)) == false );
  }

  SECTION("containsPoint invalid") {

    constexpr float halfDimension_1 = halfDimension + 1.0;
    REQUIRE ( boundary.containsPoint(float2(center.x - halfDimension_1, center.y)) == false );
    REQUIRE ( boundary.containsPoint(float2(center.x + halfDimension_1, center.y)) == false );
    REQUIRE ( boundary.containsPoint(float2(center.x, center.y - halfDimension_1)) == false );
    REQUIRE ( boundary.containsPoint(float2(center.x, center.y + halfDimension_1)) == false );

    REQUIRE ( boundary.containsPoint(float2(center.x - halfDimension_1, center.y - halfDimension_1)) == false );
    REQUIRE ( boundary.containsPoint(float2(center.x - halfDimension_1, center.y + halfDimension_1)) == false );
    REQUIRE ( boundary.containsPoint(float2(center.x + halfDimension_1, center.y - halfDimension_1)) == false );
    REQUIRE ( boundary.containsPoint(float2(center.x + halfDimension_1, center.y + halfDimension_1)) == false );
  }

  SECTION("intersectsAABB") {

    constexpr float halfDimension_1 = halfDimension + 1.0;
    REQUIRE( boundary.intersectsAABB(AABB<float2>(center, 1.0)) == true);
    REQUIRE( boundary.intersectsAABB(AABB<float2>(center, halfDimension)) == true);
    REQUIRE( boundary.intersectsAABB(AABB<float2>(center, halfDimension_1)) == true);

    const float2 center2(center.x + halfDimension, center.y);
    REQUIRE( boundary.intersectsAABB(AABB<float2>(center2, 0.0)) == true);
    REQUIRE( boundary.intersectsAABB(AABB<float2>(center2, halfDimension)) == true);
    REQUIRE( boundary.intersectsAABB(AABB<float2>(center2, halfDimension_1)) == true);

    const float2 center3(center.x + halfDimension * 2, center.y);
    REQUIRE( boundary.intersectsAABB(AABB<float2>(center3, 0.0)) == false);
    REQUIRE( boundary.intersectsAABB(AABB<float2>(center3, halfDimension)) == true);
    REQUIRE( boundary.intersectsAABB(AABB<float2>(center3, halfDimension_1)) == true);

    const float2 center4(center.x + halfDimension_1 * 2, center.y);
    REQUIRE( boundary.intersectsAABB(AABB<float2>(center4, 0.0)) == false);
    REQUIRE( boundary.intersectsAABB(AABB<float2>(center4, halfDimension)) == false);
    REQUIRE( boundary.intersectsAABB(AABB<float2>(center4, halfDimension_1)) == false);
  }

    SECTION("special") {

      // std::fabs instead of std::abs, which would turn float->int
      const AABB<float2> boundary2(float2(-3.75, -8.75), 1.25);
      REQUIRE( boundary2.containsPoint(float2(-2, -8)) == false);
    }
}

TEST_CASE( "Quad tree", "[quad_tree][data_structure]" ) {

  const float2 center(0, 0);
  constexpr float halfDimension(10);
  const AABB<float2> boundary(center, halfDimension);
  QuadTree<float2> t(boundary);

  SECTION("Insert point") {
    REQUIRE ( t.insert(float2(0, 0)) == true );
  }
  SECTION("Insert point 2x") {
    REQUIRE ( t.insert(float2(0, 0)) == true );
    REQUIRE ( t.insert(float2(0, 0)) == true );
  }

  SECTION("Insert point outside") {
    REQUIRE ( t.insert(float2(20, 20)) == false );
  }

  SECTION("Insert some points") {
    for (int c = -3; c < 3; ++c)
      for (int r = -3; r < 3; ++r)
        REQUIRE ( t.insert(float2(center.x + r, center.y + c)) == true );

    const std::vector<float2> all_points = t.points();
    REQUIRE ( all_points.size() == 6*6 );
  }

  SECTION("Insert many points") {
    for (int c = -halfDimension; c < halfDimension; ++c)
      for (int r = -halfDimension; r < halfDimension; ++r)
        REQUIRE ( t.insert(float2(center.x + r, center.y + c)) == true );

    const std::vector<float2> all_points = t.points();
    REQUIRE ( all_points.size() == std::size_t(halfDimension*2 * halfDimension*2) ); // 400
  }

  SECTION("Query empty tree") {
    REQUIRE ( t.queryRange(boundary).size() == 0 );
  }

  SECTION("Query 1") {
    const float2 p1(0, 0);
    t.insert(p1);
    const std::vector<float2> points =  t.queryRange(boundary);

    REQUIRE ( points.size() == 1);
    REQUIRE ( points[0] == p1);
  }

  SECTION("Query 1, bad range") {
    const float2 p1(0, 0);
    t.insert(p1);
    const std::vector<float2> points =  t.queryRange(AABB<float2>(float2(-halfDimension, -halfDimension), halfDimension/2));
    REQUIRE ( points.size() == 0);
  }

  SECTION("Query intersection") {
    const float2 p1(0, 0);
    const float2 p2(-halfDimension, -halfDimension);
    t.insert(p1);
    t.insert(p2);
    const std::vector<float2> points =  t.queryRange(AABB<float2>(float2(-halfDimension, -halfDimension), halfDimension/2));

    REQUIRE ( points.size() == 1);
    REQUIRE ( points[0] == p2);
  }

  SECTION("Query many points topLeft") {
    for (int c = -halfDimension; c < halfDimension; ++c)
      for (int r = -halfDimension; r < halfDimension; ++r)
        REQUIRE ( t.insert(float2(center.x + r, center.y + c)) == true );

    const float2 topLeft(-halfDimension, -halfDimension);
    const std::vector<float2> points =  t.queryRange(AABB<float2>(topLeft, halfDimension/2));

    REQUIRE ( points.size() == 36);
    for (int c = -halfDimension; c < -halfDimension/2; ++c)
      for (int r = -halfDimension; r < -halfDimension/2; ++r)
        REQUIRE ( std::find(points.begin(), points.end(), float2(center.x + r, center.y + c)) != points.end() );
  }

  SECTION("Query many points inside") {
    for (int c = -halfDimension; c < halfDimension; ++c)
      for (int r = -halfDimension; r < halfDimension; ++r)
        REQUIRE ( t.insert(float2(center.x + r, center.y + c)) == true );

    const float2 right(halfDimension/2, 0);
    const std::vector<float2> points =  t.queryRange(AABB<float2>(right, halfDimension/4));

    for (int c = -halfDimension/4; c < halfDimension/4; ++c)
      for (int r = std::ceil(halfDimension/4); r < halfDimension*0.75; ++r)
        REQUIRE ( std::find(points.begin(), points.end(), float2(center.x + r, center.y + c)) != points.end() );
  }
}
