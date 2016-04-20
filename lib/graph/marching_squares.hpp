#ifndef MARCHING_SQUARES_HPP
#define MARCHING_SQUARES_HPP

#include <vector>


struct ImageMatrix {
  enum CellType { FREE, SOLID, DESTROYABLE };

  const std::size_t width_;
  const std::size_t height_;
  const std::vector<CellType> cells_;
  ImageMatrix(std::size_t w, std::size_t h, const std::vector<CellType>& c)
    : width_(w), height_(h), cells_(c) {}
};

struct size_t2 {
  std::size_t x, y;
  constexpr size_t2 (int _x, int _y) : x(_x), y(_y) {}
  size_t2 (const size_t2& o) : x(o.x), y(o.y) {}
  void operator+=(const size_t2 o) { x += o.x; y += o.y; }
  bool operator==(const size_t2 o) const { return x == o.x && y == o.y; }
};


namespace detail {

  int getMaskAt(int x, int y, const ImageMatrix& image_matrix)
  {
    const std::size_t width = image_matrix.width_;
    const std::vector<ImageMatrix::CellType> cells = image_matrix.cells_;

    const ImageMatrix::CellType quad[4] = {
        cells[(y-1) * width + (x-1)], cells[(y-1) * width + x],   // TL T
        cells[    y * width + (x-1)], cells[    y * width + x] }; // R  X

    const int mask = ((quad[0] == ImageMatrix::FREE) ? 0x0 : 0x1)
                   | ((quad[1] == ImageMatrix::FREE) ? 0x0 : 0x2)
                   | ((quad[2] == ImageMatrix::FREE) ? 0x0 : 0x4)
                   | ((quad[3] == ImageMatrix::FREE) ? 0x0 : 0x8);

    return mask;
  }

  void visitPoint(std::size_t x, std::size_t y,
                  int mask,
                  std::vector< bool >& visited,
                  std::vector< std::pair<size_t2, size_t2> >& lines,
                  const ImageMatrix& image_matrix)
  {
    const std::size_t width = image_matrix.width_;
    const std::size_t height = image_matrix.height_;

    visited[y * width + x] = true;

    const bool horizontal_top = (mask == 0x7 || mask == 0x2 || mask == 0x6);
    const bool horizontal_bottom = (mask == 0x8 || mask == 0x9 || mask == 0xd);
    const bool vertical_left = (mask == 0x7 || mask == 0x4 || mask == 0x6);
    const bool vertical_right = (mask == 0x8 || mask == 0x9 || mask == 0xb);
    const bool horizontal = horizontal_top || horizontal_bottom;
    const bool vertical = vertical_left || vertical_right;

    if (!horizontal && !vertical)
      return;

    if (horizontal) {

      // go left till possible
      size_t2 i(x, y);
      while (true) {
        i += size_t2(1, 0);
        int next_mask = getMaskAt(i.x, i.y, image_matrix);
        if (i.x < width && i.y < height &&
          ( (horizontal_top && next_mask == 0x3) ||
            (horizontal_bottom && next_mask == 0xc) ||
            (vertical_left && next_mask == 0x5) ||
            (vertical_right && next_mask == 0xa) ) &&
          visited[i.y*width + i.x] == false)
          visited[i.y*width + i.x] = true;
        else
          break;
      }
      if (i.x != x) {
        visited[i.y*width + i.x] = false;
        lines.push_back(std::pair<size_t2, size_t2>(size_t2(x, y), size_t2( i.x, i.y)));
      }
    }

    if (vertical) {
      size_t2 i(x, y);
      while (true) {
        i += size_t2(0, 1);
        int next_mask = getMaskAt(i.x, i.y, image_matrix);
        if (i.x < width && i.y < height &&
          ( (horizontal_top && next_mask == 0x3) ||
            (horizontal_bottom && next_mask == 0xc) ||
            (vertical_left && next_mask == 0x5) ||
            (vertical_right && next_mask == 0xa) ) &&
          visited[i.y*width + i.x] == false)
          visited[i.y*width + i.x] = true;
        else
          break;
      }
      if (i.y != y) {
        visited[i.y*width + i.x] = false;
        lines.push_back(std::pair<size_t2, size_t2>(size_t2(x, y),  size_t2( i.x, i.y)));
      }
    }
  }


} // detail namespace


std::vector< std::pair<size_t2, size_t2> > marchingSquares(const ImageMatrix& image_matrix)
{
  const std::size_t width = image_matrix.width_;
  const std::size_t height = image_matrix.height_;

  std::vector<bool> visited(width * height, false);
  std::vector< std::pair<size_t2, size_t2> > lines;

  size_t2 point(1, 1);
  for (point.y = 1 ;point.y < height; ++point.y) {
    for (point.x = 1; point.x < width; ++point.x) {
      if (visited[point.y * width + point.x] == true)
        continue;

      const int mask = detail::getMaskAt(point.x, point.y, image_matrix);
      if (mask == 0x0 || mask == 0xf) // empty or full
        continue;

      detail::visitPoint(point.x, point.y, mask, visited, lines, image_matrix);
    }
  }

  return lines;
}


#endif // MARCHING_SQUARES_HPP
