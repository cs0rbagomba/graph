#include <graph/priority_queue.hpp>

#include "../catch.hpp"

#include "fixture.hpp"



TEST_CASE("Priority queue", "[priority_queue][data_structure]" ) {

typedef float W;
typedef float2 V;
typedef std::pair<W, V> P;

  SECTION("empty") {
    const PriorityQueue<W, V> pq;

    REQUIRE( pq.size() == 0 );
    REQUIRE( pq.empty() == true );
  }

  SECTION("One element") {
    PriorityQueue<W, V> pq;
    const V f2(1.0f, 1.0f);
    pq.push(1.0f, f2);

    REQUIRE( pq.size() == 1 );
    REQUIRE( pq.empty() == false );

    const P p = pq.top();
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

    const P p1 = pq.top(); pq.pop();
    REQUIRE( p1.first == 1.0f );
    REQUIRE( p1.second == f2_1 );
    REQUIRE( pq.size() == 2 );

    const P p2 = pq.top(); pq.pop();
    REQUIRE( p2.first == 2.0f );
    REQUIRE( p2.second == f2_2 );
    REQUIRE( pq.size() == 1 );

    const P p3 = pq.top(); pq.pop();
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

    const P p1 = pq.top(); pq.pop();
    REQUIRE( p1.first == 1.0f );
    REQUIRE( p1.second == f2_3 );

    const P p2 = pq.top(); pq.pop();
    REQUIRE( p2.first == 2.0f );
    REQUIRE( p2.second == f2_2 );

    const P p3 = pq.top(); pq.pop();
    REQUIRE( p3.first == 4.0f );
    REQUIRE( p3.second == f2_1 );
  }

  SECTION("Same key multivalue") {
    PriorityQueue<W, V> pq;
    const V f2_1(1.0f, 1.0f);
    const V f2_21(2.0f, 21.0f);
    const V f2_22(2.0f, 22.0f);
    const V f2_23(2.0f, 23.0f);
    const V f2_24(2.0f, 24.0f);
    const V f2_34(3.0f, 34.0f);
    const V f2_33(3.0f, 32.0f);

    pq.push(3.0f, f2_34);
    pq.push(1.0f, f2_1);
    pq.push(2.0f, f2_23);
    pq.push(3.0f, f2_33);
    pq.push(2.0f, f2_21);
    pq.push(2.0f, f2_22);

    const P p1 = pq.top(); pq.pop();
    REQUIRE( p1.first == 1.0f );
    REQUIRE( p1.second == f2_1 );

    // priority queue has a multimap as underlying data_structure:
    // "If the container has elements with equivalent key, inserts at the upper bound of that range"
    // so with same keys, element pop order is same as adding order
    const P p2 = pq.top(); pq.pop();
    REQUIRE( p2.first == 2.0f );
    REQUIRE( p2.second == f2_23 );
    const P p3 = pq.top(); pq.pop();
    REQUIRE( p3.first == 2.0f );
    REQUIRE( p3.second == f2_21 );
    const P p4 = pq.top(); pq.pop();
    REQUIRE( p4.first == 2.0f );
    REQUIRE( p4.second == f2_22 );

    const P p5 = pq.top(); pq.pop();
    REQUIRE( p5.first == 3.0f );
    REQUIRE( p5.second == f2_34 );
    const P p6 = pq.top(); pq.pop();
    REQUIRE( p6.first == 3.0f );
    REQUIRE( p6.second == f2_33 );
  }

    SECTION("Same key multivalue modyfykey") {
    PriorityQueue<W, V> pq;
    const V f2_1(1.0f, 1.0f);
    const V f2_21(2.0f, 21.0f);
    const V f2_22(2.0f, 22.0f);
    const V f2_23(2.0f, 23.0f);
    const V f2_24(2.0f, 24.0f);
    const V f2_34(3.0f, 34.0f);
    const V f2_33(3.0f, 32.0f);

    pq.push(3.0f, f2_34);
    pq.push(1.0f, f2_1);
    pq.push(2.0f, f2_23);
    pq.push(3.0f, f2_33);
    pq.push(2.0f, f2_21);
    pq.push(2.0f, f2_22);

    pq.modifyKey(2.0f, f2_21, 1.0f);
    pq.modifyKey(3.0f, f2_33, 2.0f);

    const P p1 = pq.top(); pq.pop();
    REQUIRE( p1.first == 1.0f );
    REQUIRE( p1.second == f2_1 );
    const P p3 = pq.top(); pq.pop();
    REQUIRE( p3.first == 1.0f );
    REQUIRE( p3.second == f2_21 );

    const P p2 = pq.top(); pq.pop();
    REQUIRE( p2.first == 2.0f );
    REQUIRE( p2.second == f2_23 );
    const P p4 = pq.top(); pq.pop();
    REQUIRE( p4.first == 2.0f );
    REQUIRE( p4.second == f2_22 );
    const P p6 = pq.top(); pq.pop();
    REQUIRE( p6.first == 2.0f );
    REQUIRE( p6.second == f2_33 );

    const P p5 = pq.top(); pq.pop();
    REQUIRE( p5.first == 3.0f );
    REQUIRE( p5.second == f2_34 );

  }
}
