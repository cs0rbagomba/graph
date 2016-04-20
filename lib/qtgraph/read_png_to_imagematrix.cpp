#include "read_png_to_imagematrix.hpp"

#include <png++/png.hpp>
#include <cstring> // for strerror needed by png++/error.hpp

ImageMatrix readPngToImageMatrix(const std::string& filename)
{
  png::image<png::gray_pixel> image(filename); // throws std_error(filename);

  const std::size_t width  = image.get_width();
  const std::size_t height = image.get_height();
  std::vector<ImageMatrix::CellType> cells(width * height);

  for (std::size_t y = 0; y < height; ++y) {
    const png::image<png::gray_pixel>::row_type& row = image[y];
    for (std::size_t x = 0; x < width; ++x) {
      ImageMatrix::CellType c; // map pixel luminance to cell type
      if (row[x] < 16)        c = ImageMatrix::SOLID;       // black
      else if (row[x] >= 240) c = ImageMatrix::FREE;        // white
      else                    c = ImageMatrix::DESTROYABLE; // everything else

      cells.push_back(c);
    }
  }
  return ImageMatrix(width, height, cells);
}

