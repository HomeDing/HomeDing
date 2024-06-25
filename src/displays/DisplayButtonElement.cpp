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

#define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)


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
  if (over != _pressed) needsDraw = true;
  _pressed = over;
  return (over);
}

void DisplayButtonElement::touchEnd(int16_t xPos, int16_t yPos) {
  // TRACE("touchEnd(%d/%d)", xPos, yPos);
  if (box.contains(xPos, yPos)) {
    _board->dispatch(_clickAction);
  }
  _pressed = false;
  needsDraw = true;
}


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

  } else if (name == HomeDing::Action::Text) {
    _text = value;
    needsDraw = true;

  } else if (_stricmp(name, "onclick") == 0) {
    _clickAction = value;

  } else {
    ret = false;
  }  // if

  return (ret);
}  // set()


/// @brief Draw the button on display.
void DisplayButtonElement::draw() {

  TRACE("draw %d.%d.%d.%d", box.x_min, box.y_min, box.x_max, box.y_min);

  DisplayOutputElement::draw();  // prepare colors
  _display->drawButton(box.x_min, box.y_min, box.x_max - box.x_min + 1, box.y_max - box.y_min + 1, _text.c_str(), _pressed);
}

// End
