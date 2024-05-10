// GFXDrawObject.h

// This gfxDrawObject class implements the necessary steps to draw a object based on a path with transformation and
// coloring options to simplify drawing with gfxDraw native functions.
//
//  * All transformations are combined into a transformation matrix to avoid intermediate transformations with rounding
//    effects.
//  * The fill color can be specified using simple linear gradients.
//  * The Transformations can be 
//  * Example of a gauge based on gfxDrawObject that manipulates segments based on a given value.

// https://svg-path-visualizer.netlify.app/#M2%2C2%20Q8%2C2%208%2C8

#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <functional>
#include <vector>
#include <algorithm>

#include <cctype>
// #include <cmath>

#include "gfxDraw.h"
#include "gfxDrawColors.h"

// scaling: factors are in unit 100 (percent)

#define GFXSCALE100(p, f100) (((int32_t)(p) * f100 + 50) / 100)
#define GFXSCALE1000(p, f1000) (((int32_t)(p) * f1000 + 500) / 1000)

namespace gfxDraw {

// Matrix type definition for transformation using 1000 factor and numbers.
typedef int32_t Matrix1000[3][3];


/// @brief A gfxDrawObject is used to define a stroke path and coloring of a graphical object.
/// It supports creating paths by simple functions like lines and rectangles or by using the svg path notation (not fully supported)
/// The stroke color can be set.
/// The fill color can be set to solid or gradient functions.
/// TODO: collect the current colors from the canvas so the object can be "undrawn" later.

class gfxDrawObject {
public:
  enum FillMode { None,
                  Solid,
                  Horizontal,
                  Vertical,
                  Linear };

  // constructor functions

  gfxDrawObject() {
    _fillMode = None;
    _stroke = gfxDraw::BLACK;
    _initMatrix(_matrix);
  };

  gfxDrawObject(gfxDraw::RGBA stroke, gfxDraw::RGBA fill)
    : gfxDrawObject() {

    _stroke = stroke;
    if (fill.Alpha != 0) {
      _fillMode = Solid;
      _fillColor1 = fill;
    }
  };

  gfxDrawObject(const char *pathText, gfxDraw::RGBA stroke, gfxDraw::RGBA fill)
    : gfxDrawObject(stroke, fill) {
    setPath(pathText);
  };


  // ===== path creation functions =====

  // save the path by converting to a segment vector.
  void setPath(const char *path) {
    _segments = gfxDraw::parsePath(path);
  }

  void setRect(int16_t w, int16_t h) {
    _segments.push_back(gfxDraw::Segment(gfxDraw::Segment::Move, 0, 0));
    _segments.push_back(gfxDraw::Segment(gfxDraw::Segment::Line, 0, h - 1));
    _segments.push_back(gfxDraw::Segment(gfxDraw::Segment::Line, w - 1, h - 1));
    _segments.push_back(gfxDraw::Segment(gfxDraw::Segment::Line, w - 1, 0));
    _segments.push_back(gfxDraw::Segment(gfxDraw::Segment::Close));
  }


  // ===== coloring functions =====

  void setStrokeColor(gfxDraw::RGBA stroke) {
    _stroke = stroke;
  };

  void setFillColor(gfxDraw::RGBA fill) {
    _fillMode = Solid;
    _fillColor1 = fill;
  };


  // ===== transformation functions =====

  // the transformation is recorded to a 3*3 matrix and applied just before drawing.

  void resetTransformation() {
    _initMatrix(_matrix);
  }

  // apply the scaling factors to the transformation matrix;
  void move(int16_t dx, int16_t dy) {
    if ((dx != 0) || (dy != 0)) {
      Matrix1000 MoveMatrix;
      _initMatrix(MoveMatrix);
      MoveMatrix[0][2] = dx * 1000;
      MoveMatrix[1][2] = dy * 1000;
      _multiplyMatrix(_matrix, MoveMatrix);
    }
  };

  // apply the scaling factors to the transformation matrix;
  void scale(int16_t scale100) {
    if (scale100 != 100) {
      Matrix1000 scaleMatrix;
      _initMatrix(scaleMatrix);
      scaleMatrix[0][0] = scaleMatrix[1][1] = scale100 * 10;
      _multiplyMatrix(_matrix, scaleMatrix);
    }
  };


  // apply the scaling factors to the transformation matrix;
  void rotate(int16_t angle) {
    if (angle != 0) {
      Matrix1000 rotMatrix;
      _initMatrix(rotMatrix);
      // rotMatrix[0][0] = rotMatrix[1][1] = scale100 * 10;
      _multiplyMatrix(_matrix, rotMatrix);
    }
  };


  void draw(gfxDraw::fDrawPixel cbDraw) {
    printf("draw()\n");

    printf(" stroke = %02x.%02x.%02x.%02x\n", _stroke.Alpha, _stroke.Red, _stroke.Green, _stroke.Blue);
    printf(" fill   = %02x.%02x.%02x.%02x\n", _fillColor1.Alpha, _fillColor1.Red, _fillColor1.Green, _fillColor1.Blue);

    // create a copy
    std::vector<gfxDraw::Segment> tSegments = _segments;

    // transform with matrix
    transformSegments(tSegments, [&](int16_t &x, int16_t &y) {
      int32_t tx, ty;
      tx = x * _matrix[0][0] + y * _matrix[0][1] + _matrix[0][2];
      ty = x * _matrix[1][0] + y * _matrix[1][1] + _matrix[1][2];

      x = tx / 1000;
      y = ty / 1000;
    });

    // draw...
    if (_fillColor1.Alpha == 0) {
      // need to draw the strike pixels only
      gfxDraw::drawSegments(tSegments, [&](int16_t x, int16_t y) {
        cbDraw(x, y, _stroke);
      });

    } else if ((_stroke.Alpha > 0)) {
      gfxDraw::fillSegments(
        tSegments, [&](int16_t x, int16_t y) {
          cbDraw(x, y, _stroke);
        },
        [&](int16_t x, int16_t y) {
          cbDraw(x, y, _getColor(x, y));
        });

    } else if (_stroke.Alpha == 0) {
      gfxDraw::fillSegments(
        tSegments, nullptr,
        [&](int16_t x, int16_t y) {
          cbDraw(x, y, _getColor(x, y));
        });
    }
  };



  void setFillGradient(gfxDraw::RGBA fill1, int16_t x1, int16_t y1, gfxDraw::RGBA fill2, int16_t x2, int16_t y2) {
    printf("setFillGradient(#%08lx %d/%d #%08lx %d/%d )\n", fill1.toColor24(), x1, y1, fill2.toColor24(), x2, y2);

    _fillColor1 = fill1;
    _fillColor2 = fill2;
    _gradientX1 = x1;
    _gradientY1 = y1;

    if (y1 == y2) {
      _fillMode = Horizontal;
      d1000 = (x2 - x1) * 1000;

    } else if (x1 == x2) {
      _fillMode = Vertical;
      d1000 = (y2 - y1) * 1000;

    } else {
      _fillMode = Linear;

      // pre-calculate all factors for dist calculation in linear Gradient color function.
      // instead of using floats use the factor 1000.
      // Line formula: `y = mx + b` for the line where fill1 color will be used;
      // This is the orthogonal line through p1 (slope = 1/m)
      int16_t dx = (x2 - x1);
      int16_t dy = (y2 - y1);
      printf(" dx=%d, dy=%d\n", dx, dy);

      m1000 = -dx * 1000 / dy;  // = -2000
      printf(" m1000=%d\n", m1000);
      b1000 = (y1 * 1000) - (m1000 * x1);  // 6000 - ( -2000 * 4)
      printf(" b1000=%d\n", b1000);

      // distance of the 2 gradient points.
      d1000 = sqrt(dx * dx + dy * dy) * 1000;

      // Distance formula: `d = (mx - y + b) / sqrt(m*m + 1)`
      // divisor for the distance formula
      int16_t nen1000 = sqrt(m1000 * m1000 + 1000000);
      printf(" nen1000=%d\n", nen1000);

      // `y = (m1000 * x + b1000) / 1000`

      // distance between the 2 points:
      // int32_t d = sqrt((x2 - x1) * (y2 - y1));

      // distance of point x/y from this line:
      // d = (mx - y + b) /  sqrt(m*m + 1) // (-2 * x - y + 14) / sqrt(4 +1)
      // (-2 * 10 - 9 + 14) / sqrt(2*2 +1)

      // sqrt(m*m + 1) = sqrt(m1000*m1000 + 1*1000*1000) / 1000
      // can be simplified with a small error by just
      // = sqrt(m1000 / 1000 * m1000 / 1000 + 1)
      // = sqrt(m1000 / 1000 * m1000 / 1000 ) = m/1000

      // d = (m1000 x - y * 1000 + b1000) /  sqrt(m*m + 1)
      // (-2000 * 10 - 9 * 1000 + 14000) / sqrt(m1000*m1000 / 1000 / 1000 + 1*1000*1000)
      // (-2000 * 9 - 5 * 1000 + 14000) / sqrt(m1000*m1000 / 1000 / 1000 + 1*1000*1000)

      int16_t px = 5;
      int16_t py = 9;
      int16_t pd = 1000 * ((1000 * py - m1000 * px - b1000) + 500) / nen1000;
      printf(" pd=%d\n", pd);
    };
    printf(" d1000=%d\n", d1000);
  };



  /// @brief calculate color of coordinate x/y
  /// @param x
  /// @param y
  /// @return
  gfxDraw::RGBA _getColor(int16_t x, int16_t y) {
    if (_fillMode == Solid) {
      return (_fillColor1);

    } else if (_fillMode == Horizontal) {
      int32_t dp1000 = (x - _gradientX1) * 1000;
      int32_t f100 = (dp1000 * 100) / d1000;

      if (f100 <= 0) {
        return (_fillColor1);

      } else if (f100 >= 100) {
        return (_fillColor2);

      } else {
        int32_t q100 = (100 - f100);
        gfxDraw::RGBA col = gfxDraw::RGBA(
          (q100 * _fillColor1.Red + f100 * _fillColor2.Red) / 100,
          (q100 * _fillColor1.Green + f100 * _fillColor2.Green) / 100,
          (q100 * _fillColor1.Blue + f100 * _fillColor2.Blue) / 100,
          (q100 * _fillColor1.Alpha + f100 * _fillColor2.Alpha) / 100);
        return (col);
      };

    } else if (_fillMode == Vertical) {
      int32_t dp1000 = (y - _gradientY1) * 1000;
      int32_t f100 = (dp1000 * 100) / d1000;

      if (f100 <= 0) {
        return (_fillColor1);

      } else if (f100 >= 100) {
        return (_fillColor2);

      } else {
        int32_t q100 = (100 - f100);
        gfxDraw::RGBA col = gfxDraw::RGBA(
          (q100 * _fillColor1.Red + f100 * _fillColor2.Red) / 100,
          (q100 * _fillColor1.Green + f100 * _fillColor2.Green) / 100,
          (q100 * _fillColor1.Blue + f100 * _fillColor2.Blue) / 100,
          (q100 * _fillColor1.Alpha + f100 * _fillColor2.Alpha) / 100);
        return (col);
      };
    } else {
      // not implemented yet
      return (gfxDraw::PURPLE);
    }
  }

protected:
  std::vector<gfxDraw::Segment> _segments;

private:
  // combined transformation matrix
  Matrix1000 _matrix;


  void _initMatrix(Matrix1000 &m) {
    memset(&m, 0, sizeof(m));
    m[0][0] = 1000;
    m[1][1] = 1000;
    m[2][2] = 1000;
  }

  void _multiplyMatrix(Matrix1000 &m1, Matrix1000 &m2) {
    Matrix1000 r;
    for (int x = 0; x < 3; x++) {
      for (int y = 0; y < 3; y++) {
        r[x][y] = 0;
        for (int k = 0; k < 3; k++) {
          r[x][y] += (m2[x][k] * m1[k][y]);
        }
        r[x][y] /= 1000;
      }
    }
    memcpy(&m1, &r, sizeof(Matrix1000));
  };


  // Stroke coloring
  gfxDraw::RGBA _stroke;

  // Fill coloring

  FillMode _fillMode;

  /// @brief used for solid filling and gradient start color.
  gfxDraw::RGBA _fillColor1;

  /// @brief used for gradient end color.
  gfxDraw::RGBA _fillColor2;

  // point of fillColor1
  int16_t _gradientX1 = 0;
  int16_t _gradientY1 = 0;

  int32_t m1000;
  int32_t b1000;

  // distance of the 2 gradient points.
  int32_t d1000;
};

// public:

// void drawCircleIntern(int16_t xc, int16_t yc, int16_t x, int16_t y, fSetPixel fBorderColor) {
//   fBorderColor(xc + x, yc + y);
//   fBorderColor(xc - x, yc + y);
//   fBorderColor(xc + x, yc - y);
//   fBorderColor(xc - x, yc - y);
//   fBorderColor(xc + y, yc + x);
//   fBorderColor(xc - y, yc + x);
//   fBorderColor(xc + y, yc - x);
//   fBorderColor(xc - y, yc - x);
// }

// // Draw a full circle with border and fill color
// void drawCircle(int16_t xc, int16_t yc, int16_t r, fSetPixel fBorderColor, fSetPixel fFillColor = nullptr) {


//   int16_t x = 0, y = r;
//   int16_t d = 3 - 2 * r;
//   drawCircleIntern(xc, yc, x, y, fBorderColor);
//   while (y >= x) {
//     x++;
//     if (d > 0) {
//       y--;
//       d = d + 4 * (x - y) + 10;
//     } else {
//       d = d + 4 * x + 6;
//     }
//     drawCircleIntern(xc, yc, x, y, fBorderColor);
//   }
// }

}

// End.
