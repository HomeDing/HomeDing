// draw algorithms

// This library implements drawing polygons (and paths) on a display.

// These pixel oriented drawing functions are implemented to use callback functions for the
// effective drawing to make them independent from an specific canvas implementation and can be
// used for drawing and un-drawing.

// The functions have minimized use of float and arc arithmetics.
// Path drawing is supporting one closed path and unfolded paths only.
// The functions have minimized use of float and arc arithmetics.


// Some basic drawing algorithms are based on the efficient drawing approach of bresenham, see
// <http://members.chello.at/easyfilter/bresenham.html>.

// The basic drawing algorithms are implemented as static function in the `gfxDraw` namespace where you can also find useful color constants.
// The display coordinates are signed 16-bit integers.

// https://svg-path-visualizer.netlify.app/#M2%2C2%20Q8%2C2%208%2C8

#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <functional>
#include <vector>
#include <algorithm>

#include <cctype>
#include <cstring>
#include <cmath>

// scaling: factors are in unit 100 (percent)

#define GFXSCALE100(p, f100) (((int32_t)(p) * f100 + 50) / 100)

namespace gfxDraw {



#pragma pack(push, 1)
/// @brief The RGBA class is used to define the color and opacity of a single abstract pixel.
class RGBA {
public:
  RGBA() = default;
  RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
  RGBA(uint32_t col24);

  uint8_t Red;
  uint8_t Green;
  uint8_t Blue;
  uint8_t Alpha;

  constexpr bool operator==(const RGBA &col2);
  constexpr bool operator!=(const RGBA &col2);

  /// @brief Convert into a 3*8 bit value using #rrggbb.
  /// @return color value.
  uint32_t toColor24();
};
#pragma pack(pop)

/// @brief Callback function definition to address a pixel on a display
typedef std::function<void(int16_t x, int16_t y)> fSetPixel;


/// @brief Callback function definition to address a pixel on a display
typedef std::function<void(int16_t x, int16_t y, RGBA color)> fDrawPixel;


/// @brief The Segment struct holds all information about a segment of a path.
class Segment {
public:
  Segment() = default;
  Segment(char _type, int16_t x = 0, int16_t y = 0);

  char type;

  union {
    int16_t p[6];  // for parameter scanning

    struct {  // x,y for Lines, and move
      int16_t x1;
      int16_t y1;
    };
  };
};



/// @brief The Point holds a pixel position and provides some useful static methods.
class Point {
public:
  Point(int16_t _x, int16_t _y)
    : x(_x), y(_y){};

  /// @brief X coordinate of the Point
  int16_t x;

  /// @brief Y coordinate of the Point
  int16_t y;

  /// @brief compare function for std::sort to sort points by (y) and ascending (x)
  /// @param p1 first point
  /// @param p2 second point
  /// @return when p1 is lower than p2
  static bool compare(const Point &p1, const Point &p2) {
    if (p1.y != p2.y)
      return (p1.y < p2.y);
    return (abs(p1.x) < abs(p2.x));
  };
};

/// ===== Basic draw functions with callback =====


/// @brief Draw a line using the most efficient algorithm
/// @param x0 Starting Point X coordinate.
/// @param y0 Starting Point Y coordinate.
/// @param x1 Ending Point X coordinate.
/// @param y1 Ending Point Y coordinate.
/// @param cbDraw Callback with coordinates of line pixels.
/// @param w Width of line.
void line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, fSetPixel cbDraw);


/// @brief Draw a rectangle with border and fill color
void rect(int16_t x0, int16_t y0, int16_t w, int16_t h, fSetPixel fBorder, fSetPixel fFill = nullptr);


// bezier
void cubicBezier(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, fSetPixel cbDraw);

// arc

/// @brief Draw a path without filling.
/// @param segments Vector of the segments of the path.
/// @param dx Position X coordinate for the path.
/// @param dy Position Y coordinate for the path.
/// @param cbDraw Callback with coordinates of line pixels.
void path(std::vector<Segment> &segments, int16_t dx, int16_t dy, fSetPixel cbDraw);


/// @brief Draw a path with filling.
void fillPath(std::vector<Segment> &segments, int16_t dx, int16_t dy, fSetPixel cbBorder, fSetPixel cbFill = nullptr);

void scale(std::vector<Segment> &segments, int16_t f);


/// @brief Scan a path using the svg/path/d syntax to create a vector(array) of Segments.
/// @param pathText path definition as String
/// @return Vector with Segments.
/// @example pathText="M4 8l12-6l10 10h-8v4h-6z"
std::vector<Segment> parsePath(const char *pathText);


/// @brief draw a path using a border and optional fill drawing function.
/// @param path The path definition using SVG path syntax.
/// @param x Starting Point X coordinate.
/// @param y Starting Point Y coordinate.
/// @param scale scaling factor * 100.
/// @param cbBorder Draw function for border pixels. cbFill is used when cbBorder is null.
/// @param cbFill Draw function for filling pixels.
void pathByText(const char *pathText, int16_t x, int16_t y, int16_t scale100, fSetPixel cbBorder, fSetPixel cbFill);


}  // gfxDraw:: namespace

#include "gfxDrawColors.h"


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

  gfxDrawObject() {
    _fillMode = None;
    _stroke = gfxDraw::BLACK;
  };

  gfxDrawObject(gfxDraw::RGBA stroke, gfxDraw::RGBA fill) {
    _stroke = stroke;
    if (fill.Alpha == 0) {
      _fillMode = None;
    } else {
      _fillMode = Solid;
      _fillColor1 = fill;
    }
  };

#ifdef ARDUINO
  void setPath(String path) {
    setPath(path.c_str());
  }
#endif

  void setPath(const char *path) {
    _segments = gfxDraw::parsePath(path);
  }

  void setRect(int16_t w, int16_t h) {
    _segments.push_back(gfxDraw::Segment('M', 0, 0));
    _segments.push_back(gfxDraw::Segment('h', w));
    _segments.push_back(gfxDraw::Segment('v', h));
    _segments.push_back(gfxDraw::Segment('h', -w));
    _segments.push_back(gfxDraw::Segment('z'));
  }

  void setStrokeColor(gfxDraw::RGBA stroke) {
    _stroke = stroke;
  };

  void setFillColor(gfxDraw::RGBA fill) {
    _fillMode = Solid;
    _fillColor1 = fill;
  };

  void scale(int16_t scale100) {
    if (scale100 != 100)
      gfxDraw::scale(_segments, scale100);
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


  void draw(int16_t x, int16_t y, gfxDraw::fDrawPixel cbDraw) {
    printf("draw()\n");
    _xOffset = x;
    _yOffset = y;
    _fDraw = cbDraw;

    printf(" stroke = %02x.%02x.%02x.%02x\n", _stroke.Alpha, _stroke.Red, _stroke.Green, _stroke.Blue);
    printf(" fill   = %02x.%02x.%02x.%02x\n", _fillColor1.Alpha, _fillColor1.Red, _fillColor1.Green, _fillColor1.Blue);

    if (_fillColor1.Alpha == 0) {
      // need to draw the strike pixels only
      path(_segments, x, y,
           [&](int16_t x, int16_t y) {
             cbDraw(x, y, _stroke);
           });

    } else if ((_stroke.Alpha > 0)) {
      fillPath(
        _segments, 0, 0,
        [&](int16_t x, int16_t y) {
          cbDraw(x + _xOffset, y + _yOffset, _stroke);
        },
        [&](int16_t x, int16_t y) {
          cbDraw(x + _xOffset, y + _yOffset, _getColor(x, y));
        });


    } else if (_stroke.Alpha == 0) {
      fillPath(
        _segments, 0, 0,
        nullptr,
        [&](int16_t x, int16_t y) {
          cbDraw(x + _xOffset, y + _yOffset, _getColor(x, y));
        });
    }
  };

private:
  std::vector<gfxDraw::Segment> _segments;

  // offset fro drawing the whole gfxDrawObject
  int16_t _xOffset = 0;
  int16_t _yOffset = 0;

  // Stroke coloring
  gfxDraw::RGBA _stroke;

  // Fill coloring

  FillMode _fillMode;

  /// @brief used for solid filling and gradient start color.
  gfxDraw::RGBA _fillColor1;

  /// @brief used for gradient end color.
  gfxDraw::RGBA _fillColor2;

  gfxDraw::fDrawPixel _fDraw;

  // point of fillColor1
  int16_t _gradientX1 = 0;
  int16_t _gradientY1 = 0;

  int32_t m1000;
  int32_t b1000;

  // distance of the 2 gradient points.
  int32_t d1000;
};

// /// @brief The DrawAlgo provides all functions to draw visual elements on a pixel based canvas.
// class DrawAlgo {

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


// End.
