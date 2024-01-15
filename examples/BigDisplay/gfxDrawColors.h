// Pre-defined drawing Colors

#pragma once

#include "gfxDraw.h"

namespace gfxDraw {

// Some useful constants for simple colors

// clang-format off
const RGBA BLACK (    0,    0,    0);
const RGBA SILVER( 0xCC, 0xCC, 0xCC);
const RGBA RED   ( 0xFF,    0,    0);
const RGBA ORANGE( 0xE9, 0x76,    0);
const RGBA YELLOW( 0xF6, 0xC7,    0);
const RGBA GREEN (    0, 0xFF,    0);
const RGBA BLUE  (    0,    0, 0xFF);
const RGBA CYAN  (    0, 0xFF, 0xFF);
const RGBA PURPLE( 0x99, 0x46, 0x80);
const RGBA WHITE ( 0xFF, 0xFF, 0xFF);

const RGBA TRANSPARENT( 0x00, 0x00, 0x00, 0x00);
// c lang-format on

void dumpColorTable();

}  // namespace gfxDraw

// End.
