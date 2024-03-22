// Bounding box class


// outer bounds of a area or draw object
class BoundingBox {
public:
  int16_t x_min;
  int16_t y_min;
  int16_t x_max;
  int16_t y_max;

  /// @brief constructor of an empty box.
  BoundingBox() {
    x_min = 0;
    y_min = 0;
    x_max = -1;
    y_max = -1;
  };

  /// @brief copy constructor.
  BoundingBox(const BoundingBox &other) {
    x_min = other.x_min;
    y_min = other.y_min;
    x_max = other.x_max;
    y_max = other.y_max;
  };

  /// @brief explicit constructor.
  BoundingBox(int16_t xMin, int16_t yMin, int16_t xMax, int16_t yMax) {
    x_min = xMin;
    y_min = yMin;
    x_max = xMax;
    y_max = yMax;
  };

  /// @brief copy operator.
  BoundingBox &operator=(const BoundingBox &other) {
    x_min = other.x_min;
    y_min = other.y_min;
    x_max = other.x_max;
    y_max = other.y_max;
    return (*this);
  };

  /// @brief check if the box contains the given point
  /// @param x x-coordinate of the point
  /// @param y y-coordinate of the point
  /// @return true when point is inside.
  bool contains(int16_t x, int16_t y) {
    return ((x >= x_min) && (x <= x_max) && (y >= y_min) && (y <= y_max));
  }  // contains()


  /// @brief check if a box overlaps the this box.
  /// @param b
  /// @return
  bool overlaps(BoundingBox &b) {
    bool isOut =
      (b.x_min > this->x_max) || (b.x_max < this->x_min)
      || (b.y_min > this->y_max) || (b.y_max < this->y_min);
    return (!isOut);
  }  // overlaps()


  /// @brief check if the box overlaps the given box.
  bool overlaps(int16_t x, int16_t y, int16_t w, int16_t h) {
    return ((x_max >= x) && (x_min <= x + w - 1) && (y_max >= y) && (y_min <= y + h - 1));
  }

  bool isEmpty() {
    return ((x_max < x_min) && (y_max < y_min));
  }

  void extend(BoundingBox &other) {
    if (isEmpty()) {
      x_min = other.x_min;
      y_min = other.y_min;
      x_max = other.x_max;
      y_max = other.y_max;

    } else {
      x_min = std::min(x_min, other.x_min);
      y_min = std::min(y_min, other.y_min);
      x_max = std::max(x_max, other.x_max);
      y_max = std::max(y_max, other.y_max);
    }
  }

  // void BoundingSphere::merge(const BoundingBox &box) {};
};
