// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxdrawCircle.h: Header file for circle drawing functions
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// These pixel oriented drawing functions are implemented to use callback functions for the effective drawing
// to make them independent from an specific canvas or GFX implementation and can be used for drawing and un-drawing.
//
// The functions have minimized use of float and arc arithmetics.
// Path drawing is supporting on any given path.
// Filled paths are supported on closed paths only.
//
// Changelog: See gfxdraw.h and documentation files in this library.
//
// - - - - -

#pragma once

#include "gfxDrawCommon.h"

namespace gfxDraw {

enum ArcFlags : uint16_t {
  CounterClockwise = 0x00,
  Clockwise = 0x02,
  // ShortPath = 0x00
  LongPath = 0x10,

  FullCircle = LongPath | Clockwise
};

inline ArcFlags operator|(ArcFlags a, ArcFlags b) {
  return static_cast<ArcFlags>(static_cast<int>(a) | static_cast<int>(b));
}


/// ===== Basic draw functions with callback =====

/// @brief draw a whole circle. The draw function is not called in order of the pixels on the circle.
/// @param center center of the circle
/// @param radius radius of the circle
/// @param cbDraw SetPixel callback
void drawCircle(Point center, int16_t radius, fSetPixel cbDraw);

/// @brief draw a circle segment or a whole circle.
/// @param center center of the circle
/// @param radius radius of the circle
/// @param startPoint first point of the arc
/// @param endPoint last point of the arc
/// @param clockwise order of pixel drawing.
/// @param cbDraw SetPixel callback

void drawCircle(Point center, int16_t radius, Point startPoint, Point endPoint, ArcFlags flags, fSetPixel cbDraw);

// void drawCircleSegment(int16_t x0, int16_t y0, int16_t x1, int16_t y1, fSetPixel cbDraw);

}  // gfxDraw:: namespace




// End.
