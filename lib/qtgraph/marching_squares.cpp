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

      // mark white cells sorrounded by 4 black cells as black
//       if (y > 1 && y < height_-1 && x > 1 && x < width_-1 &&
//           image[y-1][x] < 240 &&
//           image[y+1][x] < 240 &&
//           image[y][x-1] < 240 &&
//           image[y][x+1] < 240)
//         c = DESTROYABLE;


      cells_.push_back(c);
    }
  }
}


std::vector< std::pair<float2, float2> >
MarchingSquares::RunMarchingSquares() {

  const int number_of_cells = width_*height_;
  std::vector<bool> visited(number_of_cells, false);

  std::vector< std::pair<float2, float2> > lines;
  int2 point(1, 1);
  for (point.y = 1 ;point.y < height_; ++point.y) {
    for (point.x = 1; point.x < width_; ++point.x) {

      const int mask = getMaskAt(cells_, width_, point);

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

      // vertical/horizontal lines start with an edge
      assert(mask !=  3);
      assert(mask != 12);
      assert(mask !=  5);
      assert(mask != 10);

      // cannot reach nonvisited bottomright corner
//       assert(mask != 1);
//       assert(mask != 14);

      if (mask == 0x7 || mask == 0x2 || mask == 0x8 || mask == 0xd || mask == 0x6 || mask == 0x9) {
        int2 i = point;
        int counter = 0;
        while (true) {
          int2 n = int2(i.x + 1, i.y);
          int next_mask = getMaskAt(cells_, width_, n);
          if (n.x < width_ && point.y < height_ &&
             ( ((mask == 0x7 || mask == 0x2 || mask == 0x6) && next_mask == 0x3) ||
               ((mask == 0x8 || mask == 0xd || mask == 0x9) && next_mask == 0xc)) &&
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
             ( ((mask == 0x7 || mask == 0x4 || mask == 0x6) && next_mask == 0x5) ||
               ((mask == 0xb || mask == 0x8 || mask == 0x9) && next_mask == 0xa)) &&
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

  return lines;
}
