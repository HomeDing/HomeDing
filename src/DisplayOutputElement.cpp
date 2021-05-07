/**
 * @file DisplayOutputElement.cpp
 * @brief Base Output Element for a display.
 * 
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog, see DisplayOutputElement.h.
 */

#include <Arduino.h>
#include <HomeDing.h>

#include <DisplayOutputElement.h>

#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayOutputElement::set(const char *name, const char *value)
{
  TRACE("set %s=%s", name, value);
  bool ret = Element::set(name, value);

  if (!ret) {
    ret = true;

    if (_stricmp(name, "redraw") == 0) {
      this->draw();

    } else if (_stricmp(name, "page") == 0) {
      _page = _atoi(value);

    } else {
      ret = false;
    }
  } // if
  return (ret);
} // set()


/**
 * @brief Activate the DisplayOutputElement.
 */
void DisplayOutputElement::start()
{
  DisplayAdapter *d = _board->display;

  if (d == NULL) {
    LOGGER_EERR("no display defined");

  } else {
    _display = d;
    if (_page > d->maxpage) {
      d->maxpage = _page;
    }
    Element::start();
  } // if
} // start()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
void DisplayOutputElement::draw() {}

// End