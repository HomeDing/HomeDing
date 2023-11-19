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

#include <displays/DisplayButtonElement.h>

#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)


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
bool DisplayButtonElement::touchStart(uint16_t xPos, uint16_t yPos) {
  // TRACE("touchStart(%d/%d)", xPos, yPos);
  bool over = overlap(xPos, yPos);
  if (over != _pressed) _needredraw = true;
  _pressed = over;
  return (over);
}

void DisplayButtonElement::touchEnd(uint16_t xPos, uint16_t yPos) {
  // TRACE("touchEnd(%d/%d)", xPos, yPos);
  if (overlap(xPos, yPos)) {
    _board->dispatch(_clickAction);
  }
  _pressed = false;
  _needredraw = true;
}


// ===== Element functions =====

/// @brief Initialize the DisplayButtonElement.
/// @param board Reference to the board.
void DisplayButtonElement::init(Board *board) {
  DisplayOutputElement::init(board);
  _h = 24;  // default height for buttons
}  // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayButtonElement::set(const char *name, const char *value) {
  // TRACE("set %s=%s", name, value);
  bool ret = true;

  if (DisplayOutputElement::set(name, value)) {
    // done

  } else if (_stricmp(name, "text") == 0) {
    _text = value;
    _needredraw = true;

  } else if (_stricmp(name, "onclick") == 0) {
    _clickAction = value;

  } else {
    ret = false;
  }  // if

  return (ret);
}  // set()


/// @brief Draw the button on display.
void DisplayButtonElement::draw() {
  DisplayOutputElement::draw(); // prepare colors
  _display->drawButton(_x, _y, _w, _h, _text.c_str(), _pressed);
}

// End
