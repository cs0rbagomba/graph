#include <graph/marching_squares.hpp>

#include "../catch.hpp"

#include "fixture.hpp"


TEST_CASE( "Marching squares", "[marching_squares][algorithm]" ) {

  SECTION("empty") {
    const ImageMatrix empty(0, 0, std::vector<ImageMatrix::CellType>());
    const std::vector< std::pair<size_t2, size_t2> > result = marchingSquares(empty);
    REQUIRE ( result.size() == 0 );
  }

  SECTION("dot") {
    const std::vector<ImageMatrix::CellType> dot_v {
      ImageMatrix::FREE, ImageMatrix::FREE,  ImageMatrix::FREE,
      ImageMatrix::FREE, ImageMatrix::SOLID, ImageMatrix::FREE,
      ImageMatrix::FREE, ImageMatrix::FREE,  ImageMatrix::FREE
    };
    const ImageMatrix dot_i(3, 3, dot_v);
    const std::vector< std::pair<size_t2, size_t2> > result = marchingSquares(dot_i);
    REQUIRE ( result.size() == 4 );

    REQUIRE ( std::find(result.begin(), result.end(), std::pair<size_t2, size_t2>(size_t2(1, 1), size_t2(2, 1)) ) != result.end() );
    REQUIRE ( std::find(result.begin(), result.end(), std::pair<size_t2, size_t2>(size_t2(1, 1), size_t2(1, 2)) ) != result.end() );
    REQUIRE ( std::find(result.begin(), result.end(), std::pair<size_t2, size_t2>(size_t2(2, 1), size_t2(2, 2)) ) != result.end() );
    REQUIRE ( std::find(result.begin(), result.end(), std::pair<size_t2, size_t2>(size_t2(1, 2), size_t2(2, 2)) ) != result.end() );
  }

  SECTION("horizontal line") {
    const std::vector<ImageMatrix::CellType> line_v {
      ImageMatrix::FREE, ImageMatrix::FREE,  ImageMatrix::FREE, ImageMatrix::FREE, ImageMatrix::FREE,
      ImageMatrix::FREE, ImageMatrix::SOLID, ImageMatrix::SOLID, ImageMatrix::SOLID, ImageMatrix::FREE,
      ImageMatrix::FREE, ImageMatrix::FREE, ImageMatrix::FREE,  ImageMatrix::FREE, ImageMatrix::FREE
    };
    const ImageMatrix line_i(5, 3, line_v);
    const std::vector< std::pair<size_t2, size_t2> > result = marchingSquares(line_i);
    REQUIRE ( result.size() == 4 );

    REQUIRE ( std::find(result.begin(), result.end(), std::pair<size_t2, size_t2>(size_t2(1, 1), size_t2(4, 1)) ) != result.end() );
    REQUIRE ( std::find(result.begin(), result.end(), std::pair<size_t2, size_t2>(size_t2(1, 1), size_t2(1, 2)) ) != result.end() );
    REQUIRE ( std::find(result.begin(), result.end(), std::pair<size_t2, size_t2>(size_t2(4, 1), size_t2(4, 2)) ) != result.end() );
    REQUIRE ( std::find(result.begin(), result.end(), std::pair<size_t2, size_t2>(size_t2(1, 2), size_t2(4, 2)) ) != result.end() );
  }

}

