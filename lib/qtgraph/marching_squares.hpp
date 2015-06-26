#ifndef WORLD_WORLD_HPP
#define WORLD_WORLD_HPP

#include <string>
#include <vector>

class float2;

class MarchingSquares {

public:
  enum CellType { FREE, SOLID, DESTROYABLE };

  MarchingSquares();

  void ReadImage(const std::string& filename);
  std::vector< std::pair<float2, float2> > RunMarchingSquares();

private:
  int getMaskAt(int x, int y) const;
  void visitPoint(int x, int y, int mask, std::vector<bool>& visited, std::vector< std::pair<float2, float2> >& lines) const;

  std::size_t width_;
  std::size_t height_;
  std::vector<CellType> cells_;
};

#endif // WORLD_WORLD_HPP
