#include "marching_squares.hpp"

#include "floats.hpp"

// #include <cstring> // for strerror needed by png++/error.hpp

#include <png++/png.hpp>


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
      // if (x == 0 || x == width_ - 1 || y == 0 || y == height_ - 1)
      //   c = SOLID;

      cells_.push_back(c);
    }
  }
}


std::vector< std::pair<float2, float2> >
MarchingSquares::RunMarchingSquares() {

  std::vector< std::pair<float2, float2> > lines;
  for (size_t y = 1; y < height_; ++y) {
    for (size_t x = 1; x < width_; ++x) {

      const CellType quad[4] = {
        cells_[(y-1) * width_ + (x-1)], cells_[(y-1) * width_ + x],   // TL T
        cells_[    y * width_ + (x-1)], cells_[    y * width_ + x] }; // R  X

      const int mask = ((quad[0] == FREE) ? 0x0 : 0x1)
                     | ((quad[1] == FREE) ? 0x0 : 0x2)
                     | ((quad[2] == FREE) ? 0x0 : 0x4)
                     | ((quad[3] == FREE) ? 0x0 : 0x8);

      const float2 points[8] = { // clockwise, starting in top-left corner
        float2(x-1,    y-1   ), // TL 0
        float2(x-0.5f, y-1   ), // T  1
        float2(x,      y-1   ), // TR 2
        float2(x,      y-0.5f), // R  3
        float2(x,      y     ), // BR 4
        float2(x-0.5f, y     ), // B  5
        float2(x-1,    y     ), // BL 6
        float2(x-1,    y-0.5f)  // L  7
      };

      // these are the marching squares cases
      int s = -1, e = -1;
      switch (mask) {
        case 0x0: /* all free, nothing to do */      break;

        case 0x1: /* TL       = TL  */ s = 7; e = 1; break;
        case 0x2: /*   TR     = TR  */ s = 1; e = 3; break;
        case 0x4: /*     BL   = BL  */ s = 5; e = 7; break;
        case 0x8: /*       BR = BR  */ s = 3; e = 5; break;

        case 0x3: /* TLTR     = top    */ s = 7; e = 3; break;
        case 0x5: /* TL  BL   = left   */ s = 1; e = 5; break;
        case 0xa: /*   TR  BR = right  */ s = 5; e = 1; break;
        case 0xc: /*     BLBR = bottom */ s = 3; e = 7; break;

        // two lines
        case 0x6: /*   TRBL   = left desc diagonal */ s = 1 | 5 << 4; e = 3 | 7 << 4; break;
        case 0x9: /* TL    BR = right asc diagonal */ s = 7 | 3 << 4; e = 1 | 5 << 4; break;

        case 0x7: /* TLTRBL   = BR free */ s = 3; e = 5; break;
        case 0xb: /* TLTR  BR = BL free */ s = 5; e = 7; break;
        case 0xd: /* TL  BLBR = TR free */ s = 1; e = 3; break;
        case 0xe: /*   TRBLBR = TL free */ s = 7; e = 1; break;

        case 0xf: /* all blocked, nothing to do */       break;
      }

      if (s == -1) {
        // assert: e == -1
      }
      else if ((s & 0xf) == s) {
        // assert: (e & 0xf) == e
        lines.push_back(std::pair<float2, float2>(points[s], points[e]));
      }
      else {
        int s1 = s & 0xf, e1 = e & 0xf;
        int s2 = s >> 4,  e2 = e >> 4;
        lines.push_back(std::pair<float2, float2>(points[s1], points[e1]));
        lines.push_back(std::pair<float2, float2>(points[s2], points[e2]));
      }
    }
  }

  // build quadtree and combine lines (there are many, many small pieces around now, combine to longer lines)

  return lines;
}
