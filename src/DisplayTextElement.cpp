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

#include <Arduino.h>
#include <HomeDing.h>

#include <DisplayTextElement.h>

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
void DisplayTextElement::draw()
{
  if (active && (_display->page == _page)) {
    DisplayOutputElement::draw();
    _display->clear(_x, _y, _w, _h);
    String msg(_prefix);
    msg.concat(_value);
    msg.concat(_postfix);
    _w = _display->drawText(_x, _y, _fontsize, msg);
    _display->flush();
  }
} // draw

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DisplayTextElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, PROP_VALUE) == 0) {
    _value = value;
    draw();

  } else if (_stricmp(name, "prefix") == 0) {
    _prefix = value;

  } else if (_stricmp(name, "postfix") == 0) {
    _postfix = value;

  } else if (_stricmp(name, "x") == 0) {
    _x = _atoi(value);

  } else if (_stricmp(name, "y") == 0) {
    _y = _atoi(value);

  } else if (_stricmp(name, "fontsize") == 0) {
    int s = _atoi(value);
    if ((s == 10) || (s == 16) || (s == 24)) {
      _fontsize = s;
    } // if

  } else if (_stricmp(name, "clear") == 0) {
    _value.clear();
    draw();

  } else {
    ret = DisplayOutputElement::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the DisplayTextElement.
 */
void DisplayTextElement::start()
{
  DisplayOutputElement::start();

  if (_display) {
    _h = _display->getFontHeight(_fontsize);
    draw();
  } // if
} // start()


/**
 * @brief push the current value of all properties to the callback.
 */
void DisplayTextElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  DisplayOutputElement::pushState(callback);
  callback(PROP_VALUE, _value.c_str());
} // pushState()

// End
