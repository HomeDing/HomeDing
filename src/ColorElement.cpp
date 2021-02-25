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

#define MAX_HUE 1536
#define MAX_SATURATION 255
#define MAX_BRIGHTNESS 255

/*
 * Convert hue, saturation and lightness into a color value 0x00RRGGBB
 * using interger based calculations. The input values must be  in range.
 * @brief
 * There are 6 segments in the hue wheel based on range 0 to 1535
 * red, yellow, green, cyan, blue, magenta, (red)
 *   0     256,   512,  768, 1024,    1280, 1536
 * @param  hue 0...1535 (6 * 256).
 * @param  saturation 0...255
 * @param  lightness 0...255
*/
uint32_t hslColor(int hue, int saturation, int lightness)
{
  hue = hue % MAX_HUE;

  // resulting colors
  int r = 0, g = 0, b = 0;

  // A certain part of the full color is used for resulting color,
  // with maximum at lightness = 50 and is used as a factor to scale full color.
  // chroma = 0...255;
  int chroma;

  // There is a minimum for all base colors to match the lightness
  int minimum;

  // calc full color by hue in range 0-255 on rgb.
  int offset = (hue % 256); // 0...255 in every segment
  if (hue < 256) {
    // red to yellow
    r = 255;
    g = offset;

  } else if (hue < 512) {
    // yellow to green
    r = 255 - offset;
    g = 255;

  } else if (hue < 768) {
    // green to cyan
    g = 255;
    b = offset;

  } else if (hue < 1024) {
    // cyan to blue
    g = 255 - offset;
    b = 255;

  } else if (hue < 1280) {
    // blue to magenta
    r = offset;
    b = 255;

  } else {
    // magenta to red
    b = 255 - offset;
    r = 255;
  } // if

  if (lightness == 0) {
    chroma = 0;
    minimum = 0;

  } else if (lightness <= 127) {
    chroma = (2 * (lightness) * (saturation)) / 255 + 1;
    minimum = 0;

  } else {
    chroma = (2 * (255 - lightness) * (saturation)) / 255;
    minimum = lightness - chroma / 2;
  }

  // Serial.printf(" chroma =%3d\n", chroma);
  // Serial.printf(" minimum=%3d\n", minimum);

  r = minimum + (r * chroma) / 255;
  g = minimum + (g * chroma) / 255;
  b = minimum + (b * chroma) / 255;

  return ((r << 16) + (g << 8) + b);
} // hslColor()


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
      // setup fading range and time
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
    // duration for wheel, pulse and fade effect
    _duration = _atotime(value) * 1000;

  } else if (_stricmp(name, "saturation") == 0) {
    // lightness for wheel effect
    _saturation = constrain(_atoi(value), 0, 255);

  } else if (_stricmp(name, "lightness") == 0) {
    // lightness for wheel effect
    _lightness = _atoi(value);
    _lightness = constrain(_lightness, 0, 255);

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


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
    int w, r, g, b;

    // white
    v0 = (startColor & 0xFF000000) >> 24;
    v1 = (endColor & 0xFF000000) >> 24;
    w = v0 + (v1 - v0) * factor / 255;
    w = constrain(w, 0, 255);

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

    nextColor = ((w << 24) + (r << 16) + (g << 8) + b);
  } // if
  return (nextColor);
}


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void ColorElement::loop()
{
  static unsigned long lastTime = 0;
  // dynamic color patterns
  unsigned long now = millis(); // current (relative) time in msecs.
  uint32_t nextValue = _value;

  if ((_mode == Mode::fix) && (_value != _toValue)) {
    nextValue = _toValue;

  } else if (now < lastTime + 100) {
    // no new automation step more often than 10 times per second.
    return;

  } else if ((_mode == Mode::fade) && (_value != _toValue)) {
    unsigned long d = now - _startTime; // duration up to now
    if (d >= _duration) {
      nextValue = _toValue;
    } else {
      int p = (d * 255) / _duration; // percentage of fade transition in / 0..255
      nextValue = fadeColor(_fromValue, _toValue, p);
    }

  } else if (_mode == Mode::pulse) {
    // pulse brightness 0...255...1 = 256+254 = 510 steps
    int bright = (now % _duration) * 510 / _duration;
    if (bright > 255) {
      bright = 510 - bright;
    }
    nextValue = fadeColor(0x00000000, _toValue, bright);

  } else if (_mode == Mode::wheel) {
    int hue = (now % _duration) * MAX_HUE / _duration;
    nextValue = hslColor(hue, _saturation, _lightness);
  }

  if (nextValue != _value) {
    char sColor[38];
    sprintf(sColor, "x%08x", nextValue);
    _board->dispatch(_valueAction, sColor);
    _value = nextValue;
  }
  lastTime = now;
} // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void ColorElement::pushState(
    std::function<void(const char *pName, const char *eValue)> callback)
{
  char sColor[38];
  Element::pushState(callback);
  callback("mode", String((int)_mode).c_str());

  if (_mode != Mode::wheel) {
    sprintf(sColor, "x%08x", _toValue); // do not report fading and interim colors
    callback(PROP_VALUE, sColor);
  }

  callback("duration", String(_duration).c_str());
  callback("saturation", String(_saturation).c_str());
  callback("lightness", String(_lightness).c_str());
  // callback("brightness", String(_brightness).c_str());
} // pushState()


// End
