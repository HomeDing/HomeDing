/**
 * @file DisplayLineElement.cpp
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
 * Changelog, see DisplayLineElement.h.
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <DisplayLineElement.h>

/**
 * @brief static factory function to create a new DisplayLineElement.
 * @return DisplayLineElement* as Element* created element
 */
Element *DisplayLineElement::create()
{
  return (new DisplayLineElement());
} // create()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayLineElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, "x0") == 0) {
    _x0 = _atoi(value);

  } else if (_stricmp(name, "x1") == 0) {
    _x1 = _atoi(value);

  } else if (_stricmp(name, "y0") == 0) {
    _y0 = _atoi(value);

  } else if (_stricmp(name, "y1") == 0) {
    _y1 = _atoi(value);

  } else if (_stricmp(name, "redraw") == 0) {
    _neededraw = true;

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the DisplayLineElement.
 */
void DisplayLineElement::start()
{
  DisplayAdapter *d = _board->display;

  if (d == NULL) {
    LOGGER_EERR("no display defined");

  } else {
    _display = d;
    _neededraw = true;
    Element::start();
  } // if
} // start()


/**
 * @brief check the state of the DHT values and eventually create actions.
 */
void DisplayLineElement::loop()
{
  if (_neededraw) {
    _display->drawLine(_x0, _y0, _x1, _y1);
    _display->flush();
    _neededraw = false;
  } // if
} // loop()


// End
