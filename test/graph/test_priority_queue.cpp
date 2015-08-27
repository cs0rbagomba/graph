#include <graph/priority_queue.hpp>

#include "../catch.hpp"

#include "fixture.hpp"



TEST_CASE("Priority queue", "[priority_queue][data_structure]" ) {

typedef float W;
typedef float2 V;
typedef std::pair<W, V> P;

  SECTION("empty") {
    PriorityQueue<W, V> pq;

    REQUIRE( pq.size() == 0 );
    REQUIRE( pq.empty() == true );
  }

  SECTION("One element") {
    PriorityQueue<W, V> pq;
    const V f2(1.0f, 1.0f);
    pq.push(1.0f, f2);

    REQUIRE( pq.size() == 1 );
    REQUIRE( pq.empty() == false );

    P p = pq.top();
    REQUIRE( p.second == f2 );

    pq.pop();
    REQUIRE( pq.size() == 0 );
    REQUIRE( pq.empty() == true );
  }

  SECTION("Multiple elements") {
    PriorityQueue<W, V> pq;
    const V f2_1(1.0f, 1.0f);
    const V f2_2(2.0f, 2.0f);
    const V f2_3(3.0f, 3.0f);

    pq.push(3.0f, f2_3);
    pq.push(1.0f, f2_1);
    pq.push(2.0f, f2_2);

    REQUIRE( pq.size() == 3 );
    REQUIRE( pq.empty() == false );

    P p1 = pq.top();
    pq.pop();
    REQUIRE( p1.first == 1.0f );
    REQUIRE( p1.second == f2_1 );
    REQUIRE( pq.size() == 2 );

    P p2 = pq.top();
    pq.pop();
    REQUIRE( p2.first == 2.0f );
    REQUIRE( p2.second == f2_2 );
    REQUIRE( pq.size() == 1 );

    P p3 = pq.top();
    pq.pop();
    REQUIRE( p3.first == 3.0f );
    REQUIRE( p3.second == f2_3 );

    REQUIRE( pq.size() == 0 );
    REQUIRE( pq.empty() == true );
  }

  SECTION("Modify key") {
    PriorityQueue<W, V> pq;
    const V f2_1(1.0f, 1.0f);
    const V f2_2(2.0f, 2.0f);
    const V f2_3(3.0f, 3.0f);

    pq.push(3.0f, f2_3);
    pq.push(1.0f, f2_1);
    pq.push(2.0f, f2_2);

    pq.modifyKey(1.0f, f2_1, 1.0f + 3.0f);
    pq.modifyKey(3.0f, f2_3, 3.0f - 2.0f);

    P p1 = pq.top();
    pq.pop();
    REQUIRE( p1.first == 1.0f );
    REQUIRE( p1.second == f2_3 );

    P p2 = pq.top();
    pq.pop();
    REQUIRE( p2.first == 2.0f );
    REQUIRE( p2.second == f2_2 );

    P p3 = pq.top();
    pq.pop();
    REQUIRE( p3.first == 4.0f );
    REQUIRE( p3.second == f2_1 );
  }

}
