/**
 * @file DisplayButtonElement.cpp
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

/**
 * @brief static factory function to create a new DisplayButtonElement.
 * @return DisplayButtonElement* as Element* created element
 */
Element *DisplayButtonElement::create() {
  return (new DisplayButtonElement());
}  // create()


/// @brief Initialize the DisplayButtonElement.
/// @param board Reference to the board.
void DisplayButtonElement::init(Board *board) {
  DisplayOutputElement::init(board);
  _h = 24;  // default height for buttons
}  // init()


// return true when object is at the specified position or is overlapping with rectangle
bool DisplayButtonElement::overlap(int16_t rx, int16_t ry, uint16_t rw, uint16_t rh) {
  return ((rx + rw >= _x) && (rx < (_x + _w)) && (ry + rh >= _y) && (ry < (_y + _h)));
}

  /// @brief A touchStart elvent is given
  /// @param x X position of touch start event.
  /// @param y y position of touch start event.
  /// @return true when button wants to participate in further actions.
  bool DisplayButtonElement::touchStart(uint16_t xPos, uint16_t yPos) {
    // LOGGER_EINFO("touchStart(%d/%d)", xPos, yPos);
    _pressed = overlap(xPos, yPos);
    // LOGGER_EINFO("_pressed=%d", _pressed);

    if (_pressed) _needredraw = true;
    return (_pressed);
  }

  void DisplayButtonElement::touchEnd(uint16_t xPos, uint16_t yPos) {
    // LOGGER_EINFO("touchEnd(%d/%d)", xPos, yPos);
    _pressed = false;
    _needredraw = true;
  }

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayButtonElement::set(const char *name, const char *value) {
  TRACE("set %s=%s", name, value);
  bool ret = true;

  if (DisplayOutputElement::set(name, value)) {
    // done

  } else if (_stricmp(name, "text") == 0) {
    _text = value;
    _needredraw = true;

  } else {
    ret = false;
  }  // if

  return (ret);
}  // set()


/**
 * @brief Draw this output element.
 *
 */
void DisplayButtonElement::draw() {
  DisplayOutputElement::draw();
  _display->drawButton(_x, _y, _w, _h, _text.c_str(), _pressed);
}

/**
 * @brief push the current value of all properties to the callback.
 */
void DisplayButtonElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
}  // pushState()

// End
