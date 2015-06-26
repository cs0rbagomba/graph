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
    void operator+=(const int2 o) { x += o.x; y += o.y; }
  };
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

      /// @todo extend image?
      // mark borders as SOLID, such that the entities in the world cannot fall off
      if (x == 0 || x == width_ - 1 || y == 0 || y == height_ - 1)
        c = SOLID;

      // mark white "hole" cells sorrounded by 4 black cells as black
      if (y > 1 && y < height_-1 && x > 1 && x < width_-1 &&
          image[y][x] >= 240 &&
          image[y-1][x] < 240 &&
          image[y+1][x] < 240 &&
          image[y][x-1] < 240 &&
          image[y][x+1] < 240)
        c = SOLID;

      /// @note is it too much?
      // mark black "lost" cells sorrounded by 4 white cells as white
//       if (y > 1 && y < height_-1 && x > 1 && x < width_-1 &&
//           image[y][x] < 240 &&
//          (image[y-1][x-1] >= 240 && image[y-1][x] >= 240 && image[y-1][x+1] >= 240 && image[y][x+1] >= 240 && image[y+1][x+1] >= 240 && image[y+1][x] >= 240 && image[y+1][x-1] >= 240 && image[y][x-1] >= 240))
//         c = FREE;

//       // mark black "bump on the wall" cells as white
//       if (y > 1 && y < height_-1 && x > 1 && x < width_-1 &&
//           image[y][x] < 240 &&
//            ((image[y-1][x-1] < 240 && image[y-1][x] < 240 && image[y-1][x+1] < 240 && image[y][x+1] >= 240 && image[y+1][x+1] >= 240 && image[y+1][x] >= 240 && image[y+1][x-1] >= 240 && image[y][x-1] >= 240) ||
//             (image[y-1][x-1] >= 240 && image[y-1][x] >= 240 && image[y-1][x+1] < 240 && image[y][x+1] < 240 && image[y+1][x+1] < 240 && image[y+1][x] >= 240 && image[y+1][x-1] >= 240 && image[y][x-1] >= 240) ||
//             (image[y-1][x-1] >= 240 && image[y-1][x] >= 240 && image[y-1][x+1] >= 240 && image[y][x+1] >= 240 && image[y+1][x+1] < 240 && image[y+1][x] < 240 && image[y+1][x-1] < 240 && image[y][x-1] >= 240) ||
//             (image[y-1][x-1] < 240 && image[y-1][x] >= 240 && image[y-1][x+1] >= 240 && image[y][x+1] >= 240 && image[y+1][x+1] >= 240 && image[y+1][x] >= 240 && image[y+1][x-1] < 240 && image[y][x-1] < 240)))
//         c = FREE;

      cells_.push_back(c);
    }
  }
}


std::vector< std::pair<float2, float2> >
MarchingSquares::RunMarchingSquares() {

  std::vector<bool> visited(width_*height_, false);
  std::vector< std::pair<float2, float2> > lines;

  int2 point(1, 1);
  for (point.y = 1 ;point.y < (int)height_; ++point.y) {
    for (point.x = 1; point.x < (int)width_; ++point.x) {

      const int mask = getMaskAt(point.x, point.y);

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
//       assert(mask !=  3);
//       assert(mask != 12);
//       assert(mask !=  5);
//       assert(mask != 10);

      // cannot reach nonvisited bottomright corner
      /// @note you can. the last is not visited
//       assert(mask != 1);
//       assert(mask != 14);

      visitPoint(point.x, point.y, mask, visited, lines);
    }
    std::cout << std::endl;
  }

  return lines;
}

int MarchingSquares::getMaskAt(int x, int y) const
{
  const CellType quad[4] = {
      cells_[(y-1) * width_ + (x-1)], cells_[(y-1) * width_ + x],   // TL T
      cells_[    y * width_ + (x-1)], cells_[    y * width_ + x] }; // R  X

  const int mask = ((quad[0] == FREE) ? 0x0 : 0x1)
                 | ((quad[1] == FREE) ? 0x0 : 0x2)
                 | ((quad[2] == FREE) ? 0x0 : 0x4)
                 | ((quad[3] == FREE) ? 0x0 : 0x8);

  return mask;
}

void MarchingSquares::visitPoint(int x, int y, int mask, std::vector< bool >& visited, std::vector< std::pair<float2, float2> >& lines) const
{
  visited[y*width_ + x] = true;

  const bool horizontal_top = (mask == 0x7 || mask == 0x2 || mask == 0x6);
  const bool horizontal_bottom = (mask == 0x8 || mask == 0x9 || mask == 0xd);
  const bool vertical_left = (mask == 0x7 || mask == 0x4 || mask == 0x6);
  const bool vertical_right = (mask == 0x8 || mask == 0x9 || mask == 0xb);
  const bool horizontal = horizontal_top || horizontal_bottom;
  const bool vertical = vertical_left || vertical_right;

  if (!horizontal && !vertical)
    return;
//   assert(horizontal || vertical);

//   const bool start_2_lines = (mask == 0x7 || mask == 0x6 || mask == 0x8 || mask == 0x9);

  if (horizontal) {
    int2 i(x, y);
    while (true) {
      i += int2(1, 0);
      int next_mask = getMaskAt(i.x, i.y);
      if (i.x < (int)width_ && i.y < (int)height_ &&
        ( (horizontal_top && next_mask == 0x3) ||
          (horizontal_bottom && next_mask == 0xc) ||
          (vertical_left && next_mask == 0x5) ||
          (vertical_right && next_mask == 0xa) ) &&
        visited[i.y*width_ + i.x] == false)
        visited[i.y*width_ + i.x] = true;
      else
        break;
    }
    lines.push_back(std::pair<float2, float2>(float2(x, y),  float2( i.x, i.y)));
  }

  if (vertical) {
    int2 i(x, y);
    while (true) {
      i += int2(0, 1);
      int next_mask = getMaskAt(i.x, i.y);
      if (i.x < (int)width_ && i.y < (int)height_ &&
        ( (horizontal_top && next_mask == 0x3) ||
          (horizontal_bottom && next_mask == 0xc) ||
          (vertical_left && next_mask == 0x5) ||
          (vertical_right && next_mask == 0xa) ) &&
        visited[i.y*width_ + i.x] == false)
        visited[i.y*width_ + i.x] = true;
      else
        break;
    }
    lines.push_back(std::pair<float2, float2>(float2(x, y),  float2( i.x, i.y)));
  }
}
