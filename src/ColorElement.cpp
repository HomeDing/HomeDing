/**
 * @file TheColorAnimationElement.cpp
 * The ColorAnimation Element creates a series or pattern of RGB color values.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD 3-Clause style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see ColorElement.h
 */

#include <Arduino.h>
#include <Board.h>
#include <Element.h>

#include <HomeDing.h>

#include <ColorElement.h>


/* ===== Define local constants and often used strings ===== */

// like:
// #define ANYCONSTANT 1000

#define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new ColorElement
 * @return ColorElement* created element
 */
Element *ColorElement::create()
{
  return (new ColorElement());
} // create()


/* ===== Element functions ===== */

void ColorElement::init(Board *board)
{
  TRACE("init()");
  Element::init(board);
  // do something here like initialization
} // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool ColorElement::set(const char *name, const char *value)
{
  TRACE("set(%s=%s)", name, value);

  bool ret = true;
  unsigned long now = millis();

  if (_stricmp(name, PROP_VALUE) == 0) {
    uint32_t colorValue = _atoColor(value);

    if (_mode == Mode::fade) {
      _fromValue = _value;
      _toValue = colorValue;
      _startTime = now;

    } else if ((_mode == Mode::fix) || (_mode == Mode::pulse)) {
      _toValue = colorValue;
    }

  } else if (_stricmp(name, "mode") == 0) {
    if (_stricmp(value, "fade") == 0) {
      _mode = Mode::fade;
      _toValue = _value; // do not start fading without a new value

    } else if (_stricmp(value, "wheel") == 0) {
      _mode = Mode::wheel;

    } else if (_stricmp(value, "pulse") == 0) {
      _mode = Mode::pulse;
      _toValue = _value;
      _startTime = now;

    } else {
      _mode = Mode::fix;
      _toValue = _value;
    } // if

  } else if (_stricmp(name, ACTION_ONVALUE) == 0) {
    // save the actions
    _valueAction = value;

  } else if (_stricmp(name, "duration") == 0) {
    _duration = _atotime(value) * 1000;

  } else if (_stricmp(name, "brightness") == 0) {
    _brightness = _atoi(value);
    _brightness = constrain(_brightness, 0, 255);

  } else if (_stricmp(name, "saturation") == 0) {
    _saturation = constrain(_atoi(value), 0, 255);

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


#define MAX_HUE 1536
#define MAX_SATURATION 255
#define MAX_BRIGHTNESS 255

/*
 * Convert hue, saturation and value into a color value 0x00RRGGBB.
 * @param  hue 0 to 1536 (6 * 256).
 * @param   saturation 0...255
 * @param  value (brightness) 0...255
 * 
 * There are 6 segments in the hue wheel based on range 0...3600
 * red, yellow, green, cyan, blue, magenta, (red)
 *   0     256,   512,  768, 1024,    1280, 1536
*/
uint32_t hsvColor(int hue, int saturation, int value)
{
  hue = hue % MAX_HUE;
  int r = 0, g = 0, b = 0;

  int lift = (hue % 256); // 0...255 in every segment

  // calc color by hue in range 0-255 on rgb.
  if (hue < 256) {
    // red to yellow
    r = 255;
    g = lift;

  } else if (hue < 512) {
    // yellow to green
    r = 255 - lift;
    g = 255;

  } else if (hue < 768) {
    // green to cyan
    g = 255;
    b = lift;

  } else if (hue < 1024) {
    // cyan to blue
    g = 255 - lift;
    b = 255;

  } else if (hue < 1280) {
    // blue to magenta
    r = lift;
    b = 255;

  } else {
    // magenta to red
    b = 255 - lift;
    r = 255;
  } // if

  // now map rgb (0..255) to range (lo...hi)
  int hi = value;
  int lo = ((255 - saturation) * value) / 255;
  int delta = hi - lo;

  r = lo + (r * delta) / 255;
  g = lo + (g * delta) / 255;
  b = lo + (b * delta) / 255;

  return ((r << 16) + (g << 8) + b);
}


/*
 * Calculate an color in between 2 colors for fading.
 * @param startColor color used with factor 0.
 * @param endColor  color used with factor 255 and higher.
 * @param factor 0...255
*/
uint32_t fadeColor(uint32_t startColor, uint32_t endColor, int factor)
{
  uint32_t nextColor;

  if (factor <= 0) {
    nextColor = startColor;

  } else if (factor >= 255) {
    nextColor = endColor;

  } else {
    int v0, v1;
    int r, g, b;

    // red
    v0 = (startColor & 0x00FF0000) >> 16;
    v1 = (endColor & 0x00FF0000) >> 16;
    r = v0 + (v1 - v0) * factor / 255;
    r = constrain(r, 0, 255);

    // green
    v0 = (startColor & 0x0000FF00) >> 8;
    v1 = (endColor & 0x0000FF00) >> 8;
    g = v0 + (v1 - v0) * factor / 255;
    g = constrain(g, 0, 255);

    // blue
    v0 = (startColor & 0x000000FF);
    v1 = (endColor & 0x000000FF);
    b = v0 + (v1 - v0) * factor / 255;
    b = constrain(b, 0, 255);

    nextColor = ((r << 16) + (g << 8) + b);
  } // if
  return (nextColor);
}


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void ColorElement::loop()
{
  // dynamic color patterns
  unsigned long now = millis(); // current (relative) time in msecs.
  uint32_t nextValue = _value;

  if ((_mode == Mode::fade) && (_value != _toValue)) {
    unsigned long d = now - _startTime; // duration up to now
    if (d >= _duration) {
      nextValue = _toValue;
    } else {
      int p = (d * 255) / _duration; // percentage of fade transition in / 0..255
      nextValue = fadeColor(_fromValue, _toValue, p);
    }

  } else if ((_mode == Mode::fix) && (_value != _toValue)) {
    nextValue = _toValue;

  } else if (_mode == Mode::pulse) {
    // brightness 0...255...1 = 256+254 = 510 steps
    int p = (now % _duration) * 510 / _duration;
    if (p > 255) {
      p = 510 - p;
    }
    nextValue = fadeColor(0x00000000, _value, p);

  } else if (_mode == Mode::wheel) {
    int hue = (now % _duration) * MAX_HUE / _duration;
    nextValue = hsvColor(hue, _saturation, _brightness);
  }

  if (nextValue != _value) {
    char sColor[38];
    sprintf(sColor, "x%08x", nextValue);
    _board->dispatch(_valueAction, sColor);
    _value = nextValue;
  }
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void ColorElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  char sColor[38];
  Element::pushState(callback);
  sprintf(sColor, "x%06lx", _value);
  callback(PROP_VALUE, sColor);
  callback("mode", String((int)_mode).c_str());
  callback("duration", String(_duration).c_str());
  callback("brightness", String(_brightness).c_str());
} // pushState()


// End
