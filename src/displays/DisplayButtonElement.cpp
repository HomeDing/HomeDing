/**
 * @file displays/DisplayButtonElement.cpp
 * @brief Output Element for displaying a button on a display.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog, see DisplayButtonElement.h.
 */

#include <Arduino.h>
#include <HomeDing.h>

#if defined(ESP32)

#include <displays/DisplayButtonElement.h>
#include <gfxDraw.h>

#define TRACE(...)   LOGGER_ETRACE(__VA_ARGS__)


// ===== Element Registry =====

/// @brief Static factory function to create a new DisplayButtonElement.
/// @return DisplayButtonElement* as Element* created element
Element *DisplayButtonElement::create() {
  return (new DisplayButtonElement());
}  // create()


// ===== DisplayButton functions =====

/// @brief A touchStart event is given
/// @param x X position of touch start event.
/// @param y y position of touch start event.
/// @return true when button wants to participate in further actions.
bool DisplayButtonElement::touchStart(int16_t xPos, int16_t yPos) {
  // TRACE("touchStart(%d/%d)", xPos, yPos);
  bool over = box.contains(xPos, yPos);
  if (over != _pressed) {
    needsDraw = true;
    HomeDing::displayAdapter->setFlush();
    TRACE("touch(%d)", over);
  }
  _pressed = over;
  return (over);
}  // touchStart()


void DisplayButtonElement::touchEnd(int16_t xPos, int16_t yPos) {
  // TRACE("touchEnd(%d/%d)", xPos, yPos);
  if (box.contains(xPos, yPos)) {
    HomeDing::Actions::push(_clickAction);
  }
  _pressed = false;
  needsDraw = true;
  HomeDing::displayAdapter->setFlush();
}  // touchEnd()


// ===== Element functions =====

/// @brief Initialize the DisplayButtonElement.
/// @param board Reference to the board.
void DisplayButtonElement::init(Board *board) {
  DisplayOutputElement::init(board);
}  // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayButtonElement::set(const char *name, const char *value) {
  // TRACE("set %s=%s", name, value);
  bool ret = true;

  if (DisplayOutputElement::set(name, value)) {
    // done

  } else if (name == HomeDing::Actions::Text) {
    _text = value;
    needsDraw = true;

  } else if (_stricmp(name, "onclick") == 0) {
    _clickAction = value;

  } else if (_stricmp(name, "action") == 0) {
    if (_stricmp(value, "click") == 0) {
      HomeDing::Actions::push(_clickAction);
    }

  } else {
    ret = false;
  }  // if

  return (ret);
}  // set()


/// @brief Draw the button on display.
void DisplayButtonElement::draw() {
  const uint16_t paddingVertical = 4;
  int16_t w = box.width();
  int16_t h = box.height();
  int16_t fontSize = _fontsize;
  int16_t padX = 0;
  int16_t padY = 0;

  TRACE("draw [%d/%d - %d/%d] <%s> %d", box.x_min, box.x_max, box.y_min, box.y_max, _text, _pressed);

  DisplayOutputElement::draw();  // prepare colors

  // fontsize can be defined smaller but not extending height - padding.
  if (fontSize < 0) { fontSize = h - (2 * paddingVertical); }
  if (fontSize < 0) { fontSize = 0; }

  BoundingBox txtBox = HomeDing::displayAdapter->textBox(fontSize, _text.c_str());

  txtBox.shift(box.x_min, box.y_min);

  // center horizontal
  padX = (box.x_max - txtBox.x_max) / 2;
  // center vertical
  if (txtBox.y_max < box.y_max) { padY = (box.y_max - txtBox.y_max) / 2; }

  HomeDing::strokeColor = (_pressed ? _backgroundColor : _strokeColor);
  HomeDing::fillColor = (_pressed ? _strokeColor : _backgroundColor);

  HomeDing::displayAdapter->startWrite();
  gfxDraw::drawRoundedRect(box.x_min, box.y_min, w, h, (h / 2), HomeDing::stroke, HomeDing::fill);
  HomeDing::displayAdapter->endWrite();

  HomeDing::displayAdapter->drawText(box.x_min + padX, box.y_min + padY, _fontsize, _text.c_str(), HomeDing::strokeColor);
}  // draw()


#endif

// End
