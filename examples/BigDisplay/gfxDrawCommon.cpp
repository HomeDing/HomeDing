// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxDrawBezier.cpp: Library implementation file for bezier curve drawing functions
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Changelog: See gfxdraw.h and documentation files in this library.
//
// - - - - -

#include "gfxDraw.h"
#include "gfxDrawCommon.h"

#define TRACE(...)  // printf(__VA_ARGS__)

namespace gfxDraw {

// Proposes a next pixel of the path.

void proposePixel(int16_t x, int16_t y, fSetPixel cbDraw) {
  static Point lastPoints[3];

  TRACE("proposePixel(%d, %d)\n", x, y);

  if ((x == lastPoints[0].x) && (y == lastPoints[0].y)) {
    // don't collect duplicates
    TRACE("  duplicate!\n");

  } else if (y == POINT_BREAK_Y) {
    // draw all remaining points and invalidate lastPoints
    TRACE("  flush!\n");
    for (int n = 2; n >= 0; n--) {
      if (lastPoints[n].y != POINT_INVALID_Y)
        cbDraw(lastPoints[n].x, lastPoints[n].y);
      lastPoints[n].y = POINT_INVALID_Y;
    }  // for

  } else if (lastPoints[0].y == POINT_BREAK_Y) {
    lastPoints[0].x = x;
    lastPoints[0].y = y;

  } else {
    // draw oldest point and shift new point in
    if (lastPoints[2].y != POINT_INVALID_Y)
      cbDraw(lastPoints[2].x, lastPoints[2].y);
    lastPoints[2] = lastPoints[1];
    lastPoints[1] = lastPoints[0];
    lastPoints[0].x = x;
    lastPoints[0].y = y;

    if (lastPoints[1].y != POINT_INVALID_Y) {
      bool delFlag = false;
      bool insFlag = false;

      // don't draw "corner" points
      if ((lastPoints[0].y == lastPoints[1].y) && (abs(lastPoints[0].x - lastPoints[1].x) == 1)) {
        delFlag = (lastPoints[1].x == lastPoints[2].x);
      } else if ((lastPoints[0].x == lastPoints[1].x) && (abs(lastPoints[0].y - lastPoints[1].y) == 1)) {
        delFlag = (lastPoints[1].y == lastPoints[2].y);
      }

      // draw between unconnected points
      if (!delFlag) {
        if ((abs(lastPoints[0].x - lastPoints[1].x) <= 1) && (abs(lastPoints[0].y - lastPoints[1].y) <= 1)) {
          // points are connected -> no additional draw required

        } else if ((abs(lastPoints[0].x - lastPoints[1].x) <= 2) && (abs(lastPoints[0].y - lastPoints[1].y) <= 2)) {
          // simple interpolate new lastPoints[1]
          // TRACE("  gap!\n");
          if (lastPoints[2].y != POINT_INVALID_Y)
            cbDraw(lastPoints[2].x, lastPoints[2].y);
          lastPoints[2] = lastPoints[1];
          lastPoints[1].x = (lastPoints[0].x + lastPoints[1].x) / 2;
          lastPoints[1].y = (lastPoints[0].y + lastPoints[1].y) / 2;
        } else if ((abs(lastPoints[0].x - lastPoints[1].x) > 2) || (abs(lastPoints[0].y - lastPoints[1].y) > 2)) {
          // TRACE("  big gap!\n");

          // draw a streight line from lastPoints[1] to lastPoints[0]
          if (lastPoints[2].y != POINT_INVALID_Y)
            cbDraw(lastPoints[2].x, lastPoints[2].y);
          drawLine(lastPoints[1].x, lastPoints[1].y, lastPoints[0].x, lastPoints[0].y, cbDraw);
          lastPoints[2].y = POINT_INVALID_Y;
          lastPoints[1].y = POINT_INVALID_Y;
          // lastPoints[0] stays.
        }
      }

      if (delFlag) {
        // remove lastPoints[1];
        lastPoints[1] = lastPoints[2];
        lastPoints[2].y = POINT_INVALID_Y;
      }
    }
  }

};


}  // gfxDraw:: namespace

// End.
