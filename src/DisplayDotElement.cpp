/**
 * @file DisplayDotElement.cpp
 * @brief Output Element for controlling a binary output on a display.
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
 * Changelog, see DisplayDotElement.h.
 */

#include <Arduino.h>
#include <Element.h>
#include <Board.h>

#include <DisplayDotElement.h>

/**
 * @brief static factory function to create a new DisplayDotElement.
 * @return DisplayDotElement* as Element* created element
 */
Element *DisplayDotElement::create()
{
  return (new DisplayDotElement());
} // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayDotElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, PROP_VALUE) == 0) {
    _value = _atob(value);

  } else if (_stricmp(name, "x") == 0) {
    _x = _atoi(value);

  } else if (_stricmp(name, "y") == 0) {
    _y = _atoi(value);

  } else if (!active) {
    // no actions.
    ret = Element::set(name, value);
    if (!ret) {
      LOGGER_EERR("inactive");
    }

  } else if (_stricmp(name, "clear") == 0) {
    _display->clear();
    _display->flush();

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the DisplayDotElement.
 */
void DisplayDotElement::start()
{
  DisplayAdapter *d = (DisplayAdapter *)(_board->display);

  if (d == NULL) {
    LOGGER_EERR("no display defined");

  } else {
    _display = d;
    _displayValue = !_value; // send to display next time
    Element::start();
  } // if
} // start()


/**
 * @brief check the state of the DHT values and eventually create actions.
 */
void DisplayDotElement::loop()
{
  if (_value != _displayValue) {
    _display->clear(_x, _y, _w, _h);
    _display->drawDot(_x, _y, _h, _value);
    _display->flush();
    _displayValue = _value;
  } // if
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void DisplayDotElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback(PROP_VALUE, _value ? "1" : "0");
} // pushState()

// End
