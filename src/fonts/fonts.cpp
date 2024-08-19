// fonts.cpp

#include <Arduino.h>
#include <gfxfont.h>

// static storage for fonts

// ===== 10 pixel height

const uint8_t Font_10Bitmaps[]  = {
  0xFA, 0xFA, 0xB6, 0x80, 0x57, 0xD5, 0xF5, 0x00, 0x23, 0xA3, 0x07, 0x06, 0x2E, 0x20, 0xC6, 0x44, 0x44, 0x4C, 0x60, 0x64, 0xA4, 0xCA, 0xC9, 0xA0,
  0xE0, 0x6A, 0xAA, 0x40, 0x95, 0x55, 0x80, 0x55, 0x00, 0x21, 0x3E, 0x42, 0x00, 0xF6, 0xF8, 0xF0, 0x24, 0xA4, 0xA4, 0x80, 0x74, 0x63, 0x58, 0xC5,
  0xC0, 0x2E, 0x92, 0x48, 0x00, 0x74, 0x62, 0x22, 0x23, 0xE0, 0xF8, 0x88, 0xE0, 0xC5, 0xC0, 0x12, 0x95, 0x2F, 0x88, 0x40, 0xFC, 0x21, 0xE0, 0x87,
  0xC0, 0x74, 0x61, 0x6C, 0xC5, 0xC0, 0xF8, 0x44, 0x22, 0x10, 0x80, 0x74, 0x62, 0xE8, 0xC5, 0xC0, 0x74, 0x62, 0xF0, 0xC5, 0xC0, 0xF0, 0xF0, 0xF0,
  0xF6, 0x1B, 0x20, 0xC1, 0x80, 0xF8, 0x3E, 0xC1, 0x82, 0x6C, 0x00, 0x74, 0x62, 0x22, 0x00, 0x80, 0x74, 0x67, 0x59, 0xC1, 0xC0, 0x22, 0x95, 0x1F,
  0xC6, 0x20, 0xF4, 0x63, 0xE8, 0xC7, 0xC0, 0x74, 0x61, 0x08, 0x45, 0xC0, 0xF4, 0x63, 0x18, 0xC7, 0xC0, 0xFC, 0x21, 0xE8, 0x43, 0xE0, 0xFC, 0x21,
  0xE8, 0x42, 0x00, 0x74, 0x61, 0x38, 0xC5, 0xE0, 0x8C, 0x63, 0xF8, 0xC6, 0x20, 0xFE, 0xF8, 0x42, 0x10, 0xC5, 0xC0, 0x8C, 0x65, 0xC9, 0x46, 0x20,
  0x84, 0x21, 0x08, 0x43, 0xE0, 0x8E, 0xF7, 0x5A, 0xC6, 0x20, 0x8E, 0x73, 0x5A, 0xCE, 0x60, 0x74, 0x63, 0x18, 0xC5, 0xC0, 0xF4, 0x63, 0xE8, 0x42,
  0x00, 0x74, 0x63, 0x1A, 0xC9, 0xA0, 0xF4, 0x63, 0xEA, 0x4A, 0x20, 0x74, 0x60, 0xE0, 0xC5, 0xC0, 0xF9, 0x08, 0x42, 0x10, 0x80, 0x8C, 0x63, 0x18,
  0xC5, 0xC0, 0x8C, 0x63, 0x15, 0x28, 0x80, 0x8C, 0x6B, 0x5A, 0xA9, 0x40, 0x8C, 0x54, 0x45, 0x46, 0x20, 0x8C, 0x54, 0x42, 0x10, 0x80, 0xF8, 0x44,
  0x44, 0x43, 0xE0, 0xEA, 0xAA, 0xC0, 0x92, 0x24, 0x89, 0x20, 0xD5, 0x55, 0xC0, 0x22, 0xA2, 0x00, 0xF8, 0x88, 0x80, 0x70, 0x5F, 0x17, 0x80, 0x84,
  0x2D, 0x98, 0xC7, 0xC0, 0x7C, 0x21, 0x07, 0x80, 0x08, 0x5B, 0x38, 0xC5, 0xE0, 0x74, 0x7F, 0x07, 0x00, 0x34, 0x4F, 0x44, 0x40, 0x6C, 0xE3, 0x17,
  0x85, 0xC0, 0x84, 0x2D, 0x98, 0xC6, 0x20, 0xBE, 0x45, 0x55, 0x80, 0x88, 0x9A, 0xCA, 0x90, 0xAA, 0xA4, 0xD5, 0x6B, 0x5A, 0x80, 0xB6, 0x63, 0x18,
  0x80, 0x74, 0x63, 0x17, 0x00, 0xB6, 0x63, 0x1F, 0x42, 0x00, 0x6C, 0xE3, 0x17, 0x84, 0x20, 0xBC, 0x88, 0x80, 0x7C, 0x1C, 0x1F, 0x00, 0x44, 0xF4,
  0x44, 0x30, 0x8C, 0x63, 0x36, 0x80, 0x8C, 0x62, 0xA2, 0x00, 0x8D, 0x6A, 0xA5, 0x00, 0x8A, 0x88, 0xA8, 0x80, 0x8C, 0x62, 0xA2, 0x22, 0x00, 0xF8,
  0x88, 0x8F, 0x80, 0x29, 0x28, 0x92, 0x20, 0xFE, 0x89, 0x22, 0x92, 0x80, 0x6D, 0x80
};

const GFXglyph Font_10Glyphs[]  = {
  { 0, 0, 0, 4, 0, 1 },     // 0x20 ' '
  { 1, 1, 7, 2, 0, -6 },    // 0x21 '!'
  { 2, 3, 3, 4, 0, -7 },    // 0x22 '"'
  { 4, 5, 5, 6, 0, -5 },    // 0x23 '#'
  { 8, 5, 9, 6, 0, -7 },    // 0x24 '$'
  { 14, 5, 7, 6, 0, -6 },   // 0x25 '%'
  { 19, 5, 7, 6, 0, -6 },   // 0x26 '&'
  { 24, 1, 3, 2, 0, -7 },   // 0x27 '''
  { 25, 2, 9, 4, 1, -7 },   // 0x28 '('
  { 28, 2, 9, 4, 0, -7 },   // 0x29 ')'
  { 31, 3, 3, 5, 1, -7 },   // 0x2A '*'
  { 33, 5, 5, 6, 0, -5 },   // 0x2B '+'
  { 37, 2, 4, 4, 0, -1 },   // 0x2C ','
  { 38, 5, 1, 6, 0, -3 },   // 0x2D '-'
  { 39, 2, 2, 4, 0, -1 },   // 0x2E '.'
  { 40, 3, 9, 5, 1, -7 },   // 0x2F '/'
  { 44, 5, 7, 6, 0, -6 },   // 0x30 '0'
  { 49, 3, 7, 6, 1, -6 },   // 0x31 '1'
  { 53, 5, 7, 6, 0, -6 },   // 0x32 '2'
  { 58, 5, 7, 6, 0, -6 },   // 0x33 '3'
  { 63, 5, 7, 6, 0, -6 },   // 0x34 '4'
  { 68, 5, 7, 6, 0, -6 },   // 0x35 '5'
  { 73, 5, 7, 6, 0, -6 },   // 0x36 '6'
  { 78, 5, 7, 6, 0, -6 },   // 0x37 '7'
  { 83, 5, 7, 6, 0, -6 },   // 0x38 '8'
  { 88, 5, 7, 6, 0, -6 },   // 0x39 '9'
  { 93, 2, 6, 4, 1, -5 },   // 0x3A ':'
  { 95, 2, 8, 4, 1, -5 },   // 0x3B ';'
  { 97, 5, 5, 6, 0, -5 },   // 0x3C '<'
  { 101, 5, 3, 6, 0, -4 },  // 0x3D '='
  { 103, 5, 5, 6, 0, -5 },  // 0x3E '>'
  { 107, 5, 7, 6, 0, -6 },  // 0x3F '?'
  { 112, 5, 7, 6, 0, -6 },  // 0x40 '@'
  { 117, 5, 7, 6, 0, -6 },  // 0x41 'A'
  { 122, 5, 7, 6, 0, -6 },  // 0x42 'B'
  { 127, 5, 7, 6, 0, -6 },  // 0x43 'C'
  { 132, 5, 7, 6, 0, -6 },  // 0x44 'D'
  { 137, 5, 7, 6, 0, -6 },  // 0x45 'E'
  { 142, 5, 7, 6, 0, -6 },  // 0x46 'F'
  { 147, 5, 7, 6, 0, -6 },  // 0x47 'G'
  { 152, 5, 7, 6, 0, -6 },  // 0x48 'H'
  { 157, 1, 7, 3, 1, -6 },  // 0x49 'I'
  { 158, 5, 7, 6, 0, -6 },  // 0x4A 'J'
  { 163, 5, 7, 6, 0, -6 },  // 0x4B 'K'
  { 168, 5, 7, 6, 0, -6 },  // 0x4C 'L'
  { 173, 5, 7, 6, 0, -6 },  // 0x4D 'M'
  { 178, 5, 7, 6, 0, -6 },  // 0x4E 'N'
  { 183, 5, 7, 6, 0, -6 },  // 0x4F 'O'
  { 188, 5, 7, 6, 0, -6 },  // 0x50 'P'
  { 193, 5, 7, 6, 0, -6 },  // 0x51 'Q'
  { 198, 5, 7, 6, 0, -6 },  // 0x52 'R'
  { 203, 5, 7, 6, 0, -6 },  // 0x53 'S'
  { 208, 5, 7, 6, 0, -6 },  // 0x54 'T'
  { 213, 5, 7, 6, 0, -6 },  // 0x55 'U'
  { 218, 5, 7, 6, 0, -6 },  // 0x56 'V'
  { 223, 5, 7, 6, 0, -6 },  // 0x57 'W'
  { 228, 5, 7, 6, 0, -6 },  // 0x58 'X'
  { 233, 5, 7, 6, 0, -6 },  // 0x59 'Y'
  { 238, 5, 7, 6, 0, -6 },  // 0x5A 'Z'
  { 243, 2, 9, 4, 1, -7 },  // 0x5B '['
  { 246, 3, 9, 5, 1, -7 },  // 0x5C '\'
  { 250, 2, 9, 4, 1, -7 },  // 0x5D ']'
  { 253, 5, 3, 6, 0, -7 },  // 0x5E '^'
  { 256, 5, 1, 6, 0, 1 },   // 0x5F '_'
  { 257, 3, 3, 5, 1, -7 },  // 0x60 '`'
  { 259, 5, 5, 6, 0, -4 },  // 0x61 'a'
  { 263, 5, 7, 6, 0, -6 },  // 0x62 'b'
  { 268, 5, 5, 6, 0, -4 },  // 0x63 'c'
  { 272, 5, 7, 6, 0, -6 },  // 0x64 'd'
  { 277, 5, 5, 6, 0, -4 },  // 0x65 'e'
  { 281, 4, 7, 5, 0, -6 },  // 0x66 'f'
  { 285, 5, 7, 6, 0, -4 },  // 0x67 'g'
  { 290, 5, 7, 6, 0, -6 },  // 0x68 'h'
  { 295, 1, 7, 3, 1, -6 },  // 0x69 'i'
  { 296, 2, 9, 4, 1, -6 },  // 0x6A 'j'
  { 299, 4, 7, 5, 0, -6 },  // 0x6B 'k'
  { 303, 2, 7, 4, 1, -6 },  // 0x6C 'l'
  { 305, 5, 5, 6, 0, -4 },  // 0x6D 'm'
  { 309, 5, 5, 6, 0, -4 },  // 0x6E 'n'
  { 313, 5, 5, 6, 0, -4 },  // 0x6F 'o'
  { 317, 5, 7, 6, 0, -4 },  // 0x70 'p'
  { 322, 5, 7, 6, 0, -4 },  // 0x71 'q'
  { 327, 4, 5, 5, 0, -4 },  // 0x72 'r'
  { 330, 5, 5, 6, 0, -4 },  // 0x73 's'
  { 334, 4, 7, 5, 0, -6 },  // 0x74 't'
  { 338, 5, 5, 6, 0, -4 },  // 0x75 'u'
  { 342, 5, 5, 6, 0, -4 },  // 0x76 'v'
  { 346, 5, 5, 6, 0, -4 },  // 0x77 'w'
  { 350, 5, 5, 6, 0, -4 },  // 0x78 'x'
  { 354, 5, 7, 6, 0, -4 },  // 0x79 'y'
  { 359, 5, 5, 6, 0, -4 },  // 0x7A 'z'
  { 363, 3, 9, 6, 2, -7 },  // 0x7B '{'
  { 367, 1, 7, 3, 1, -6 },  // 0x7C '|'
  { 368, 3, 9, 6, 1, -7 },  // 0x7D '}'
  { 372, 5, 2, 6, 0, -3 }   // 0x7E '~'
};

 GFXfont Font_10  = { (uint8_t *)Font_10Bitmaps, (GFXglyph *)Font_10Glyphs, 0x20, 0x7E, 12 };

// ===== 16 pixel height

const uint8_t Font_16Bitmaps[]  = {
  0x00, 0xFF, 0xFF, 0x0F, 0x99, 0x99, 0x90, 0x04, 0x40, 0xCC, 0x0C, 0x87, 0xFF, 0x09, 0x81, 0x98, 0x19, 0x0F, 0xFE, 0x13, 0x03, 0x30, 0x22, 0x00,
  0x08, 0x04, 0x0F, 0x9D, 0x2C, 0x86, 0x43, 0xF0, 0x7E, 0x0B, 0x84, 0xC2, 0x71, 0x77, 0xE0, 0x40, 0x20, 0x00, 0x61, 0x49, 0x24, 0x8C, 0x80, 0x40,
  0x40, 0x20, 0x26, 0x14, 0x92, 0x49, 0x28, 0x60, 0x3C, 0x18, 0x86, 0x01, 0x80, 0x60, 0x0C, 0x0F, 0x8F, 0x33, 0xC6, 0xF0, 0xE6, 0x18, 0xFB, 0xF8,
  0x00, 0x39, 0x98, 0xCC, 0x63, 0x18, 0xC6, 0x30, 0xC6, 0x18, 0xE0, 0xE3, 0x0C, 0x61, 0x8C, 0x63, 0x18, 0xC6, 0x63, 0x33, 0x80, 0x10, 0x23, 0x59,
  0xC3, 0x9A, 0xC4, 0x08, 0x30, 0xCF, 0xFF, 0x30, 0xC0, 0xF6, 0xFF, 0xF0, 0xF0, 0x11, 0x12, 0x22, 0x44, 0x48, 0x88, 0x3E, 0x31, 0x98, 0xD8, 0x3C,
  0x1E, 0x0F, 0x07, 0x83, 0xC1, 0xB1, 0x98, 0xC7, 0xC0, 0x38, 0xD8, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xFF, 0x7C, 0xC6, 0x83,
  0x03, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0xFF, 0x7C, 0x87, 0x03, 0x03, 0x06, 0x3C, 0x06, 0x03, 0x03, 0x03, 0x86, 0x7C, 0x0C, 0x1C, 0x1C,
  0x2C, 0x2C, 0x4C, 0x4C, 0x8C, 0xFF, 0x0C, 0x0C, 0x0C, 0x7E, 0x60, 0x60, 0x60, 0x7C, 0x46, 0x03, 0x03, 0x03, 0x03, 0x86, 0x7C, 0x1E, 0x18, 0x98,
  0x18, 0x0D, 0xE7, 0x1B, 0x07, 0x83, 0xC1, 0xA0, 0xD8, 0xC7, 0xC0, 0xFF, 0x03, 0x06, 0x06, 0x06, 0x0C, 0x0C, 0x0C, 0x18, 0x18, 0x18, 0x30, 0x3E,
  0x71, 0xF0, 0x78, 0x36, 0x31, 0xF1, 0x8D, 0x83, 0xC1, 0xE0, 0xD8, 0xC7, 0xC0, 0x3E, 0x31, 0xB0, 0x58, 0x3C, 0x1E, 0x0D, 0x8E, 0x7B, 0x01, 0x81,
  0x91, 0x87, 0x80, 0xF0, 0x3C, 0x00, 0xF0, 0x3D, 0x80, 0x00, 0x20, 0x1E, 0x0F, 0xC7, 0xE0, 0xF0, 0x07, 0xE0, 0x0F, 0xC0, 0x1E, 0x00, 0x20, 0xFF,
  0xFF, 0xFC, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xC0, 0x80, 0x1E, 0x01, 0xF8, 0x07, 0xE0, 0x1E, 0x1F, 0x9F, 0x87, 0x80, 0x80, 0x00, 0x7D, 0x0C, 0x18,
  0x30, 0xC3, 0x0C, 0x18, 0x30, 0x00, 0xC1, 0x80, 0x07, 0x80, 0x61, 0x83, 0x03, 0x18, 0x06, 0xC3, 0x8F, 0x1B, 0x3C, 0xCC, 0xF3, 0x33, 0xCC, 0xEF,
  0x1F, 0x66, 0x00, 0x0C, 0x00, 0x18, 0x60, 0x1E, 0x00, 0x0E, 0x01, 0xC0, 0x38, 0x0D, 0x81, 0xB0, 0x63, 0x0C, 0x61, 0xFC, 0x60, 0xCC, 0x19, 0x83,
  0x60, 0x30, 0xFE, 0x61, 0xB0, 0xD8, 0x6C, 0x37, 0xE3, 0x0D, 0x83, 0xC1, 0xE0, 0xF0, 0xDF, 0xC0, 0x1F, 0x18, 0xD8, 0x18, 0x0C, 0x06, 0x03, 0x01,
  0x80, 0xC0, 0x30, 0x0C, 0x63, 0xE0, 0xFE, 0x30, 0xCC, 0x1B, 0x03, 0xC0, 0xF0, 0x3C, 0x0F, 0x03, 0xC0, 0xF0, 0x6C, 0x33, 0xF8, 0xFF, 0xC0, 0xC0,
  0xC0, 0xC0, 0xFF, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xFF, 0xFF, 0x83, 0x06, 0x0C, 0x1F, 0xB0, 0x60, 0xC1, 0x83, 0x06, 0x00, 0x1F, 0x8C, 0x76, 0x07,
  0x00, 0xC0, 0x30, 0x0C, 0x3F, 0x0F, 0xC0, 0xD8, 0x33, 0x18, 0x7C, 0xC1, 0xE0, 0xF0, 0x78, 0x3C, 0x1F, 0xFF, 0x07, 0x83, 0xC1, 0xE0, 0xF0, 0x78,
  0x30, 0xFF, 0xFF, 0xFF, 0x18, 0xC6, 0x31, 0x8C, 0x63, 0x18, 0xC6, 0x3F, 0xF8, 0x00, 0xC1, 0xB0, 0xCC, 0x63, 0x30, 0xD8, 0x3C, 0x0D, 0x83, 0x30,
  0xC6, 0x30, 0xCC, 0x1B, 0x03, 0xC1, 0x83, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xC1, 0x83, 0x07, 0xF0, 0xE0, 0xFC, 0x1F, 0xC7, 0xF8, 0xFD, 0xB7, 0xB6,
  0xF2, 0x9E, 0x73, 0xCE, 0x78, 0x8F, 0x01, 0xE0, 0x30, 0xC1, 0xF0, 0xF8, 0x7E, 0x3D, 0x9E, 0xCF, 0x37, 0x9B, 0xC7, 0xE3, 0xF0, 0xF8, 0x30, 0x1F,
  0x06, 0x31, 0x83, 0x60, 0x3C, 0x07, 0x80, 0xF0, 0x1E, 0x03, 0xC0, 0x6C, 0x18, 0xC6, 0x0F, 0x80, 0xFC, 0xC6, 0xC3, 0xC3, 0xC3, 0xC6, 0xFC, 0xC0,
  0xC0, 0xC0, 0xC0, 0xC0, 0x1F, 0x06, 0x31, 0x83, 0x60, 0x3C, 0x07, 0x80, 0xF0, 0x1E, 0x03, 0xC0, 0x6C, 0x18, 0xC6, 0x0F, 0x80, 0x30, 0x03, 0x80,
  0xFC, 0x63, 0x30, 0xD8, 0x6C, 0x36, 0x33, 0xF1, 0x8C, 0xC3, 0x61, 0xB0, 0x78, 0x30, 0x3E, 0x31, 0xB0, 0x18, 0x0F, 0x03, 0xF0, 0x3C, 0x03, 0x01,
  0xC0, 0xF0, 0xCF, 0xC0, 0xFF, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xC1, 0xE0, 0xF0, 0x78, 0x3C, 0x1E, 0x0F, 0x07,
  0x83, 0xC1, 0xE0, 0xD8, 0xC7, 0xC0, 0xC1, 0xE0, 0xF0, 0x6C, 0x66, 0x33, 0x18, 0xD8, 0x6C, 0x36, 0x0E, 0x07, 0x03, 0x80, 0xC1, 0x83, 0xC1, 0x83,
  0xC1, 0x83, 0x63, 0xC6, 0x63, 0xC6, 0x63, 0xC6, 0x36, 0x6C, 0x36, 0x6C, 0x36, 0x6C, 0x1C, 0x38, 0x1C, 0x38, 0x1C, 0x38, 0xE0, 0xEE, 0x38, 0xC6,
  0x0D, 0xC1, 0xF0, 0x1C, 0x03, 0x80, 0xF8, 0x3B, 0x86, 0x31, 0xC3, 0x70, 0x70, 0xC0, 0xF0, 0x36, 0x19, 0x86, 0x33, 0x0C, 0xC1, 0xE0, 0x78, 0x0C,
  0x03, 0x00, 0xC0, 0x30, 0xFF, 0xFF, 0xF0, 0x38, 0x0C, 0x06, 0x03, 0x81, 0xC0, 0x60, 0x30, 0x1C, 0x0F, 0xFF, 0xFF, 0xFC, 0xCC, 0xCC, 0xCC, 0xCC,
  0xCC, 0xCC, 0xF0, 0x88, 0x84, 0x44, 0x22, 0x21, 0x11, 0xF3, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0xF0, 0x0C, 0x07, 0x83, 0x31, 0x86, 0xC0, 0xC0,
  0xFF, 0xC6, 0x30, 0x3E, 0x47, 0x03, 0x3F, 0xE3, 0xC3, 0xC3, 0xC7, 0x7B, 0xC0, 0xC0, 0xC0, 0xC0, 0xDC, 0xE6, 0xC3, 0xC3, 0xC3, 0xC3, 0xE6, 0xDC,
  0x3C, 0xC7, 0x06, 0x0C, 0x18, 0x18, 0x9E, 0x03, 0x03, 0x03, 0x03, 0x3B, 0x67, 0xC3, 0xC3, 0xC3, 0xC3, 0x67, 0x3B, 0x3C, 0x66, 0xC3, 0xFF, 0xC0,
  0xC0, 0x63, 0x3E, 0x3D, 0x86, 0x18, 0xF9, 0x86, 0x18, 0x61, 0x86, 0x18, 0x3B, 0x67, 0xC3, 0xC3, 0xC3, 0xC3, 0x67, 0x3B, 0x03, 0x46, 0x3C, 0xC0,
  0xC0, 0xC0, 0xC0, 0xDE, 0xE7, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xF0, 0xFF, 0xFF, 0x33, 0x00, 0x33, 0x33, 0x33, 0x33, 0xFE, 0xC1, 0x83, 0x06,
  0x0C, 0x79, 0xB6, 0x78, 0xF1, 0xB3, 0x36, 0x30, 0xFF, 0xFF, 0xFF, 0xDE, 0x7B, 0x9A, 0x6C, 0x30, 0xF0, 0xC3, 0xC3, 0x0F, 0x0C, 0x3C, 0x30, 0xF0,
  0xC3, 0xDE, 0xE7, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0x3E, 0x31, 0xB0, 0x78, 0x3C, 0x1E, 0x0D, 0x8C, 0x7C, 0xDC, 0xE6, 0xC3, 0xC3, 0xC3, 0xC3,
  0xE6, 0xDC, 0xC0, 0xC0, 0xC0, 0x3B, 0x67, 0xC3, 0xC3, 0xC3, 0xC3, 0x67, 0x3B, 0x03, 0x03, 0x03, 0xDF, 0x31, 0x8C, 0x63, 0x18, 0x7D, 0x87, 0x07,
  0x87, 0xC1, 0xC1, 0xC3, 0x7C, 0x00, 0x61, 0x86, 0x3F, 0x61, 0x86, 0x18, 0x61, 0x86, 0x0F, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xE7, 0x7B, 0xC1,
  0xE0, 0xD8, 0xCC, 0x63, 0x61, 0xB0, 0x70, 0x38, 0xC6, 0x3C, 0x63, 0x66, 0x66, 0xF6, 0x69, 0x66, 0x96, 0x39, 0xC3, 0x0C, 0xE3, 0xB1, 0x8D, 0x83,
  0x81, 0xC1, 0xB1, 0x8D, 0xC7, 0xC1, 0xE0, 0xD8, 0xCC, 0x63, 0x61, 0xB0, 0x70, 0x38, 0x0C, 0x3E, 0x0E, 0x00, 0xFE, 0x0C, 0x30, 0xC3, 0x0C, 0x30,
  0x7F, 0x0E, 0x3C, 0x60, 0xC1, 0x87, 0x1C, 0x70, 0x70, 0x70, 0x60, 0xC1, 0x83, 0xC3, 0x80, 0xFF, 0xFE, 0x00, 0xE1, 0xE0, 0xC1, 0x83, 0x07, 0x07,
  0x07, 0x1C, 0x70, 0xC1, 0x83, 0x1E, 0x38, 0x00, 0x30, 0xA4, 0xA1, 0x80
};

const GFXglyph Font_16Glyphs[]  = {
  { 0, 1, 1, 6, 0, 3 },         // 0x20 ' '
  { 1, 2, 12, 6, 2, -11 },      // 0x21 '!'
  { 4, 4, 5, 8, 2, -11 },       // 0x22 '"'
  { 7, 12, 11, 14, 1, -10 },    // 0x23 '#'
  { 24, 9, 15, 11, 1, -12 },    // 0x24 '$'
  { 42, 9, 12, 11, 1, -11 },    // 0x25 '%'
  { 56, 10, 12, 12, 1, -11 },   // 0x26 '&'
  { 71, 1, 5, 5, 2, -11 },      // 0x27 '''
  { 73, 5, 15, 11, 3, -12 },    // 0x28 '('
  { 83, 5, 16, 11, 3, -12 },    // 0x29 ')'
  { 93, 7, 8, 11, 2, -8 },      // 0x2A '*'
  { 100, 6, 6, 11, 3, -7 },     // 0x2B '+'
  { 105, 2, 4, 4, 1, -1 },      // 0x2C ','
  { 106, 6, 2, 11, 3, -5 },     // 0x2D '-'
  { 108, 2, 2, 4, 1, -1 },      // 0x2E '.'
  { 109, 4, 12, 6, 1, -11 },    // 0x2F '/'
  { 115, 9, 12, 11, 1, -11 },   // 0x30 '0'
  { 129, 8, 12, 11, 1, -11 },   // 0x31 '1'
  { 141, 8, 12, 11, 1, -11 },   // 0x32 '2'
  { 153, 8, 12, 11, 1, -11 },   // 0x33 '3'
  { 165, 8, 12, 11, 1, -11 },   // 0x34 '4'
  { 177, 8, 12, 11, 1, -11 },   // 0x35 '5'
  { 189, 9, 12, 11, 1, -11 },   // 0x36 '6'
  { 203, 8, 12, 11, 1, -11 },   // 0x37 '7'
  { 215, 9, 12, 11, 1, -11 },   // 0x38 '8'
  { 229, 9, 12, 11, 1, -11 },   // 0x39 '9'
  { 243, 2, 7, 4, 1, -6 },      // 0x3A ':'
  { 246, 2, 9, 4, 1, -6 },      // 0x3B ';'
  { 249, 12, 9, 15, 2, -9 },    // 0x3C '<'
  { 263, 11, 6, 13, 1, -7 },    // 0x3D '='
  { 272, 11, 9, 13, 1, -9 },    // 0x3E '>'
  { 285, 7, 12, 9, 1, -11 },    // 0x3F '?'
  { 296, 14, 14, 16, 1, -12 },  // 0x40 '@'
  { 321, 11, 12, 12, 0, -11 },  // 0x41 'A'
  { 338, 9, 12, 11, 1, -11 },   // 0x42 'B'
  { 352, 9, 12, 11, 1, -11 },   // 0x43 'C'
  { 366, 10, 12, 12, 1, -11 },  // 0x44 'D'
  { 381, 8, 12, 10, 1, -11 },   // 0x45 'E'
  { 393, 7, 12, 9, 1, -11 },    // 0x46 'F'
  { 404, 10, 12, 12, 1, -11 },  // 0x47 'G'
  { 419, 9, 12, 11, 1, -11 },   // 0x48 'H'
  { 433, 2, 12, 6, 2, -11 },    // 0x49 'I'
  { 436, 5, 14, 7, 1, -11 },    // 0x4A 'J'
  { 446, 10, 12, 12, 1, -11 },  // 0x4B 'K'
  { 461, 7, 12, 9, 1, -11 },    // 0x4C 'L'
  { 472, 11, 12, 13, 1, -11 },  // 0x4D 'M'
  { 489, 9, 12, 11, 1, -11 },   // 0x4E 'N'
  { 503, 11, 12, 13, 1, -11 },  // 0x4F 'O'
  { 520, 8, 12, 10, 1, -11 },   // 0x50 'P'
  { 532, 11, 14, 13, 1, -11 },  // 0x51 'Q'
  { 552, 9, 12, 11, 1, -11 },   // 0x52 'R'
  { 566, 9, 12, 11, 1, -11 },   // 0x53 'S'
  { 580, 8, 12, 10, 1, -11 },   // 0x54 'T'
  { 592, 9, 12, 11, 1, -11 },   // 0x55 'U'
  { 606, 9, 12, 11, 1, -11 },   // 0x56 'V'
  { 620, 16, 12, 18, 1, -11 },  // 0x57 'W'
  { 644, 11, 12, 13, 1, -11 },  // 0x58 'X'
  { 661, 10, 12, 10, 0, -11 },  // 0x59 'Y'
  { 676, 10, 12, 12, 1, -11 },  // 0x5A 'Z'
  { 691, 4, 15, 8, 2, -12 },    // 0x5B '['
  { 699, 4, 12, 6, 1, -11 },    // 0x5C '\'
  { 705, 4, 15, 8, 2, -12 },    // 0x5D ']'
  { 713, 10, 5, 14, 2, -12 },   // 0x5E '^'
  { 720, 8, 1, 9, 0, 2 },       // 0x5F '_'
  { 721, 4, 3, 6, 1, -12 },     // 0x60 '`'
  { 723, 8, 9, 10, 1, -8 },     // 0x61 'a'
  { 732, 8, 12, 10, 1, -11 },   // 0x62 'b'
  { 744, 7, 8, 9, 1, -7 },      // 0x63 'c'
  { 751, 8, 12, 10, 1, -11 },   // 0x64 'd'
  { 763, 8, 8, 10, 1, -7 },     // 0x65 'e'
  { 771, 6, 12, 8, 1, -11 },    // 0x66 'f'
  { 780, 8, 11, 10, 1, -7 },    // 0x67 'g'
  { 791, 8, 12, 10, 1, -11 },   // 0x68 'h'
  { 803, 2, 12, 4, 1, -11 },    // 0x69 'i'
  { 806, 4, 14, 6, 1, -11 },    // 0x6A 'j'
  { 813, 7, 12, 9, 1, -11 },    // 0x6B 'k'
  { 824, 2, 12, 4, 1, -11 },    // 0x6C 'l'
  { 827, 14, 8, 16, 1, -7 },    // 0x6D 'm'
  { 841, 8, 8, 10, 1, -7 },     // 0x6E 'n'
  { 849, 9, 8, 11, 1, -7 },     // 0x6F 'o'
  { 858, 8, 11, 10, 1, -7 },    // 0x70 'p'
  { 869, 8, 11, 11, 1, -7 },    // 0x71 'q'
  { 880, 5, 8, 7, 1, -7 },      // 0x72 'r'
  { 885, 7, 9, 9, 1, -8 },      // 0x73 's'
  { 894, 6, 12, 8, 1, -11 },    // 0x74 't'
  { 903, 8, 8, 10, 1, -7 },     // 0x75 'u'
  { 911, 9, 8, 11, 1, -7 },     // 0x76 'v'
  { 920, 12, 8, 14, 1, -7 },    // 0x77 'w'
  { 932, 9, 8, 11, 1, -7 },     // 0x78 'x'
  { 941, 9, 11, 11, 1, -7 },    // 0x79 'y'
  { 954, 7, 8, 9, 1, -7 },      // 0x7A 'z'
  { 961, 7, 15, 9, 1, -12 },    // 0x7B '{'
  { 975, 1, 15, 3, 1, -12 },    // 0x7C '|'
  { 978, 7, 15, 9, 1, -12 },    // 0x7D '}'
  { 992, 9, 3, 11, 1, -6 }      // 0x7E '~'
};

 GFXfont Font_16  = { (uint8_t *)Font_16Bitmaps, (GFXglyph *)Font_16Glyphs, 0x20, 0x7E, 18 };

// ===== 24 pixel height

static uint8_t Font_24Bitmaps[] = {
  0x00, 0xFF, 0xFF, 0xFF, 0x03, 0xF0, 0xCF, 0x3C, 0xF3, 0xCF, 0x3C, 0xC0, 0x06, 0x60, 0x33, 0x01, 0x98, 0x0C, 0xC7, 0xFF, 0xBF, 0xFC, 0x33, 0x01,
  0x98, 0x19, 0x80, 0xCC, 0x3F, 0xFD, 0xFF, 0xE3, 0x30, 0x19, 0x80, 0xCC, 0x06, 0x60, 0x04, 0x00, 0x80, 0x10, 0x0F, 0xC7, 0xFD, 0xC8, 0xB1, 0x06,
  0x20, 0xE4, 0x0F, 0x80, 0xFE, 0x03, 0xE0, 0x4E, 0x08, 0xC1, 0x1C, 0x27, 0xFF, 0xC7, 0xE0, 0x10, 0x02, 0x00, 0x40, 0x00, 0x3C, 0x0C, 0x0C, 0xC1,
  0x83, 0x0C, 0x60, 0x61, 0x8C, 0x0C, 0x33, 0x01, 0x86, 0x60, 0x30, 0xD8, 0x03, 0x33, 0x00, 0x3C, 0xC7, 0x80, 0x19, 0x98, 0x06, 0x61, 0x80, 0xCC,
  0x30, 0x31, 0x86, 0x06, 0x30, 0xC1, 0x86, 0x18, 0x30, 0xC3, 0x0C, 0x0C, 0xC1, 0x80, 0xF0, 0x00, 0x0F, 0xC0, 0x1F, 0xE0, 0x38, 0x20, 0x30, 0x00,
  0x30, 0x00, 0x30, 0x00, 0x38, 0x00, 0x1C, 0x00, 0x3E, 0x00, 0x77, 0x06, 0xE3, 0x86, 0xC1, 0xCC, 0xC0, 0xEC, 0xC0, 0x78, 0xE0, 0x38, 0x70, 0xFC,
  0x3F, 0xCE, 0x0F, 0x87, 0xFF, 0xFC, 0x00, 0x19, 0x8C, 0xC6, 0x33, 0x18, 0xC6, 0x31, 0x8C, 0x63, 0x0C, 0x63, 0x0C, 0x61, 0x80, 0xC3, 0x18, 0x63,
  0x18, 0x43, 0x18, 0xC6, 0x31, 0x8C, 0x66, 0x31, 0x98, 0xCC, 0x00, 0x04, 0x00, 0x83, 0x11, 0xBA, 0xE1, 0xF0, 0x3E, 0x1D, 0x76, 0x23, 0x04, 0x00,
  0x80, 0x00, 0x18, 0x18, 0x18, 0xFF, 0xFF, 0x18, 0x18, 0x18, 0x6D, 0xBD, 0x80, 0xFF, 0xFF, 0xFC, 0x00, 0x03, 0x03, 0x06, 0x06, 0x06, 0x0C, 0x0C,
  0x0C, 0x18, 0x18, 0x18, 0x30, 0x30, 0x30, 0x60, 0x60, 0x60, 0xC0, 0xC0, 0x0F, 0x03, 0xFC, 0x70, 0xE6, 0x06, 0xE0, 0x7C, 0x03, 0xC0, 0x3C, 0x03,
  0xC0, 0x3C, 0x03, 0xC0, 0x3C, 0x03, 0xC0, 0x3E, 0x07, 0x60, 0x77, 0x0E, 0x3F, 0xC0, 0xF0, 0x0C, 0x0F, 0x0F, 0xC3, 0x30, 0x0C, 0x03, 0x00, 0xC0,
  0x30, 0x0C, 0x03, 0x00, 0xC0, 0x30, 0x0C, 0x03, 0x00, 0xC0, 0x30, 0xFF, 0xFF, 0xF0, 0x3F, 0x1F, 0xF3, 0x07, 0x00, 0x70, 0x06, 0x00, 0xC0, 0x18,
  0x06, 0x01, 0xC0, 0x30, 0x0C, 0x03, 0x00, 0xC0, 0x30, 0x0C, 0x03, 0x00, 0xFF, 0xFF, 0xFC, 0x00, 0x7E, 0x1F, 0xF2, 0x0E, 0x00, 0x60, 0x0C, 0x01,
  0x80, 0x30, 0x0C, 0x3F, 0x07, 0xF0, 0x07, 0x00, 0x30, 0x06, 0x00, 0xC0, 0x3C, 0x0E, 0xFF, 0xCF, 0xE0, 0x00, 0x03, 0x80, 0xF0, 0x1E, 0x06, 0xC0,
  0xD8, 0x33, 0x06, 0x61, 0x8C, 0x31, 0x8C, 0x31, 0x86, 0x60, 0xCF, 0xFF, 0xFF, 0xC0, 0x60, 0x0C, 0x01, 0x80, 0x30, 0x00, 0x7F, 0xCF, 0xF9, 0x80,
  0x30, 0x06, 0x00, 0xC0, 0x1F, 0xC3, 0xFC, 0x41, 0xC0, 0x1C, 0x01, 0x80, 0x30, 0x07, 0x80, 0xF8, 0x3B, 0x8E, 0x3F, 0x83, 0xE0, 0x00, 0x0F, 0x83,
  0xF0, 0xE0, 0x38, 0x0E, 0x01, 0x80, 0x37, 0xC7, 0xFC, 0xF1, 0xDC, 0x1F, 0x01, 0xE0, 0x3C, 0x07, 0x80, 0xF8, 0x3B, 0x8E, 0x3F, 0x83, 0xE0, 0x00,
  0xFF, 0xFF, 0xFC, 0x03, 0x00, 0x60, 0x1C, 0x03, 0x00, 0x60, 0x18, 0x03, 0x00, 0xE0, 0x18, 0x03, 0x00, 0xC0, 0x18, 0x07, 0x00, 0xC0, 0x18, 0x04,
  0x00, 0x00, 0x1F, 0x0F, 0xF9, 0xC7, 0x60, 0x3C, 0x07, 0x80, 0xF0, 0x1B, 0x06, 0x3F, 0x87, 0xF1, 0xC7, 0x60, 0x3C, 0x07, 0x80, 0xF0, 0x1B, 0x06,
  0x7F, 0xC7, 0xF0, 0x00, 0x1F, 0x07, 0xF1, 0xC7, 0x70, 0x7C, 0x07, 0x80, 0xF0, 0x1E, 0x03, 0xE0, 0xEE, 0x3C, 0xFF, 0x8F, 0xB0, 0x06, 0x01, 0xC0,
  0x70, 0x1C, 0x3F, 0x07, 0xC0, 0x00, 0xFC, 0x00, 0x3F, 0x6D, 0x80, 0x00, 0x0D, 0xB7, 0xB0, 0x00, 0x00, 0x70, 0x3F, 0x1F, 0x8F, 0xC0, 0xE0, 0x0F,
  0xC0, 0x1F, 0x80, 0x3F, 0x00, 0x70, 0xFF, 0xFF, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0x80, 0xE0, 0x0F, 0xC0, 0x1F,
  0x80, 0x3F, 0x00, 0x70, 0x3F, 0x1F, 0x8F, 0xC0, 0xE0, 0x00, 0x3E, 0x3F, 0xB0, 0xF0, 0x30, 0x18, 0x0C, 0x0C, 0x0E, 0x0E, 0x0E, 0x06, 0x03, 0x01,
  0x80, 0x00, 0x00, 0x30, 0x18, 0x0C, 0x00, 0x00, 0xFC, 0x00, 0x3F, 0xF8, 0x03, 0xC0, 0xF0, 0x38, 0x01, 0xC3, 0x80, 0x07, 0x38, 0x79, 0x99, 0x8F,
  0xFC, 0xFC, 0x71, 0xE3, 0xC7, 0x07, 0x1E, 0x30, 0x18, 0xF1, 0x80, 0xC7, 0x8C, 0x06, 0x3C, 0x60, 0x73, 0x71, 0x87, 0xB9, 0x8F, 0xFF, 0x8E, 0x1E,
  0x70, 0x38, 0x00, 0x00, 0xE0, 0x04, 0x03, 0xC0, 0xE0, 0x0F, 0xFE, 0x00, 0x1F, 0xC0, 0x00, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x07, 0xE0, 0x06,
  0x60, 0x06, 0x60, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x18, 0x18, 0x18, 0x18, 0x38, 0x1C, 0x3F, 0xFC, 0x3F, 0xFC, 0x60, 0x06, 0x60, 0x06, 0x60,
  0x06, 0xE0, 0x07, 0xFF, 0x0F, 0xFC, 0xC0, 0xEC, 0x06, 0xC0, 0x6C, 0x06, 0xC0, 0x6C, 0x0C, 0xFF, 0x8F, 0xFC, 0xC0, 0x6C, 0x03, 0xC0, 0x3C, 0x03,
  0xC0, 0x3C, 0x06, 0xFF, 0xEF, 0xF8, 0x07, 0xE0, 0x7F, 0xE3, 0xC1, 0xDC, 0x01, 0x60, 0x03, 0x80, 0x0C, 0x00, 0x30, 0x00, 0xC0, 0x03, 0x00, 0x0C,
  0x00, 0x30, 0x00, 0xE0, 0x01, 0x80, 0x07, 0x00, 0x4F, 0x07, 0x1F, 0xF8, 0x1F, 0x80, 0xFF, 0x81, 0xFF, 0xE3, 0x01, 0xE6, 0x00, 0xEC, 0x00, 0xD8,
  0x01, 0xF0, 0x01, 0xE0, 0x03, 0xC0, 0x07, 0x80, 0x0F, 0x00, 0x1E, 0x00, 0x3C, 0x00, 0xF8, 0x01, 0xB0, 0x07, 0x60, 0x3C, 0xFF, 0xF1, 0xFF, 0x00,
  0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x60, 0x0C, 0x01, 0x80, 0x30, 0x06, 0x00, 0xFF, 0xDF, 0xFB, 0x00, 0x60, 0x0C, 0x01, 0x80, 0x30, 0x06, 0x00, 0xFF,
  0xFF, 0xFC, 0x00, 0xFF, 0xFF, 0xFC, 0x03, 0x00, 0xC0, 0x30, 0x0C, 0x03, 0x00, 0xFF, 0xBF, 0xEC, 0x03, 0x00, 0xC0, 0x30, 0x0C, 0x03, 0x00, 0xC0,
  0x30, 0x00, 0x07, 0xE0, 0x3F, 0xF0, 0xE0, 0x73, 0x80, 0x26, 0x00, 0x1C, 0x00, 0x30, 0x00, 0x60, 0x00, 0xC0, 0x7F, 0x80, 0xFF, 0x00, 0x1E, 0x00,
  0x3E, 0x00, 0x6C, 0x00, 0xDC, 0x01, 0x9E, 0x07, 0x1F, 0xFC, 0x0F, 0xE0, 0x00, 0xC0, 0x1E, 0x00, 0xF0, 0x07, 0x80, 0x3C, 0x01, 0xE0, 0x0F, 0x00,
  0x78, 0x03, 0xFF, 0xFF, 0xFF, 0xF0, 0x07, 0x80, 0x3C, 0x01, 0xE0, 0x0F, 0x00, 0x78, 0x03, 0xC0, 0x1E, 0x00, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0,
  0x0C, 0x30, 0xC3, 0x0C, 0x30, 0xC3, 0x0C, 0x30, 0xC3, 0x0C, 0x30, 0xC3, 0x0C, 0x30, 0xC3, 0xFB, 0xE0, 0xC0, 0x73, 0x03, 0x8C, 0x1C, 0x30, 0xE0,
  0xC7, 0x03, 0x38, 0x0D, 0xC0, 0x3E, 0x00, 0xF0, 0x03, 0x60, 0x0C, 0xE0, 0x31, 0xC0, 0xC3, 0x83, 0x07, 0x0C, 0x0E, 0x30, 0x1C, 0xC0, 0x3B, 0x00,
  0x70, 0xC0, 0x18, 0x03, 0x00, 0x60, 0x0C, 0x01, 0x80, 0x30, 0x06, 0x00, 0xC0, 0x18, 0x03, 0x00, 0x60, 0x0C, 0x01, 0x80, 0x30, 0x06, 0x00, 0xFF,
  0xFF, 0xFC, 0x00, 0xE0, 0x07, 0xF0, 0x0F, 0xF0, 0x0F, 0xF8, 0x1F, 0xD8, 0x1B, 0xD8, 0x1B, 0xCC, 0x33, 0xCC, 0x33, 0xCC, 0x33, 0xC6, 0x63, 0xC6,
  0x63, 0xC7, 0xE3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC1, 0x83, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xE0, 0x1F, 0x80, 0xFC, 0x07, 0xF0, 0x3D, 0x81, 0xE6,
  0x0F, 0x30, 0x78, 0xC3, 0xC6, 0x1E, 0x18, 0xF0, 0xC7, 0x83, 0x3C, 0x19, 0xE0, 0x6F, 0x03, 0x78, 0x0F, 0xC0, 0x7E, 0x01, 0xC0, 0x07, 0xE0, 0x1F,
  0xF8, 0x3C, 0x1C, 0x70, 0x0E, 0x60, 0x06, 0xE0, 0x07, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xE0, 0x07, 0x60,
  0x06, 0x70, 0x0E, 0x38, 0x1C, 0x1F, 0xF8, 0x07, 0xE0, 0xFF, 0x1F, 0xFB, 0x03, 0x60, 0x3C, 0x07, 0x80, 0xF0, 0x1E, 0x06, 0xFF, 0xDF, 0xE3, 0x00,
  0x60, 0x0C, 0x01, 0x80, 0x30, 0x06, 0x00, 0xC0, 0x18, 0x00, 0x00, 0x07, 0xE0, 0x1F, 0xF8, 0x3C, 0x1C, 0x70, 0x0E, 0x60, 0x06, 0xE0, 0x07, 0xC0,
  0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xE0, 0x07, 0x60, 0x06, 0x70, 0x0E, 0x38, 0x1C, 0x1F, 0xF8, 0x07, 0xF0, 0x00,
  0xF8, 0x00, 0x1F, 0x00, 0x0F, 0xFF, 0x07, 0xFE, 0x30, 0x31, 0x80, 0xCC, 0x06, 0x60, 0x33, 0x01, 0x98, 0x18, 0xFF, 0xC7, 0xFC, 0x30, 0x71, 0x81,
  0x8C, 0x06, 0x60, 0x33, 0x01, 0xD8, 0x06, 0xC0, 0x36, 0x00, 0xC0, 0x1F, 0x87, 0xFE, 0x70, 0x6C, 0x00, 0xC0, 0x0C, 0x00, 0xC0, 0x07, 0x00, 0x7F,
  0x01, 0xFC, 0x01, 0xE0, 0x07, 0x00, 0x30, 0x03, 0x00, 0x3C, 0x0E, 0xFF, 0xE3, 0xF8, 0xFF, 0xFF, 0xF0, 0xC0, 0x30, 0x0C, 0x03, 0x00, 0xC0, 0x30,
  0x0C, 0x03, 0x00, 0xC0, 0x30, 0x0C, 0x03, 0x00, 0xC0, 0x30, 0x0C, 0x03, 0x00, 0xC0, 0x1E, 0x00, 0xF0, 0x07, 0x80, 0x3C, 0x01, 0xE0, 0x0F, 0x00,
  0x78, 0x03, 0xC0, 0x1E, 0x00, 0xF0, 0x07, 0x80, 0x3C, 0x01, 0xE0, 0x0D, 0x80, 0xCE, 0x0E, 0x3F, 0xE0, 0x7C, 0x00, 0xC0, 0x03, 0x60, 0x06, 0x60,
  0x06, 0x60, 0x06, 0x30, 0x0C, 0x30, 0x0C, 0x38, 0x1C, 0x18, 0x18, 0x18, 0x18, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x06, 0x60, 0x06, 0x60, 0x07,
  0xE0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0xC0, 0x70, 0x1E, 0x03, 0x80, 0xD8, 0x36, 0x0C, 0xC1, 0xB0, 0x66, 0x0D, 0x83, 0x30, 0x6C, 0x18, 0xC6,
  0x31, 0x86, 0x31, 0x8C, 0x31, 0x8C, 0x61, 0x8C, 0x63, 0x06, 0xC1, 0xB0, 0x36, 0x0D, 0x81, 0xB0, 0x6C, 0x0D, 0x83, 0x60, 0x38, 0x0E, 0x01, 0xC0,
  0x70, 0x0E, 0x03, 0x80, 0x70, 0x1C, 0x00, 0xE0, 0x0E, 0xE0, 0x38, 0xC0, 0x61, 0xC1, 0xC1, 0xC7, 0x01, 0x8C, 0x03, 0xB8, 0x03, 0xE0, 0x03, 0x80,
  0x07, 0x00, 0x1F, 0x00, 0x77, 0x00, 0xC6, 0x03, 0x8E, 0x0E, 0x0E, 0x18, 0x0C, 0x70, 0x1D, 0xC0, 0x1C, 0x00, 0xE0, 0x1D, 0x80, 0x63, 0x03, 0x0E,
  0x1C, 0x18, 0x60, 0x73, 0x80, 0xFC, 0x01, 0xE0, 0x07, 0x80, 0x0C, 0x00, 0x30, 0x00, 0xC0, 0x03, 0x00, 0x0C, 0x00, 0x30, 0x00, 0xC0, 0x03, 0x00,
  0x0C, 0x00, 0xFF, 0xFF, 0xFF, 0xF0, 0x01, 0x80, 0x0E, 0x00, 0x70, 0x01, 0x80, 0x0C, 0x00, 0x60, 0x03, 0x80, 0x1C, 0x00, 0x60, 0x03, 0x00, 0x18,
  0x00, 0xE0, 0x07, 0x00, 0x18, 0x00, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xF1, 0x8C, 0x63, 0x18, 0xC6, 0x31, 0x8C, 0x63, 0x18, 0xC6, 0x31, 0xFF, 0x80,
  0xC0, 0xC0, 0x60, 0x60, 0x60, 0x30, 0x30, 0x30, 0x18, 0x18, 0x18, 0x0C, 0x0C, 0x0C, 0x06, 0x06, 0x06, 0x03, 0x03, 0xFF, 0xC6, 0x31, 0x8C, 0x63,
  0x18, 0xC6, 0x31, 0x8C, 0x63, 0x18, 0xC7, 0xFF, 0x80, 0x07, 0x00, 0x7C, 0x07, 0x70, 0x71, 0xC7, 0x07, 0x70, 0x1C, 0x00, 0xFF, 0xFF, 0xFF, 0xE1,
  0xC3, 0x87, 0x3F, 0x0F, 0xF9, 0x03, 0x00, 0x30, 0x06, 0x3F, 0xDF, 0xFF, 0x03, 0xC0, 0x78, 0x1F, 0x87, 0xBF, 0xF3, 0xE6, 0x00, 0xC0, 0x0C, 0x00,
  0xC0, 0x0C, 0x00, 0xC0, 0x0C, 0xF8, 0xFF, 0xCF, 0x0E, 0xE0, 0x6C, 0x03, 0xC0, 0x3C, 0x03, 0xC0, 0x3C, 0x03, 0xE0, 0x6F, 0x0E, 0xFF, 0xCC, 0xF8,
  0x0F, 0x8F, 0xF7, 0x05, 0x80, 0xC0, 0x30, 0x0C, 0x03, 0x00, 0xC0, 0x18, 0x07, 0x04, 0xFF, 0x1F, 0x80, 0x00, 0x30, 0x03, 0x00, 0x30, 0x03, 0x00,
  0x31, 0xF3, 0x3F, 0xF7, 0x0F, 0x60, 0x7C, 0x03, 0xC0, 0x3C, 0x03, 0xC0, 0x3C, 0x03, 0x60, 0x77, 0x0F, 0x3F, 0xF1, 0xF3, 0x0F, 0x83, 0xFC, 0x70,
  0xE6, 0x03, 0xC0, 0x3F, 0xFF, 0xFF, 0xFC, 0x00, 0xC0, 0x06, 0x00, 0x70, 0x23, 0xFE, 0x0F, 0xC0, 0x0F, 0x1F, 0x30, 0x30, 0x30, 0xFF, 0xFF, 0x30,
  0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x1F, 0x33, 0xFF, 0x70, 0xFE, 0x07, 0xC0, 0x3C, 0x03, 0xC0, 0x3C, 0x03, 0xC0, 0x36,
  0x07, 0x70, 0xF3, 0xFF, 0x1F, 0x30, 0x07, 0x20, 0xE3, 0xFC, 0x1F, 0x80, 0xC0, 0x18, 0x03, 0x00, 0x60, 0x0C, 0x01, 0x9F, 0x3F, 0xF7, 0x87, 0xE0,
  0x78, 0x0F, 0x01, 0xE0, 0x3C, 0x07, 0x80, 0xF0, 0x1E, 0x03, 0xC0, 0x78, 0x0C, 0x00, 0xFC, 0x3F, 0xFF, 0xFF, 0xF0, 0x18, 0xC6, 0x00, 0x0C, 0x63,
  0x18, 0xC6, 0x31, 0x8C, 0x63, 0x18, 0xC7, 0xFF, 0x00, 0xC0, 0x18, 0x03, 0x00, 0x60, 0x0C, 0x01, 0x83, 0xB0, 0xE6, 0x38, 0xCE, 0x1B, 0x83, 0xE0,
  0x7C, 0x0D, 0xC1, 0x9C, 0x31, 0xC6, 0x1C, 0xC1, 0xD8, 0x1C, 0x00, 0xDB, 0x6D, 0xB6, 0xDB, 0x6D, 0xB6, 0xEC, 0x00, 0xCF, 0x87, 0xCF, 0xFD, 0xFE,
  0xF0, 0xF8, 0x7E, 0x07, 0x03, 0xC0, 0x60, 0x3C, 0x06, 0x03, 0xC0, 0x60, 0x3C, 0x06, 0x03, 0xC0, 0x60, 0x3C, 0x06, 0x03, 0xC0, 0x60, 0x3C, 0x06,
  0x03, 0xC0, 0x60, 0x30, 0xCF, 0x9F, 0xFB, 0xC3, 0xF0, 0x3C, 0x07, 0x80, 0xF0, 0x1E, 0x03, 0xC0, 0x78, 0x0F, 0x01, 0xE0, 0x3C, 0x06, 0x00, 0x1F,
  0x83, 0xFC, 0x70, 0xEE, 0x06, 0xC0, 0x3C, 0x03, 0xC0, 0x3C, 0x03, 0xC0, 0x3E, 0x06, 0x70, 0xE3, 0xFC, 0x1F, 0x80, 0xCF, 0x8F, 0xFC, 0xF0, 0xEE,
  0x06, 0xC0, 0x3C, 0x03, 0xC0, 0x3C, 0x03, 0xC0, 0x3E, 0x06, 0xF0, 0xEF, 0xFC, 0xCF, 0x8C, 0x00, 0xC0, 0x0C, 0x00, 0xC0, 0x00, 0x1F, 0x33, 0xFF,
  0x70, 0xF6, 0x07, 0xC0, 0x3C, 0x03, 0xC0, 0x3C, 0x03, 0xC0, 0x36, 0x07, 0x70, 0xF3, 0xFF, 0x1F, 0x30, 0x03, 0x00, 0x30, 0x03, 0x00, 0x30, 0xCF,
  0xFF, 0xF0, 0xE0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x3F, 0x1F, 0xEE, 0x0B, 0x00, 0xC0, 0x3F, 0x03, 0xF0, 0x1F, 0x00, 0xC0,
  0x38, 0x1F, 0xFE, 0x7F, 0x00, 0x30, 0x30, 0x30, 0x30, 0xFF, 0xFF, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3F, 0x1F, 0xC0, 0x78,
  0x0F, 0x01, 0xE0, 0x3C, 0x07, 0x80, 0xF0, 0x1E, 0x03, 0xC0, 0x78, 0x1F, 0x87, 0xBF, 0xF3, 0xE6, 0x00, 0xC0, 0x78, 0x0F, 0x01, 0xB0, 0x66, 0x0C,
  0xC1, 0x8C, 0x61, 0x8C, 0x31, 0x83, 0x60, 0x6C, 0x0F, 0x80, 0xE0, 0x00, 0xC1, 0xC1, 0xE0, 0xE0, 0xF0, 0x70, 0x6C, 0x6C, 0x66, 0x36, 0x33, 0x1B,
  0x18, 0xD8, 0xD8, 0x6C, 0x6C, 0x36, 0x36, 0x0E, 0x0E, 0x07, 0x07, 0x03, 0x83, 0x80, 0x80, 0x80, 0x00, 0xE0, 0x3B, 0x83, 0x8E, 0x38, 0x31, 0x80,
  0xD8, 0x07, 0xC0, 0x1C, 0x01, 0xF0, 0x1D, 0xC0, 0xC6, 0x0C, 0x18, 0xE0, 0xEE, 0x03, 0x80, 0xC0, 0x1B, 0x01, 0x98, 0x0C, 0xE0, 0xE3, 0x06, 0x18,
  0x70, 0x63, 0x03, 0x18, 0x0D, 0x80, 0x6C, 0x03, 0xE0, 0x0E, 0x00, 0x70, 0x03, 0x00, 0x18, 0x03, 0x80, 0x1C, 0x00, 0x00, 0xFF, 0xFF, 0xFC, 0x03,
  0x00, 0xE0, 0x38, 0x0E, 0x03, 0x80, 0xE0, 0x38, 0x0E, 0x01, 0x80, 0x7F, 0xFF, 0xFE, 0x00, 0x07, 0x87, 0xC3, 0x01, 0x80, 0xC0, 0x60, 0x30, 0x18,
  0x0C, 0x0E, 0x3E, 0x1F, 0x01, 0xC0, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x80, 0xF8, 0x3C, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0,
  0x7C, 0x06, 0x03, 0x01, 0x80, 0xC0, 0x60, 0x30, 0x18, 0x0E, 0x03, 0xE1, 0xF1, 0xC0, 0xC0, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x0F, 0x87, 0x80,
  0x00
};

static GFXglyph Font_24Glyphs[] = {
  { 0, 1, 1, 9, 0, 0 },          // 0x20 ' '
  { 1, 2, 18, 8, 3, -17 },       // 0x21 '!'
  { 6, 6, 7, 10, 2, -17 },       // 0x22 '"'
  { 12, 13, 16, 15, 1, -16 },    // 0x23 '#'
  { 38, 11, 21, 13, 1, -18 },    // 0x24 '$'
  { 68, 19, 18, 21, 1, -17 },    // 0x25 '%'
  { 112, 16, 18, 18, 1, -17 },   // 0x26 '&'
  { 148, 2, 7, 6, 2, -17 },      // 0x27 '''
  { 151, 5, 21, 9, 2, -19 },     // 0x28 '('
  { 165, 5, 21, 9, 2, -19 },     // 0x29 ')'
  { 179, 11, 10, 13, 1, -13 },   // 0x2A '*'
  { 194, 8, 8, 10, 1, -12 },     // 0x2B '+'
  { 202, 3, 6, 8, 2, -2 },       // 0x2C ','
  { 205, 8, 2, 10, 1, -9 },      // 0x2D '-'
  { 207, 2, 3, 8, 3, -2 },       // 0x2E '.'
  { 209, 8, 19, 10, 1, -18 },    // 0x2F '/'
  { 228, 12, 18, 15, 2, -17 },   // 0x30 '0'
  { 255, 10, 18, 15, 3, -17 },   // 0x31 '1'
  { 278, 11, 18, 15, 2, -17 },   // 0x32 '2'
  { 304, 11, 18, 15, 2, -17 },   // 0x33 '3'
  { 330, 11, 18, 15, 2, -17 },   // 0x34 '4'
  { 356, 11, 18, 15, 2, -17 },   // 0x35 '5'
  { 382, 11, 18, 15, 2, -17 },   // 0x36 '6'
  { 408, 11, 18, 15, 2, -17 },   // 0x37 '7'
  { 434, 11, 18, 15, 2, -17 },   // 0x38 '8'
  { 460, 11, 18, 15, 2, -17 },   // 0x39 '9'
  { 486, 2, 12, 9, 3, -11 },     // 0x3A ':'
  { 489, 3, 15, 9, 2, -11 },     // 0x3B ';'
  { 496, 12, 9, 14, 1, -12 },    // 0x3C '<'
  { 510, 15, 7, 21, 3, -11 },    // 0x3D '='
  { 524, 12, 9, 14, 1, -12 },    // 0x3E '>'
  { 538, 9, 18, 13, 2, -17 },    // 0x3F '?'
  { 559, 21, 21, 23, 1, -18 },   // 0x40 '@'
  { 615, 16, 18, 17, 0, -17 },   // 0x41 'A'
  { 651, 12, 18, 14, 1, -17 },   // 0x42 'B'
  { 678, 14, 18, 16, 1, -17 },   // 0x43 'C'
  { 710, 15, 18, 17, 1, -17 },   // 0x44 'D'
  { 745, 11, 18, 13, 1, -17 },   // 0x45 'E'
  { 771, 10, 18, 12, 1, -17 },   // 0x46 'F'
  { 794, 15, 18, 17, 1, -17 },   // 0x47 'G'
  { 829, 13, 18, 15, 1, -17 },   // 0x48 'H'
  { 859, 2, 18, 6, 2, -17 },     // 0x49 'I'
  { 864, 6, 22, 8, 1, -17 },     // 0x4A 'J'
  { 881, 14, 18, 16, 1, -17 },   // 0x4B 'K'
  { 913, 11, 18, 13, 1, -17 },   // 0x4C 'L'
  { 939, 16, 18, 18, 1, -17 },   // 0x4D 'M'
  { 975, 13, 18, 15, 1, -17 },   // 0x4E 'N'
  { 1005, 16, 18, 18, 1, -17 },  // 0x4F 'O'
  { 1041, 11, 18, 14, 1, -17 },  // 0x50 'P'
  { 1067, 16, 21, 18, 1, -17 },  // 0x51 'Q'
  { 1109, 13, 18, 15, 1, -17 },  // 0x52 'R'
  { 1139, 12, 18, 14, 1, -17 },  // 0x53 'S'
  { 1166, 10, 18, 12, 1, -17 },  // 0x54 'T'
  { 1189, 13, 18, 15, 1, -17 },  // 0x55 'U'
  { 1219, 16, 18, 17, 0, -17 },  // 0x56 'V'
  { 1255, 21, 18, 23, 1, -17 },  // 0x57 'W'
  { 1303, 15, 18, 17, 1, -17 },  // 0x58 'X'
  { 1338, 14, 18, 16, 1, -17 },  // 0x59 'Y'
  { 1370, 14, 18, 16, 1, -17 },  // 0x5A 'Z'
  { 1402, 5, 21, 9, 2, -18 },    // 0x5B '['
  { 1416, 8, 19, 10, 1, -18 },   // 0x5C '\'
  { 1435, 5, 21, 10, 2, -18 },   // 0x5D ']'
  { 1449, 13, 6, 17, 2, -19 },   // 0x5E '^'
  { 1460, 12, 2, 13, 0, 3 },     // 0x5F '_'
  { 1463, 6, 4, 10, 2, -19 },    // 0x60 '`'
  { 1466, 11, 13, 13, 1, -12 },  // 0x61 'a'
  { 1485, 12, 18, 14, 1, -17 },  // 0x62 'b'
  { 1512, 10, 13, 12, 1, -12 },  // 0x63 'c'
  { 1529, 12, 18, 14, 1, -17 },  // 0x64 'd'
  { 1556, 12, 13, 14, 1, -12 },  // 0x65 'e'
  { 1576, 8, 18, 10, 1, -17 },   // 0x66 'f'
  { 1594, 12, 17, 14, 1, -12 },  // 0x67 'g'
  { 1620, 11, 18, 13, 1, -17 },  // 0x68 'h'
  { 1646, 2, 18, 6, 2, -17 },    // 0x69 'i'
  { 1651, 5, 21, 7, 1, -17 },    // 0x6A 'j'
  { 1665, 11, 18, 13, 1, -17 },  // 0x6B 'k'
  { 1691, 3, 18, 6, 2, -17 },    // 0x6C 'l'
  { 1699, 20, 13, 22, 1, -12 },  // 0x6D 'm'
  { 1732, 11, 13, 13, 1, -12 },  // 0x6E 'n'
  { 1751, 12, 13, 14, 1, -12 },  // 0x6F 'o'
  { 1771, 12, 17, 14, 1, -12 },  // 0x70 'p'
  { 1797, 12, 17, 14, 1, -12 },  // 0x71 'q'
  { 1823, 8, 13, 10, 1, -12 },   // 0x72 'r'
  { 1836, 10, 13, 12, 1, -12 },  // 0x73 's'
  { 1853, 8, 17, 10, 1, -16 },   // 0x74 't'
  { 1870, 11, 13, 13, 1, -12 },  // 0x75 'u'
  { 1889, 11, 13, 13, 1, -12 },  // 0x76 'v'
  { 1908, 17, 13, 19, 1, -12 },  // 0x77 'w'
  { 1937, 13, 13, 15, 1, -12 },  // 0x78 'x'
  { 1959, 13, 17, 15, 1, -12 },  // 0x79 'y'
  { 1988, 11, 13, 13, 1, -12 },  // 0x7A 'z'
  { 2007, 9, 22, 11, 1, -18 },   // 0x7B '{'
  { 2033, 2, 24, 6, 2, -19 },    // 0x7C '|'
  { 2039, 9, 22, 11, 1, -18 }    // 0x7D '}'
};

GFXfont Font_24 = { (uint8_t *)Font_24Bitmaps, (GFXglyph *)Font_24Glyphs, 0x20, 0x7D, 27 };
