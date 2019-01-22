/**
 * @file DisplayTextElement.cpp
 * @brief Output Element for controlling a text output on a display.
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
 * Changelog, see DisplayTextElement.h.
 */

#include <DisplayTextElement.h>
#include <Board.h>

/**
 * @brief static factory function to create a new DisplayTextElement.
 * @return DisplayTextElement* as Element* created element
 */
Element *DisplayTextElement::create()
{
  return (new DisplayTextElement());
} // create()


/**
 * @brief send current text to display
 */
void DisplayTextElement::_drawText() {
  // LOGGER_EINFO("draw %d (%s)", active, _value.c_str());
  if (active) {
    _display->clear(_x, _y, _w, _h);
    String msg(_prefix);
    msg.concat(_value);
    msg.concat(_postfix);
    _w = _display->drawText(_x, _y, _h, msg);
    _display->flush();
  }
}

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayTextElement::set(const char *name, const char *value)
{
  // LOGGER_ETRACE("set(%s:%s)", name, value);
  bool ret = true;

  if (_stricmp(name, "prefix") == 0) {
    if (strlen(value) < sizeof(_prefix))
      strcpy(_prefix, value);

  } else if (_stricmp(name, "postfix") == 0) {
    if (strlen(value) < sizeof(_postfix))
      strcpy(_postfix, value);

  } else if (_stricmp(name, "x") == 0) {
    _x = _atoi(value);

  } else if (_stricmp(name, "y") == 0) {
    _y = _atoi(value);

  } else if (_stricmp(name, "fontsize") == 0) {
    int s = _atoi(value);
    if ((s == 10) || (s == 16) || (s == 24)) {
      _h = s;
    } // if

  } else if (_stricmp(name, "value") == 0) {
    _value = value;
    _drawText();

  } else if (! active) {
    // no actions.
    LOGGER_ETRACE(" not active.");

  } else if (_stricmp(name, "clear") == 0) {
    _display->clear();
    _display->flush();

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the DisplayTextElement.
 */
void DisplayTextElement::start()
{
  DisplayAdapter *d = (DisplayAdapter *)(_board->display);

  if (d == NULL) {
    LOGGER_EERR("no display defined");

  } else {
    _display = d;
    Element::start();
    _drawText();
  } // if
} // start()


/**
 * @brief push the current value of all properties to the callback.
 */
void DisplayTextElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  Element::pushState(callback);
  callback("value", _value.c_str());
} // pushState()

// End
