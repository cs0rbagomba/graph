#include "marching_squares.hpp"

#include "floats.hpp"

#include <cstring> // for strerror needed by png++/error.hpp

#include <png++/png.hpp>

// #include <bitset>
#include <iostream>
#include <iomanip>

namespace {

    namespace Color {
      enum Code {
          FG_RED      = 31,
          FG_GREEN    = 32,
          FG_BLUE     = 34,
          FG_DEFAULT  = 39,
          BG_RED      = 41,
          BG_GREEN    = 42,
          BG_BLUE     = 44,
          BG_DEFAULT  = 49
      };
      class Modifier {
          Code code;
      public:
          Modifier(Code pCode) : code(pCode) {}
          friend std::ostream&
          operator<<(std::ostream& os, const Modifier& mod) {
              return os << "\033[" << mod.code << "m";
          }
      };
  }

  struct int2 {
    int x, y;
    constexpr int2 (int _x, int _y) : x(_x), y(_y) {}
    int2 (const int2& o) : x(o.x), y(o.y) {}
  };
  int2 operator+(const int2& a, const int2& b) { return int2(a.x+b.x, a.y+b.y); }
  float2 operator+(const float2& f, const int2& i) { return float2(f.x+i.x, f.y+i.y); }

  inline int getMaskAt(const std::vector< MarchingSquares::CellType >& cells, int width, int2 point) {
    const int x = point.x;
    const int y = point.y;
    const MarchingSquares::CellType quad[4] = {
        cells[(y-1) * width + (x-1)], cells[(y-1) * width + x],   // TL T
        cells[    y * width + (x-1)], cells[    y * width + x] }; // R  X

    const int mask = ((quad[0] == MarchingSquares::FREE) ? 0x0 : 0x1)
                   | ((quad[1] == MarchingSquares::FREE) ? 0x0 : 0x2)
                   | ((quad[2] == MarchingSquares::FREE) ? 0x0 : 0x4)
                   | ((quad[3] == MarchingSquares::FREE) ? 0x0 : 0x8);

    return mask;
  }
}

MarchingSquares::MarchingSquares()
  : width_(0)
  , height_(0)
  , cells_()
{
}

void MarchingSquares::ReadImage(const std::string& filename)
{
  png::image<png::gray_pixel> image(filename); // throws std_error(filename);

  width_  = image.get_width();
  height_ = image.get_height();
  cells_.reserve(width_ * height_);

  for (size_t y = 0; y < height_; ++y) {
    const png::image<png::gray_pixel>::row_type& row = image[y];
    for (size_t x = 0; x < width_; ++x) {
      CellType c; // map pixel luminance to cell type
      if (row[x] < 16)        c = SOLID;       // black
      else if (row[x] >= 240) c = FREE;        // white
      else                    c = DESTROYABLE; // everything else

      // mark borders as SOLID, such that the entities in the world cannot fall off
      if (x == 0 || x == width_ - 1 || y == 0 || y == height_ - 1)
        c = SOLID;

      cells_.push_back(c);
    }
  }
}


std::vector< std::pair<float2, float2> >
MarchingSquares::RunMarchingSquares() {

//   constexpr float2 points[8] = { // clockwise, starting in top-left corner
//         float2(-1,    -1   ), // TL 0
//         float2(-0.5f, -1   ), // T  1
//         float2(0,     -1   ), // TR 2
//         float2(0,     -0.5f), // R  3
//         float2(0,     0     ), // BR 4
//         float2(-0.5f, 0    ), // B  5
//         float2(-1,    0    ), // BL 6
//         float2(-1,    -0.5f)  // L  7
//       };

//   constexpr int2 step[16] = { // clockwise, starting in top-left corner
//         int2(0, 0), // 0x0
//         int2(1, 1), // 0x1
//         int2(1, 0), // 0x2
//         int2(1, 0), // 0x3
//         int2(-1, 1), // 0x4
//         int2(0, 1), // 0x5
//         int2(0, 0), // 0x6
//         int2(1, 0), // 0x7
//         int2(1, 0), // 0x8
//         int2(0, 0), // 0x9
//         int2(0, 1), // 0xa
//         int2(0, 0), // 0xb
//         int2(1, 0), // 0xc
//         int2(1, 0), // 0xd
//         int2(0, 0), // 0xe
//         int2(0, 0) // 0xf
//       };

//   constexpr int next[16] = {
//     0x0,  // x0x0
//     0x0,  // x0x1
//     0x3,  // x0x2
//     0x0,  // x0x3 // DC
//     0x0,  // x0x4
//     0x0,  // x0x5
//     0x0,  // x0x6
//     0x3,  // x0x7
//     0xc,  // x0x8
//     0x0,  // x0x9
//     0x0,  // x0xa
//     0x0,  // x0xb
//     0x0,  // x0xc
//     0xc,  // x0xd
//     0x0,  // x0xe
//     0x0   // x0xf  // DC
//   };

//   constexpr float2 center(0.5, 0.5);
//   constexpr float2 center(0.0, 0.0);

  const int number_of_cells = width_*height_;
  std::vector<bool> visited(number_of_cells, false);

  std::vector< std::pair<float2, float2> > lines;
  int2 point(1, 1);
  for (point.y = 1 ;point.y < height_; ++point.y) {
    for (point.x = 1; point.x < width_; ++point.x) {

      const int mask = getMaskAt(cells_, width_, point);

//       // these are the marching squares cases
//       int s = -1, e = -1;
//       switch (mask) {
//         case 0x0: /* all free, nothing to do */      break;
//
//         case 0x1: /* TL       = TL  */ s = 7; e = 1; break;
//         case 0x2: /*   TR     = TR  */ s = 3; e = 1; break;
//         case 0x4: /*     BL   = BL  */ s = 5; e = 7; break;
//         case 0x8: /*       BR = BR  */ s = 5; e = 3; break;
//
//         case 0x3: /* TLTR     = top    */ s = 7; e = 3; break;
//         case 0x5: /* TL  BL   = left   */ s = 1; e = 5; break;
//         case 0xa: /*   TR  BR = right  */ s = 1; e = 5; break;
//         case 0xc: /*     BLBR = bottom */ s = 7; e = 3; break;
//
//         // two lines
//         case 0x6: /*   TRBL   = left desc diagonal */ s = 1 | 5 << 4; e = 3 | 7 << 4; break;
//         case 0x9: /* TL    BR = right asc diagonal */ s = 7 | 3 << 4; e = 1 | 5 << 4; break;
//
//         case 0x7: /* TLTRBL   = BR free */ s = 3; e = 5; break;
//         case 0xb: /* TLTR  BR = BL free */ s = 5; e = 7; break;
//         case 0xd: /* TL  BLBR = TR free */ s = 1; e = 3; break;
//         case 0xe: /*   TRBLBR = TL free */ s = 7; e = 1; break;
//
//         case 0xf: /* all blocked, nothing to do */       break;
//       }

      Color::Modifier blue(Color::FG_BLUE);
      Color::Modifier def(Color::FG_DEFAULT);
      if (mask != 0)
        std::cout << std::left << std::setw(3) << mask;
      else
        std::cout << blue << mask << def << "  ";

      if (visited[point.y*width_ + point.x] == true)
        continue;

      if (mask == 0x0 || mask == 0xf) // empty or full
        continue;

      // vertical lines start with an edge
      assert(mask !=  3);
      assert(mask != 12);
      assert(mask !=  5);
      assert(mask != 10);

      if (mask == 0x7 || mask == 0x2 || mask == 0x8 || mask == 0xd || mask == 0x6 || mask == 0x9) {
        int2 i = point;
        int counter = 0;
        while (true) {
          int2 n = int2(i.x + 1, i.y);
          int next_mask = getMaskAt(cells_, width_, n);
          if (n.x < width_ && point.y < height_ &&
             (next_mask == 0x3 || next_mask == 0xc) &&
             visited[n.y*width_ + n.x] == false) {
            visited[n.y*width_ + n.x] = true;
            i = n;
            ++counter;
          } else
              break;
        }
        lines.push_back(std::pair<float2, float2>(float2(point.x, point.y),  float2( i.x + 1, i.y)));
      }

      if (mask == 0x7 || mask == 0xb || mask == 0x4 || mask == 0x8 || mask == 0x6 || mask == 0x9) {
        int2 i = point;
        int counter = 0;
        while (true) {
          int2 n = int2(i.x, i.y + 1);
          int next_mask = getMaskAt(cells_, width_, n);
          if (n.x < width_ && point.y < height_ &&
             (next_mask == 0xa || next_mask == 0x5) &&
             visited[n.y*width_ + n.x] == false) {
            visited[n.y*width_ + n.x] = true;
            i = n;
            ++counter;
          } else
              break;
        }
        lines.push_back(std::pair<float2, float2>(float2(point.x, point.y),  float2( i.x, i.y + 1)));
      }

      visited[point.y*width_ + point.x] = true;
    }
    std::cout << std::endl;
  }

  // build quadtree and combine lines (there are many, many small pieces around now, combine to longer lines)

  return lines;
}
