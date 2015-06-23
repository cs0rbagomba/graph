#include "marching_squares.hpp"

#include "floats.hpp"

#include <cstring> // for strerror needed by png++/error.hpp

#include <png++/png.hpp>

// #include <bitset>
#include <iostream>

namespace {

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

  constexpr float2 points[8] = { // clockwise, starting in top-left corner
        float2(-1,    -1   ), // TL 0
        float2(-0.5f, -1   ), // T  1
        float2(0,     -1   ), // TR 2
        float2(0,     -0.5f), // R  3
        float2(0,     0     ), // BR 4
        float2(-0.5f, 0    ), // B  5
        float2(-1,    0    ), // BL 6
        float2(-1,    -0.5f)  // L  7
      };

  constexpr int2 step[16] = { // clockwise, starting in top-left corner
        int2(0, 0), // 0x0
        int2(1, 1), // 0x1
        int2(-1, 1), // 0x2
        int2(1, 0), // 0x3
        int2(-1, 1), // 0x4
        int2(0, 1), // 0x5
        int2(0, 0), // 0x6
        int2(0, 0), // 0x7
        int2(1, 1), // 0x8
        int2(0, 0), // 0x9
        int2(0, 1), // 0xa
        int2(0, 0), // 0xb
        int2(1, 0), // 0xc
        int2(0, 0), // 0xd
        int2(0, 0), // 0xe
        int2(0, 0) // 0xf
      };

  constexpr float2 center(0.5, 0.5);

  const int number_of_cells = width_*height_;
  std::vector<bool> visited(number_of_cells, false);

  std::vector< std::pair<float2, float2> > lines;
  int2 point(1, 1);
  for (point.y = 1 ;point.y < height_; ++point.y) {
    for (point.x = 1; point.x < width_; ++point.x) {

      const int mask = getMaskAt(cells_, width_, point);

      // these are the marching squares cases
      int s = -1, e = -1;
      switch (mask) {
        case 0x0: /* all free, nothing to do */      break;

        case 0x1: /* TL       = TL  */ s = 7; e = 1; break;
        case 0x2: /*   TR     = TR  */ s = 3; e = 1; break;
        case 0x4: /*     BL   = BL  */ s = 5; e = 7; break;
        case 0x8: /*       BR = BR  */ s = 5; e = 3; break;

        case 0x3: /* TLTR     = top    */ s = 7; e = 3; break;
        case 0x5: /* TL  BL   = left   */ s = 1; e = 5; break;
        case 0xa: /*   TR  BR = right  */ s = 1; e = 5; break;
        case 0xc: /*     BLBR = bottom */ s = 7; e = 3; break;

        // two lines
        case 0x6: /*   TRBL   = left desc diagonal */ s = 1 | 5 << 4; e = 3 | 7 << 4; break;
        case 0x9: /* TL    BR = right asc diagonal */ s = 7 | 3 << 4; e = 1 | 5 << 4; break;

        case 0x7: /* TLTRBL   = BR free */ s = 3; e = 5; break;
        case 0xb: /* TLTR  BR = BL free */ s = 5; e = 7; break;
        case 0xd: /* TL  BLBR = TR free */ s = 1; e = 3; break;
        case 0xe: /*   TRBLBR = TL free */ s = 7; e = 1; break;

        case 0xf: /* all blocked, nothing to do */       break;
      }

      std::cout << mask << " ";

      if (visited[point.y*width_ + point.x] == true)
        continue;

      if (s == -1) {
        // assert: e == -1
      }

//       if (mask == 0x7)
//         continue;

      else if ((s & 0xf) == s) {
        // assert: (e & 0xf) == e
//         lines.push_back(std::pair<float2, float2>(points[s]+center+point, points[e]+center+point));

        if (mask == 0x3 || mask == 0x5 || mask == 0xa || mask == 0xc
//            || mask == 0x1 || mask == 0x2 || mask == 0x4 || mask == 0x8
           ) {
          int2 i = point;
          while (true) {
            int2 next = i + step[mask];
            if (next.x < width_ && point.y < height_ && getMaskAt(cells_, width_, next) == mask && visited[next.y*width_ + next.x] == false) {
              visited[next.y*width_ + next.x] = true;
              i = next;
            } else
              break;
          }
          lines.push_back(std::pair<float2, float2>(points[s]+center+point, points[e]+center+i));
        }
        else
          lines.push_back(std::pair<float2, float2>(points[s]+center+point, points[e]+center+point));
      }
      else {
        int s1 = s & 0xf, e1 = e & 0xf;
        int s2 = s >> 4,  e2 = e >> 4;
        lines.push_back(std::pair<float2, float2>(points[s1]+center+point, points[e1]+center+point));
        lines.push_back(std::pair<float2, float2>(points[s2]+center+point, points[e2]+center+point));
      }

      visited[point.y*width_ + point.x] = true;
    }
    std::cout << std::endl;
  }

  // build quadtree and combine lines (there are many, many small pieces around now, combine to longer lines)

  return lines;
}
