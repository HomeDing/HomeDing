// draw algorithms

// This library implements drawing polygons (and paths) on a display.

// These pixel oriented drawing functions are implemented to use callback functions for the effective drawing
// to make them independent from an specific canvas implementation and can be used for drawing and un-drawing.
// The functions have minimized use of float and arc arithmetics.
// The line algorithms are based on the bresenham approach <http://members.chello.at/easyfilter/bresenham.html>.
//

// Display coordinates are 16-bit integers.

// https://svg-path-visualizer.netlify.app/#M2%2C2%20Q8%2C2%208%2C8

#include <functional>
#include <vector>
#include <algorithm>

#define TRACE(...)  // printf(__VA_ARGS__)

// prototype to draw a pixel to the display memory
typedef std::function<void(int16_t x, int16_t y)> fSetPixel;


/// @brief The Segment struct holds all information about a segment of a path.
typedef struct _Segment {
  char type;

  union {
    int16_t p[2];  // for parameter scanning

    struct {  // x,y for Lines, and move
      int16_t x1;
      int16_t y1;
    };
    int16_t dist;  // d for horizontal and vertical lines
  };
} Segment;


/// @brief The DAPoint holds a pixel position and provides some useful static methods.
class DAPoint {
public:
  DAPoint(int16_t _x, int16_t _y)
    : x(_x), y(_y){};

  int16_t x;
  int16_t y;

  /// @brief compare function to sort points by (y) and ascending (x)
  /// @param p1 first point
  /// @param p2 second point
  /// @return when p2 is higher than p1
  static bool compare(const DAPoint &p1, const DAPoint &p2) {
    if (p1.y != p2.y)
      return (p1.y < p2.y);
    return (abs(p1.x) < abs(p2.x));
  };
};

#define SLOPE_UNKNOWN 0
#define SLOPE_FALLING 1
#define SLOPE_RAISING 2
#define SLOPE_HORIZONTAL 3

/// @brief The DrawAlgo provides all functions to draw visual elements on a pixel based canvas.
class DrawAlgo {

public:
  /// @brief draw a path using a border and fill drawing function.
  /// @param path The path definition using SVG path syntax.
  /// @param x Starting Point X coordinate.
  /// @param y Starting Point Y coordinate.
  /// @param cbBorder Draw function for border pixels. cbFill is used when cbBorder is null.
  /// @param cbFill Draw function for filling pixels.
  void path(const char *path, int16_t x, int16_t y, fSetPixel cbBorder, fSetPixel cbFill = nullptr) {
    std::vector<Segment> vSeg = parsePath(path);
    if (cbFill) {
      _pathFill(vSeg, x, y, cbBorder, cbFill);
    } else {
      _pathBorder(vSeg, x, y, cbBorder);
    }
  }

  /// @brief Scan a path using the svg/path/d syntax to create a vector(array) of Segments.
  /// @param pathText path definition as String
  /// @return Vector with Segments.
  /// @example pathText="M4 8l12-6l10 10h-8v4h-6z"
  std::vector<Segment> parsePath(const char *pathText) {
    char *path = (char *)pathText;
    TRACE("Seg-Path: '%s'\n", path);

    Segment Seg;
    std::vector<Segment> vSeg;

    while (path && *path) {
      int parameters = -1;

      char command = *path++;
      switch (command) {
        case ' ':
          continue;
          break;

        case 'M':
        case 'L':
        case 'l':
          parameters = 2;
          break;

        case 'h':
        case 'v':
          parameters = 1;
          break;

        case 'z':
        case 'Z':
          parameters = 0;
          break;

        default:
          printf("unknown path type: %c\n", *path);
          break;
      }

      memset(&Seg, 0, sizeof(Seg));
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


  /// @brief Draw a line using the most efficient algorithm
  /// @param x0 Starting Point X coordinate.
  /// @param y0 Starting Point Y coordinate.
  /// @param x1 Ending Point X coordinate.
  /// @param y1 Ending Point Y coordinate.
  /// @param cbDraw Callback with coordinates of line pixels.
  /// @param w Width of line.
  void line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, fSetPixel cbDraw, int16_t w = 1) {
    TRACE("Draw Line %d/%d %d/%d (w=%d)\n", x0, y0, x1, y1, w);

    int16_t dx = x1 - x0;
    int16_t dy = y1 - y0;
    w = abs(w);

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
      TRACE("Draw-vertical\n");
      int16_t startX = x0 - (w / 2);
      int16_t endX = startX + w;
      int16_t sy = (y1 > y0 ? 1 : -1);
      int16_t endY = y1 + sy;

      for (int16_t x = startX; x < endX; x++) {
        for (int16_t y = y0; y != endY; y += sy) {
          // TRACE(".Set %d/%d\n", x, y);
          cbDraw(x, y);
        }
      }

    } else if (y0 == y1) {
      // fast draw horizontal lines
      TRACE("Draw-horizontal\n");
      int16_t startY = y0 - (w / 2);
      int16_t endY = startY + w;
      int16_t sx = (x1 > x0 ? 1 : -1);
      int16_t endX = x1 + sx;

      for (int16_t y = startY; y < endY; y++) {
        for (int16_t x = x0; x != endX; x += sx) {
          cbDraw(x, y);
        }
      }

    } else if (w == 1) {
      // fast draw 1 px line
      _line1(x0, y0, x1, y1, cbDraw);

    } else {
      _lineW(x0, y0, x1, y1, cbDraw, w);
    }
  };


  // Draw a rectangle with border and fill color
  void rect(int16_t x0, int16_t y0, int16_t w, int16_t h, fSetPixel fBorderColor, fSetPixel fFillColor = nullptr, int16_t borderWidth = 1) {

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

    // inside rect with background coloring
    if ((fFillColor) && (h > borderWidth * 2) && (w > borderWidth * 2)) {
      int16_t endX = x0 + w - borderWidth;
      int16_t endY = y0 + h - borderWidth;
      for (int16_t y = y0 + borderWidth; y < endY; y++)
        for (int16_t x = x0 + borderWidth; x < endX; x++)
          fFillColor(x, y);
    }

    // draw lines on the border
    fSetPixel f = fBorderColor ? fBorderColor : fFillColor;
    if (f) {
      while ((borderWidth > 0) && (w > 0) && (h > 0)) {
        int16_t x1 = x0 + w;
        int16_t y1 = y0 + h;
        for (int16_t y = y0; y < y1; y++) {
          f(x0, y);
          f(x1 - 1, y);
        }
        for (int16_t x = x0; x < x1; x++) {
          f(x, y0);
          f(x, y1 - 1);
        }
        borderWidth -= 1;
        x0 += 1;
        y0 += 1;
        w -= 2;
        h -= 2;
      }
    }
  }


  void drawCircleIntern(int16_t xc, int16_t yc, int16_t x, int16_t y, fSetPixel fBorderColor) {
    fBorderColor(xc + x, yc + y);
    fBorderColor(xc - x, yc + y);
    fBorderColor(xc + x, yc - y);
    fBorderColor(xc - x, yc - y);
    fBorderColor(xc + y, yc + x);
    fBorderColor(xc - y, yc + x);
    fBorderColor(xc + y, yc - x);
    fBorderColor(xc - y, yc - x);
  }

  // Draw a full circle with border and fill color
  void drawCircle(int16_t xc, int16_t yc, int16_t r, fSetPixel fBorderColor, fSetPixel fFillColor = nullptr) {


    int16_t x = 0, y = r;
    int16_t d = 3 - 2 * r;
    drawCircleIntern(xc, yc, x, y, fBorderColor);
    while (y >= x) {
      x++;
      if (d > 0) {
        y--;
        d = d + 4 * (x - y) + 10;
      } else {
        d = d + 4 * x + 6;
      }
      drawCircleIntern(xc, yc, x, y, fBorderColor);
    }
  }

private:
  // use cbFill when no cbBorder is given.
  void _pathFill(std::vector<Segment> segments, int16_t dx, int16_t dy, fSetPixel cbBorder, fSetPixel cbFill = nullptr) {
    std::vector<DAPoint> borderPoints;
    fSetPixel fStroke = cbBorder ? cbBorder : cbFill;  // use cbFill when no cbBorder is given.

    _pathBorder(segments, dx, dy,
               [&](int16_t x, int16_t y) {
                 borderPoints.push_back(DAPoint(x, y));
               });

    size_t pSize = borderPoints.size();

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
      DAPoint &p = borderPoints[n];

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
    std::sort(borderPoints.begin(), borderPoints.end(), DAPoint::compare);

    int16_t y = INT16_MAX;
    int16_t x = 0;

    bool isInside = false;

    for (DAPoint &p : borderPoints) {
      // Draw borderpoints and lines on inner segments
      int16_t nextX = abs(p.x);
      bool isTagged = (p.x < 0);

      fStroke(nextX, p.y);

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
  }  // _pathFill


  void _pathBorder(std::vector<Segment> segments, int16_t dx, int16_t dy, fSetPixel cbBorder) {
    int16_t startPosX = 0;
    int16_t startPosY = 0;
    int16_t posX = 0;
    int16_t posY = 0;
    int16_t endPosX = 0;
    int16_t endPosY = 0;

    for (Segment &pSeg : segments) {
      switch (pSeg.type) {
        case 'M':
          startPosX = endPosX = pSeg.x1;
          startPosY = endPosY = pSeg.y1;
          break;

        case 'L':
          endPosX = pSeg.x1;
          endPosY = pSeg.y1;
          line(posX + dx, posY + dy, endPosX + dx, endPosY + dy, cbBorder);
          break;

        case 'l':
          endPosX = posX + pSeg.x1;
          endPosY = posY + pSeg.y1;
          line(posX + dx, posY + dy, endPosX + dx, endPosY + dy, cbBorder);
          break;

        case 'h':
          endPosX = posX + pSeg.dist;
          endPosY = posY;
          line(posX + dx, posY + dy, endPosX + dx, endPosY + dy, cbBorder);
          break;

        case 'v':
          endPosX = posX;
          endPosY = posY + pSeg.dist;
          line(posX + dx, posY + dy, endPosX + dx, endPosY + dy, cbBorder);
          break;

        case 'z':
        case 'Z':
          endPosX = startPosX;
          endPosY = startPosY;
          line(posX + dx, posY + dy, endPosX + dx, endPosY + dy, cbBorder);
          break;

        default:
          printf("unknown segment-%c \n", pSeg.type);
          break;
      }

      posX = endPosX;
      posY = endPosY;
    }  // for
  } // _pathBorder()


  // Draw a 1px line using the Bresenham function for line generation
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
  }

  // Draw a wide line using the bresenham function for line generation
  // never draw beyond the bounding box given by x0/y0 - x1/y1
  void _lineW(int16_t x0, int16_t y0, int16_t x1, int16_t y1, fSetPixel cbDraw, int16_t w = 1) {
    int16_t dx = abs(x1 - x0);
    int16_t dy = abs(y1 - y0);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err0 = dx - dy;
    int16_t err = err0;
    int16_t w2 = w / 2;

    int16_t x = x0;
    int16_t y = y0;

    while (true) {

      if (err0 < 0) {
        // steil
        for (int16_t o = 0; o < w; o++) {
          // if ((x - w2 + o >= x0) && (x - w2 + o <= x1))
          cbDraw(x - w2 + o, y);
        }
      } else {
        // flach
        for (int16_t o = 0; o < w; o++) {
          // if ((y - w2 + o >= y0) && (y - w2 + o <= y1))
          cbDraw(x, y - w2 + o);
        }
      }

      if ((x == x1) && (y == y1)) break;
      int16_t e2 = 2 * err;

      if (e2 > -dy) {
        err -= dy;
        x += sx;
      }

      if (e2 < dx) {
        err += dx;
        y += sy;
      }
    }
  }  // _lineW
};

// End.
