#ifndef WORLD_WORLD_HPP
#define WORLD_WORLD_HPP

#include <string>
#include <vector>

class float2;

class MarchingSquares {
  enum CellType { FREE, SOLID, DESTROYABLE };

public:
  MarchingSquares();

  void ReadImage(const std::string& filename);
  std::vector< std::pair<float2, float2> > RunMarchingSquares();

private:

  std::size_t width_;
  std::size_t height_;
  std::vector<CellType> cells_;
};

#endif // WORLD_WORLD_HPP
