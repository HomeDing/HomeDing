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

#include "gfxDrawColors.h"

// scaling: factors are in unit 100 (percent)

#define GFXSCALE100(p, f100) (((int32_t)(p) * f100 + 50) / 100)

#define GFXPOINT100(p) (((int32_t)(p) + 50) / 100)

namespace gfxDraw {


/// @brief Callback function definition to address a pixel on a display
typedef std::function<void(int16_t x, int16_t y)> fSetPixel;


/// @brief Callback function definition to address a pixel on a display
typedef std::function<void(int16_t x, int16_t y, RGBA color)> fDrawPixel;

/// @brief Callback function to transform all points in the segments
typedef std::function<void(int16_t &x, int16_t &y)> fTransform;

/// @brief The Segment struct holds all information about a segment of a path.
class Segment {
public:
  enum Type : uint16_t {
    Move = 0x0100 + 2,
    Line = 0x0200 + 2,
    Curve = 0x0300 + 6,
    Arc = 0x0400 + 7,
    Close = 0xFF00 + 0,
  };

  Segment() = default;
  Segment(Type _type, int16_t x = 0, int16_t y = 0);

  Type type;

  union {
    int16_t p[6];  // for parameter scanning

    struct {  // x,y for Lines, and move
      int16_t x1;
      int16_t y1;
    };

    struct {  // for Arcs
      int16_t rx;
      int16_t ry;
      int16_t rotation;
      int16_t f1f2;
      int16_t xEnd;
      int16_t yEnd;
    };

    // struct {  // for Arcs
    //   int16_t rx;
    //   int16_t ry;
    //   int16_t rotation; // of ellisis
    //   int16_t delta angle; // positive -> clockwise
    //   int16_t cx;
    //   int16_t cy;
    //   int16_t xEnd;
    //   int16_t yEnd;
    // };

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
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, fSetPixel cbDraw);

/// @brief Draw the border line of a rectangle.
void drawRect(int16_t x0, int16_t y0, int16_t w, int16_t h, fSetPixel cbDraw);
void drawSolidRect(int16_t x0, int16_t y0, int16_t w, int16_t h, fSetPixel cbDraw);

/// @brief Draw a bezier curve.
void drawCubicBezier(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, fSetPixel cbDraw);

// arc
// void drawArc(..., fSetPixel cbDraw);

/// @brief Draw the border line of a path.
void drawPath(const char *pathText, fSetPixel cbDraw);


// ===== create and manipulate segments

/// @brief Scan a path using the svg/path/d syntax to create a vector(array) of Segments.
/// @param pathText path definition as String
/// @return Vector with Segments.
/// @example pathText="M4 8l12-6l10 10h-8v4h-6z"
std::vector<Segment> parsePath(const char *pathText);

/// @brief Make a identical copy of the given segment vector.
/// @param segments
void copySegments(std::vector<Segment> &segments);


/// @brief Draw an arc using the most efficient algorithm
void drawArc(int16_t x1, int16_t y1, int16_t rx, int16_t ry, int16_t phi, int16_t flags, int16_t x2, int16_t y2, fSetPixel cbDraw);


/// @brief scale all points by the factor f100 / 100.
/// @param segments Segment vector to be changed
/// @param f100 scaling factor
void scaleSegments(std::vector<Segment> &segments, int16_t f100);

/// @brief rotate all points by the given angle.
/// @param segments Segment vector to be changed
/// @param angle angle 0...360
void rotateSegments(std::vector<Segment> &segments, int16_t angle);

/// @brief move all points by the given offset in x and y.
/// @param segments Segment vector to be changed
/// @param dx X-Offset
/// @param dy Y-Offset
void moveSegments(std::vector<Segment> &segments, int16_t dx, int16_t dy);


/// @brief Transform all points in the segments
void transformSegments(std::vector<Segment> &segments, fTransform cbTransform);


/// @brief Draw a path without filling.
/// @param segments Vector of the segments of the path.
/// @param dx Position X coordinate for the path.
/// @param dy Position Y coordinate for the path.
/// @param cbDraw Callback with coordinates of line pixels.
void drawSegments(std::vector<Segment> &segments, int16_t dx, int16_t dy, fSetPixel cbDraw);
void drawSegments(std::vector<Segment> &segments, fSetPixel cbDraw);

/// @brief Draw a path with filling.
void fillSegments(std::vector<Segment> &segments, int16_t dx, int16_t dy, fSetPixel cbBorder, fSetPixel cbFill = nullptr);
void fillSegments(std::vector<Segment> &segments, fSetPixel cbBorder, fSetPixel cbFill = nullptr);



/// @brief draw a path using a border and optional fill drawing function.
/// @param path The path definition using SVG path syntax.
/// @param x Starting Point X coordinate.
/// @param y Starting Point Y coordinate.
/// @param scale scaling factor * 100.
/// @param cbBorder Draw function for border pixels. cbFill is used when cbBorder is null.
/// @param cbFill Draw function for filling pixels.
void pathByText(const char *pathText, int16_t x, int16_t y, int16_t scale100, fSetPixel cbBorder, fSetPixel cbFill);

/// @brief draw a path using a border and optional fill drawing function.
/// @param path The path definition using SVG path syntax.
/// @param x Starting Point X coordinate.
/// @param y Starting Point Y coordinate.
/// @param cbBorder Draw function for border pixels. cbFill is used when cbBorder is null.
/// @param cbFill Draw function for filling pixels.
void pathByText100(const char *pathText, int16_t x, int16_t y, fSetPixel cbBorder, fSetPixel cbFill);


/// ====== internally used functions - maybe helpful for generating paths

/// @brief Calculate the angle of a vector in degrees
/// @param dx x value of the vector
/// @param dy y value of the vector
/// @return the angle n range 0 - 359
int16_t vectorAngle(int16_t dx, int16_t dy);

}  // gfxDraw:: namespace




// End.
