
// outer bounds of a area or draw object
class BoundingBox {
public:
  int16_t x_min;
  int16_t y_min;
  int16_t x_max;
  int16_t y_max;

  /// @brief check if the box contains the given point
  /// @param x x-coordinate of the point
  /// @param y y-coordinate of the point
  /// @return true when point is inside.
  bool contains(int16_t x, int16_t y) {
    return ((x >= x_min) && (x <= x_max) && (y >= y_min) && (y <= y_max));
  }


  /// @brief check if the box overlaps the given box.
  /// @param b
  /// @return
  bool overlaps(BoundingBox &b) {
    return ((x_max >= b.x_min) && (x_min <= b.x_max) && (y_max >= b.y_min) && (y_min <= b.y_max));
  }

  /// @brief check if the box overlaps the given box.
  bool overlaps(int16_t x, int16_t y, int16_t w, int16_t h) {
    return ((x_max >= x) && (x_min <= x + w - 1) && (y_max >= y) && (y_min <= y + h - 1));
  }
};
