#ifndef READ_PNG_TO_IMAGE_MATRIX_HPP
#define READ_PNG_TO_IMAGE_MATRIX_HPP

#include <string>
#include <vector>


struct ImageMatrix {
  enum CellType { FREE, SOLID, DESTROYABLE };

  const std::size_t width_;
  const std::size_t height_;
  const std::vector<CellType> cells_;
  ImageMatrix(std::size_t w, std::size_t h, const std::vector<CellType>& c)
    : width_(w), height_(h), cells_(c) {}
};

ImageMatrix readPngToImageMatrix(const std::string& filename); // throws std_error(filename);


#endif // READ_PNG_TO_IMAGE_MATRIX_HPP
