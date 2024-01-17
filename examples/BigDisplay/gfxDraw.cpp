// gfxDraw.cpp

#include "gfxDraw.h"


#define TRACE(...) printf(__VA_ARGS__)

#define SLOPE_UNKNOWN 0
#define SLOPE_FALLING 1
#define SLOPE_RAISING 2
#define SLOPE_HORIZONTAL 3


namespace gfxDraw {

// ===== RGBA class members =====
RGBA::RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
  : Red(r), Green(g), Blue(b), Alpha(a){};

RGBA::RGBA(uint32_t col24) {
  Red = (col24 >> 16) & 0xFF;
  Green = (col24 >> 8) & 0xFF;
  Blue = col24 & 0xFF;
  Alpha = 0xFF;
};

constexpr bool RGBA::operator==(const RGBA &col2) {
  return ((Red == col2.Red) && (Green == col2.Green) && (Blue == col2.Blue) && (Alpha == col2.Alpha));
}

constexpr bool RGBA::operator!=(const RGBA &col2) {
  return ((Red != col2.Red) || (Green != col2.Green) || (Blue != col2.Blue) || (Alpha != col2.Alpha));
}

/// @brief Convert into a 3*8 bit value using #RRGGBB.
/// @return color value.
uint32_t RGBA::toColor24() {
  return ((Red << 16) | (Green << 8) | Blue);
}


Segment::Segment(char _type, int16_t p1, int16_t p2) {
  type = _type;
  p[0] = p1;
  p[1] = p2;
};



// ===== gfxDraw namespace functions =====

void dumpColor(char *name, RGBA col) {
  printf(" %-12s: %02x.%02x.%02x.%02x %08lx\n", name, col.Alpha, col.Red, col.Green, col.Blue, col.toColor24());
}

void dumpColorTable() {
  printf("        Color: A  R  G  B  #col24\n");
  dumpColor("Red", gfxDraw::RED);
  dumpColor("Green", gfxDraw::GREEN);
  dumpColor("Blue", gfxDraw::BLUE);
  dumpColor("Orange", gfxDraw::ORANGE);
  dumpColor("Transparent", gfxDraw::TRANSPARENT);
}


/// @brief Draw a 1px line using the Bresenham's Line Generation Algorithm.
/// This function is used for returning all pixels defining the line.
/// This is a private function (not mentioned in the header) in the gfxDraw namespace.
void _line1(int16_t x0, int16_t y0, int16_t x1, int16_t y1, fSetPixel cbDraw) {
  int16_t dx = abs(x1 - x0);
  int16_t dy = abs(y1 - y0);
  int16_t sx = (x0 < x1) ? 1 : -1;
  int16_t sy = (y0 < y1) ? 1 : -1;
  int16_t err = dx - dy;

  while (true) {
    cbDraw(x0, y0);
    if ((x0 == x1) && (y0 == y1)) break;

    int16_t err2 = err << 1;

    if (err2 > -dy) {
      err -= dy;
      x0 += sx;
    }
    if (err2 < dx) {
      err += dx;
      y0 += sy;
    }
  }
};

// Draw a path (no fill).

/// @brief Scale the points of a path by factor
/// @param segments 
/// @param f100 
void scale(std::vector<Segment> &segments, int16_t f100) {
  if (f100 != 100)  {
    for (Segment &pSeg : segments) {
      switch (pSeg.type) {
        case 'M':
        case 'L':
          pSeg.p[0] = GFXSCALE100(pSeg.p[0], f100);
          pSeg.p[1] = GFXSCALE100(pSeg.p[1], f100);
          break;

        case 'c':
          printf("scale 'c' not implemented! \n");
          break;

        case 'C':
          pSeg.p[0] = GFXSCALE100(pSeg.p[0], f100);
          pSeg.p[1] = GFXSCALE100(pSeg.p[1], f100);
          pSeg.p[2] = GFXSCALE100(pSeg.p[2], f100);
          pSeg.p[3] = GFXSCALE100(pSeg.p[3], f100);
          pSeg.p[4] = GFXSCALE100(pSeg.p[4], f100);
          pSeg.p[5] = GFXSCALE100(pSeg.p[5], f100);
          break;

        case 'z':
        case 'Z':
          break;

        default:
          printf("unknown segment-%c \n", pSeg.type);
          break;
      }
    }  // for
  }
}

// void simplify(int16_t dx, int16_t dy, fSetPixel cbDraw);

// Draw a path (no fill).
void path(std::vector<Segment> &segments, int16_t dx, int16_t dy, fSetPixel cbDraw) {
  TRACE("path()\n");
  int16_t startPosX = 0;
  int16_t startPosY = 0;
  int16_t posX = 0;
  int16_t posY = 0;
  int16_t endPosX = 0;
  int16_t endPosY = 0;

  if (segments.size()) {
    for (Segment &pSeg : segments) {
      switch (pSeg.type) {
        case 'M':
          startPosX = endPosX = pSeg.x1;
          startPosY = endPosY = pSeg.y1;
          break;

        case 'L':
          endPosX = pSeg.x1;
          endPosY = pSeg.y1;
          gfxDraw::line(posX + dx, posY + dy, endPosX + dx, endPosY + dy, cbDraw);
          break;

        case 'c':
          endPosX = posX + pSeg.p[4];
          endPosY = posY + pSeg.p[5];
          gfxDraw::cubicBezier(
            dx + posX, dy + posY,
            dx + posX + pSeg.p[0], dy + posY + pSeg.p[1],
            dx + posX + pSeg.p[2], dy + posY + pSeg.p[3],
            dx + endPosX, dy + endPosY, cbDraw);
          break;

        case 'C':
          endPosX = pSeg.p[4];
          endPosY = pSeg.p[5];
          gfxDraw::cubicBezier(
            dx + posX, dy + posY,
            dx + pSeg.p[0], dy + pSeg.p[1],
            dx + pSeg.p[2], dy + pSeg.p[3],
            dx + endPosX, dy + endPosY, cbDraw);
          break;

        case 'z':
        case 'Z':
          endPosX = startPosX;
          endPosY = startPosY;
          if ((posX != endPosX) || (posY != endPosY)) {
            gfxDraw::line(posX + dx, posY + dy, endPosX + dx, endPosY + dy, cbDraw);
          }
          break;

        default:
          printf("unknown segment-%c \n", pSeg.type);
          break;
      }

      posX = endPosX;
      posY = endPosY;
    }  // for
  }
}  // _path()


/// @brief Draw a path with filling.
void fillPath(std::vector<Segment> &segments, int16_t dx, int16_t dy, fSetPixel cbBorder, fSetPixel cbFill) {
  TRACE("fillPath()\n");
  std::vector<Point> borderPoints;
  fSetPixel cbStroke = cbBorder ? cbBorder : cbFill;  // use cbFill when no cbBorder is given.

  path(segments, dx, dy,
       [&](int16_t x, int16_t y) {
         //  if (y == 210) TRACE(" p(%d/%d)\n", x, y);
         borderPoints.push_back(Point(x, y));
       });

  size_t pSize = borderPoints.size();
  TRACE("  pSize=%d\n", pSize);

  // find horizontal steps and mark with -X
  int slope;      // slop before any horizontal border points.
  int lastslope;  // slope last point.

  // find slope situation from the end of polygon.
  size_t n = pSize - 1;
  while (borderPoints[n].y == borderPoints[0].y) { n--; }
  if (borderPoints[n].y < borderPoints[0].y) {
    slope = SLOPE_RAISING;
  } else if (borderPoints[n].y > borderPoints[0].y) {
    slope = SLOPE_FALLING;
  }

  if (borderPoints[pSize - 1].y < borderPoints[0].y) {
    lastslope = SLOPE_RAISING;
  } else if (borderPoints[pSize - 1].y > borderPoints[0].y) {
    lastslope = SLOPE_FALLING;
  } else {
    lastslope = SLOPE_HORIZONTAL;
  }

  int16_t lastY = borderPoints[pSize - 1].y;

  // find and mark horizontal pixel sequences
  // except first on up-stay+-down and down-stay+-up sequences
  for (size_t n = 0; n < pSize; n++) {
    Point &p = borderPoints[n];

    if (p.y == lastY) {
      lastslope = SLOPE_HORIZONTAL;
      p.x = -p.x;  // mark as non scanline crossing.

    } else if (p.y > lastY) {
      if ((lastslope == SLOPE_HORIZONTAL) && (slope == SLOPE_FALLING)) {
        // last point was starting into other direction
        borderPoints[n - 1].x = -borderPoints[n - 1].x;  // revert marker
      }
      lastslope = slope = SLOPE_RAISING;

    } else if (p.y < lastY) {
      if ((lastslope == SLOPE_HORIZONTAL) && (slope == SLOPE_RAISING)) {
        // last point was starting into other direction
        borderPoints[n - 1].x = -borderPoints[n - 1].x;  // revert marker
      }
      lastslope = slope = SLOPE_FALLING;
    }
    lastY = p.y;
  }

  // sort points by ascending lines (y)
  std::sort(borderPoints.begin(), borderPoints.end(), Point::compare);

  int16_t y = INT16_MAX;
  int16_t x = 0;

  bool isInside = false;

  for (Point &p : borderPoints) {
    // Draw borderpoints and lines on inner segments
    int16_t nextX = abs(p.x);
    bool isTagged = (p.x < 0);

    // if (p.y == 210) TRACE(" x(%d/%d)\n", p.x, p.y);

    cbStroke(nextX, p.y);

    if (p.y != y) {
      // next scan line starts, no need to draw
      y = p.y;
      x = nextX;
      isInside = (!isTagged);

    } else {
      if ((nextX > x + 1) && (isInside)) {
        // draw the line
        while (++x < nextX) {
          cbFill(x, y);
        }
      }
      if (!isTagged)
        isInside = (!isInside);
      x = nextX;
    }
  }
};


/// @brief draw a path using a border and optional fill drawing function.
/// @param path The path definition using SVG path syntax.
/// @param x Starting Point X coordinate.
/// @param y Starting Point Y coordinate.
/// @param scale scaling factor * 100.
/// @param cbBorder Draw function for border pixels. cbFill is used when cbBorder is null.
/// @param cbFill Draw function for filling pixels.
void pathByText(const char *pathText, int16_t x, int16_t y, int16_t scale100, fSetPixel cbBorder, fSetPixel cbFill) {
  std::vector<Segment> vSeg = parsePath(pathText);
  if (scale100 != 100)
    gfxDraw::scale(vSeg, scale100);

  if (cbFill) {
    fillPath(vSeg, x, y, cbBorder, cbFill);
  } else {
    path(vSeg, x, y, cbBorder);
  }
}


/// @brief Draw a line using the most efficient algorithm
/// @param x0 Starting Point X coordinate.
/// @param y0 Starting Point Y coordinate.
/// @param x1 Ending Point X coordinate.
/// @param y1 Ending Point Y coordinate.
/// @param cbDraw Callback with coordinates of line pixels.
void line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, fSetPixel cbDraw) {
  // TRACE("Draw Line %d/%d %d/%d\n", x0, y0, x1, y1);

  int16_t dx = x1 - x0;
  int16_t dy = y1 - y0;

  // // ensure x0 <= x1
  // if (dx < 0) {  // swap line endings
  //   int16_t t;
  //   t = x0;
  //   x0 = x1;
  //   x1 = t;
  //   t = y0;
  //   y0 = y1;
  //   y1 = t;
  //   dx = x1 - x0;
  //   dy = y1 - y0;
  // }

  if (x0 == x1) {
    // fast draw vertical lines
    // TRACE("Draw-vertical\n");
    int16_t sy = (y1 > y0 ? 1 : -1);
    int16_t endY = y1 + sy;
    for (int16_t y = y0; y != endY; y += sy) {
      // TRACE(".Set %d/%d\n", x, y);
      cbDraw(x0, y);
    }

  } else if (y0 == y1) {
    // fast draw horizontal lines
    // TRACE("Draw-horizontal\n");
    int16_t sx = (x1 > x0 ? 1 : -1);
    int16_t endX = x1 + sx;
    for (int16_t x = x0; x != endX; x += sx) {
      cbDraw(x, y0);
    }

  } else {
    // any other direction
    gfxDraw::_line1(x0, y0, x1, y1, cbDraw);
  }
};


/// @brief Draw a rectangle with border and fill callbacks
void rect(int16_t x0, int16_t y0, int16_t w, int16_t h, fSetPixel cbBorder, fSetPixel cbFill) {
  if ((w != 0) && (h != 0)) {
    int16_t borderWidth = (cbFill ? 1 : 0);
    fSetPixel cbStroke = cbBorder ? cbBorder : cbFill;  // use cbFill when no cbBorder is given.

    // ensure w > 0
    if (w < 0) {
      w = -w;
      x0 = x0 - w + 1;
    }

    // ensure h > 0
    if (h < 0) {
      h = -h;
      y0 = y0 - h + 1;
    }

    int16_t endX = x0 + w - 1;
    int16_t endY = y0 + h - 1;

    // inside rect with background coloring
    if ((cbFill) && (h > 2) && (w > 2)) {
      int16_t endX = x0 + w - 1;
      int16_t endY = y0 + h - 1;
      for (int16_t y = y0 + 1; y < endY; y++)
        for (int16_t x = x0 + 1; x < endX; x++)
          cbFill(x, y);
    }

    // draw lines on the border clockwise
    if (cbStroke) {
      for (int16_t x = x0; x <= endX; x++) cbStroke(x, y0);
      for (int16_t y = y0; y <= endY; y++) cbStroke(endX, y);
      for (int16_t x = endX; x >= x0; x--) cbStroke(x, endY);
      for (int16_t y = endY; y >= y0; y--) cbStroke(x0, y);
    }
  }
}  // rect()


// This implementation of cubic bezier curve with a start and an end point given and by using 2 control points.
// C x1 y1, x2 y2, x y
// good article for reading: <https://pomax.github.io/bezierinfo/>
// Here the Casteljau's algorithm approach is used.

void cubicBezier(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, fSetPixel cbDraw) {
  TRACE("cubicBezier: %d/%d %d/%d %d/%d %d/%d\n", x0, y0, x1, y1, x2, y2, x3, y3);
  std::vector<Point> borderPoints;
  // Line 1 is x0/y0 to x1/y1, dx1/dy1 is the relative vector from x0/y0 to x1/y1
  int16_t dx1 = (x1 - x0);
  int16_t dy1 = (y1 - y0);
  int16_t dx2 = (x2 - x1);
  int16_t dy2 = (y2 - y1);
  int16_t dx3 = (x3 - x2);
  int16_t dy3 = (y3 - y2);

  // line(x0, y0, x1, y1, cbDraw);
  // line(x1, y1, x2, y2, cbDraw);
  // line(x2, y2, x3, y3, cbDraw);

  // heuristic: calc the steps we need
  uint16_t steps = abs(dx1) + abs(dy1) + abs(dx2) + abs(dy2) + abs(dx3) + abs(dy3);  // p0 - 1 - 2 - 3 - 4 - p3
  TRACE("steps:%d\n", steps);

  uint16_t skipped = 0;

  int16_t lastX = x0;
  int16_t lastY = y0;

  for (uint16_t n = 1; n <= steps; n++) {
    int16_t f = (1000 * n) / steps;
    // 3 points and 3 deltas in 1000 units
    int32_t x4 = x0 * 1000 + (f * dx1);
    int32_t y4 = y0 * 1000 + (f * dy1);
    int32_t x5 = x1 * 1000 + (f * dx2);
    int32_t y5 = y1 * 1000 + (f * dy2);
    int32_t x6 = x2 * 1000 + (f * dx3);
    int32_t y6 = y2 * 1000 + (f * dy3);
    int32_t dx5 = (x5 - x4);
    int32_t dy5 = (y5 - y4);
    int32_t dx6 = (x6 - x5);
    int32_t dy6 = (y6 - y5);

    // 2 points
    int32_t x7 = x4 + (f * dx5) / 1000;
    int32_t y7 = y4 + (f * dy5) / 1000;
    int32_t x8 = x5 + (f * dx6) / 1000;
    int32_t y8 = y5 + (f * dy6) / 1000;
    int32_t dx8 = (x8 - x7);
    int32_t dy8 = (y8 - y7);

    // 1 points
    int32_t x9 = x7 + (f * dx8) / 1000;
    int32_t y9 = y7 + (f * dy8) / 1000;

    // line(lastX, lastY, x9, y9, cbDraw);
    int16_t nextX = (x9 + 500) / 1000;
    int16_t nextY = (y9 + 500) / 1000;

    // don't collect duplicates
    if ((nextX != lastX) || (nextY != lastY)) {
      lastX = nextX;
      lastY = nextY;
      borderPoints.push_back(gfxDraw::Point(lastX, lastY));
      // cbDraw(lastX, lastY);
    } else {
      skipped++;
    }
  }
  TRACE("skipped:%d\n", skipped);



  // simplify borderPoints.
  // remove duplicates
  // remove middle point in corner steps.

  size_t pSize = borderPoints.size();
  TRACE("  pSize=%d\n", pSize);
  skipped = 0;

  for (int n = 0; n < borderPoints.size() - 2; n++) {
    bool delFlag = false;

    if ((borderPoints[n].x == borderPoints[n + 1].x) && (borderPoints[n].y == borderPoints[n + 1].y)) {
      delFlag = true;

    } else if ((borderPoints[n].y == borderPoints[n + 1].y) && (abs(borderPoints[n].x - borderPoints[n + 1].x) == 1)) {
      delFlag = (borderPoints[n + 1].x == borderPoints[n + 2].x);

    } else if ((borderPoints[n].x == borderPoints[n + 1].x) && (abs(borderPoints[n].y - borderPoints[n + 1].y) == 1)) {
      delFlag = (borderPoints[n + 1].y == borderPoints[n + 2].y);
    }

    if (delFlag) {
      borderPoints.erase(borderPoints.begin() + n + 1);
      skipped++;
    };
  }
  TRACE("removed:%d\n", skipped);

  for (Point &p : borderPoints) { cbDraw(p.x, p.y); }
};


void cubicBezier2(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, fSetPixel cbDraw) {
  printf("cubicBezier2: %d/%d %d/%d %d/%d %d/%d\n", x0, y0, x1, y1, x2, y2, x3, y3);

  int32_t f, fx, fy, leg = 1;
  int32_t sx = x0 < x3 ? 1 : -1, sy = y0 < y3 ? 1 : -1; /* step direction */
  int32_t xc = -abs(x0 + x1 - x2 - x3), xa = xc - 4 * sx * (x1 - x2), xb = sx * (x0 - x1 - x2 + x3);
  int32_t yc = -abs(y0 + y1 - y2 - y3), ya = yc - 4 * sy * (y1 - y2), yb = sy * (y0 - y1 - y2 + y3);
  int32_t ab, ac, bc, cb, xx, xy, yy, dx, dy, ex, pxy, EP = 0.01;

  /* check for curve restrains */
  /* slope P0-P1 == P2-P3    and  (P0-P3 == P1-P2      or  no slope change)  */
  if ((x1 - x0) * (x2 - x3) < EP && ((x3 - x0) * (x1 - x2) < EP || xb * xb < xa * xc + EP)) {}
  if ((y1 - y0) * (y2 - y3) < EP && ((y3 - y0) * (y1 - y2) < EP || yb * yb < ya * yc + EP)) {}

  int32_t len1 = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0) + 1; /* line lengths */
  int32_t len2 = (x2 - x3) * (x2 - x3) + (y2 - y3) * (y2 - y3) + 1;

  do { /* loop over both ends */
    ab = xa * yb - xb * ya;
    ac = xa * yc - xc * ya;
    bc = xb * yc - xc * yb;
    ex = ab * (ab + ac - 3 * bc) + ac * ac;        /* P0 part of self-intersection loop? */
    f = ex > 0 ? 1 : floor(sqrt(1 + 1024 / len1)); /* calc resolution */
    ab *= f;
    ac *= f;
    bc *= f;
    ex *= f * f; /* increase resolution */
    xy = 9 * (ab + ac + bc) / 8;
    cb = 8 * (xa - ya); /* init differences of 1st degree */
    dx = 27 * (8 * ab * (yb * yb - ya * yc) + ex * (ya + 2 * yb + yc)) / 64 - ya * ya * (xy - ya);
    dy = 27 * (8 * ab * (xb * xb - xa * xc) - ex * (xa + 2 * xb + xc)) / 64 - xa * xa * (xy + xa);
    /* init differences of 2nd degree */
    xx = 3 * (3 * ab * (3 * yb * yb - ya * ya - 2 * ya * yc) - ya * (3 * ac * (ya + yb) + ya * cb)) / 4;
    yy = 3 * (3 * ab * (3 * xb * xb - xa * xa - 2 * xa * xc) - xa * (3 * ac * (xa + xb) + xa * cb)) / 4;
    xy = xa * ya * (6 * ab + 6 * ac - 3 * bc + cb);
    ac = ya * ya;
    cb = xa * xa;
    xy = 3 * (xy + 9 * f * (cb * yb * yc - xb * xc * ac) - 18 * xb * yb * ab) / 8;

    if (ex < 0) { /* negate values if inside self-intersection loop */
      dx = -dx;
      dy = -dy;
      xx = -xx;
      yy = -yy;
      xy = -xy;
      ac = -ac;
      cb = -cb;
    } /* init differences of 3rd degree */
    ab = 6 * ya * ac;
    ac = -6 * xa * ac;
    bc = 6 * ya * cb;
    cb = -6 * xa * cb;
    dx += xy;
    ex = dx + dy;
    dy += xy; /* error of 1st step */
              // exit:
    for (pxy = 0, fx = fy = f; x0 != x3 && y0 != y3;) {
      cbDraw(x0, y0);                                  /* plot curve */
      do {                                             /* move sub-steps of one pixel */
        if ((pxy == 0) && (dx > xy || dy < xy)) break; /* confusing */
        if ((pxy == 1) && (dx > 0 || dy < 0)) break;   /* values */
        y1 = 2 * ex - dy;                              /* save value for test of y step */
        if (2 * ex >= dx) {                            /* x sub-step */
          fx--;
          ex += dx += xx;
          dy += xy += ac;
          yy += bc;
          xx += ab;
        } else if (y1 > 0)
          break;
        if (y1 <= 0) { /* y sub-step */
          fy--;
          ex += dy += yy;
          dx += xy += bc;
          xx += ac;
          yy += cb;
        }
      } while (fx > 0 && fy > 0); /* pixel complete? */
      if (2 * fx <= f) {
        x0 += sx;
        fx += f;
      } /* x step */
      if (2 * fy <= f) {
        y0 += sy;
        fy += f;
      }                                          /* y step */
      if (pxy == 0 && dx < 0 && dy > 0) pxy = 1; /* pixel ahead valid */
    }
    xx = x0;
    x0 = x3;
    x3 = xx;
    sx = -sx;
    xb = -xb; /* swap legs */
    yy = y0;
    y0 = y3;
    y3 = yy;
    sy = -sy;
    yb = -yb;
    len1 = len2;
  } while (leg--); /* try other end */

  //  ??? plotLine(x0,y0, x3,y3);       /* remaining part in case of cusp or crunode */
}

/// @brief Scan a path using the svg/path/d syntax to create a vector(array) of Segments.
/// @param pathText path definition as String
/// @return Vector with Segments.
/// @example pathText="M4 8l12-6l10 10h-8v4h-6z"
std::vector<Segment> parsePath(const char *pathText) {
  TRACE("parsePath: '%s'\n", pathText);

  char *path = (char *)pathText;
  int16_t lastX = 0, lastY = 0;

  /// A lambda function to parse a parameter from the inputText.
  auto getParam = [&]() {
    while (isblank(*path) || (*path == ',')) { path++; }
    int16_t p = strtol(path, &path, 10);
    return (p);
  };

  Segment Seg;
  std::vector<Segment> vSeg;

  while (path && *path) {
    memset(&Seg, 0, sizeof(Seg));
    int parameters = -1;

    char command = *path++;
    switch (command) {
      case ' ':
        continue;
        break;

      case 'M':
      case 'L':
        lastX = Seg.p[0] = getParam();
        lastY = Seg.p[1] = getParam();
        parameters = 0;
        break;

      case 'm':
      case 'l':
        command = toUpperCase(command);  // absolute coordinates
        lastX = Seg.p[0] = lastX + getParam();
        lastY = Seg.p[1] = lastY + getParam();
        parameters = 0;
        break;

      case 'C':
      case 'c':
        parameters = 6;
        break;

      case 'h':
        command = 'L';
        lastX = Seg.p[0] = lastX + getParam();
        Seg.p[1] = lastY;  // stay;
        parameters = 0;
        break;

      case 'v':
        command = 'L';
        Seg.p[0] = lastX;  // stay;
        lastY = Seg.p[1] = lastY + getParam();
        parameters = 0;
        break;

      case 'z':
      case 'Z':
        parameters = 0;
        break;

      default:
        printf("unknown path type: %c\n", *path);
        break;
    }

    Seg.type = command;

    // collect parameters
    for (int p = 0; p < parameters; p++) {
      // skip spaces and comma
      while (isblank(*path) || (*path == ',')) { path++; }

      Seg.p[p] = strtol(path, &path, 10);
    }

    vSeg.push_back(Seg);
  }
  // printf("  scanned: %d segments\n", vSeg.size());
  // for (Segment &seg : vSeg) {
  //   printf("  '%c' %d %d\n", seg.type, seg.p[0], seg.p[1]);
  // }
  return (vSeg);
}


// ===== DrawAlgo public class members =====





}  // gfxDraw:: namespace

// End.
