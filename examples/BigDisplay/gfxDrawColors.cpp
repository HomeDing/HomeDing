// gfxDraw.cpp

#include "gfxDrawColors.h"


#define TRACE(...)  // printf(__VA_ARGS__)

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


}  // gfxDraw:: namespace

// End.
