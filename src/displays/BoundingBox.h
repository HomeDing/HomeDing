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
  /// ensure that x_min <= x_max
  BoundingBox(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    if (x0 <= x1) {
      x_min = x0;
      x_max = x1;
    } else {
      x_min = x1;
      x_max = x0;
    }

    if (y0 <= y1) {
      y_min = y0;
      y_max = y1;
    } else {
      y_min = y1;
      y_max = y0;
    }
  };

  /// @brief calculate the width of the box.
  /// @return the width of the box.
  inline int16_t width() {
    return (x_max - x_min + 1);
  };

  /// @brief calculate the height of the box.
  /// @return the height of the box.
  int16_t height() {
    return (y_max - y_min + 1);
  };

  /// @brief calculate the max quadratic size of the box treating width and height with 0 as uninitialized.
  /// @return the max. quadrat inside the box.
  int16_t qsize() {
    int16_t size =  (x_max - x_min + 1);
    int16_t h =  (y_max - y_min + 1);
    if ((h > 1) && (h < size)) {
      size = h;
    }
    return (size);  
  };


  /// @brief copy operator.
  BoundingBox &operator=(const BoundingBox &other) {
    x_min = other.x_min;
    y_min = other.y_min;
    x_max = other.x_max;
    y_max = other.y_max;
    return (*this);
  };


  /// @brief shift the bounding box by a vector
  /// @param dx x dimension of the vector
  /// @param dx y dimension of the vector

  void shift(int16_t dx, int16_t dy) {
    x_min += dx;
    x_max += dx;
    y_min += dy;
    y_max += dy;
  }  // shift()


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
    bool isOut = (b.x_min > this->x_max) || (b.x_max < this->x_min) || (b.y_min > this->y_max) || (b.y_max < this->y_min);
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
