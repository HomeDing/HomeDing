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

#include <light/ColorElement.h>

#if !defined(TRACE)
#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)
#endif


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new ColorElement
 * @return ColorElement* created element
 */
Element *ColorElement::create() {
  return (new ColorElement());
}  // create()


/* ===== Element functions ===== */

#define MAX_HUE 1536

/*
 * Convert hue into a color value 0x00rrggbb
 * using interger based calculations. The hue value must be in range 0 to 1535.
 *
 * @brief
 * There are 6 segments in the hue wheel:
 * red, yellow, green, cyan, blue, magenta, (red)
 *   0     256,   512,  768, 1024,    1280, 1536
 * @param  hue 0...1535 (6 * 256).
 */
// uint32_t hslColor(int hue, int saturation = 255, int lightness = 128) {
uint32_t ColorElement::hslColor(int hue) {
  hue = hue % MAX_HUE;

  // resulting colors
  int r = 0, g = 0, b = 0;

  // calc full color by hue in range 0-255 on rgb.
  int offset = (hue % 256);  // 0...255 in every segment
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
  }  // if

  return ((r << 16) + (g << 8) + b);
}  // hslColor()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool ColorElement::set(const char *name, const char *value) {
  TRACE("set(%s=%s)", name, value);

  bool ret = true;
  unsigned long now = millis();

  if (name == HomeDing::Action::Value) {
    // set next value of color
    uint32_t colorValue = _atoColor(value);

    if (_mode == Mode::fade) {
      // setup fading range and time
      _fromValue = _value;
      _fromBrightness = _brightness;
      _startTime = now;
    }
    _toValue = colorValue;
    _toBrightness = _brightness;
    _needValueUpdate = true;

  } else if (_stricmp(name, "brightness") == 0) {
    // set brightness: pass through to light elements
    int b = _atoi(value);
    _fromBrightness = _brightness;
    _toBrightness = constrain(b, 0, 100);
    _startTime = now;
    _needBrightnessUpdate = true;


  } else if (_stricmp(name, "mode") == 0) {
    Mode m = (Mode)ListUtils::indexOf("fix,fade,wheel,pulse", value);
    if ((m >= Mode::_min) && (m <= Mode::_max)) {
      _mode = m;
      _toValue = _value;
      _startTime = now;
    }  // if
    _needValueUpdate = true;
    _needBrightnessUpdate = true;

  } else if (_stricmp(name, "duration") == 0) {
    // duration for wheel, pulse and fade effect
    _duration = _scanDuration(value);

  } else if (_stristartswith(name, "connect[")) {
    // save ID to
    _lightElementIDs.push(value);
    TRACE("  con(%s, %d)", value, _lightElementIDs.size());

  } else if (name == HomeDing::Action::OnValue) {
    // save the actions
    _valueAction = value;

  } else if (_stricmp(name, "onbrightness") == 0) {
    // save the actions
    _brightnessAction = value;

  } else {
    ret = Element::set(name, value);
  }  // if

  return (ret);
}  // set()


/**
 * @brief Activate the LightElement.
 */
void ColorElement::start() {
  TRACE("Color::start(%d)", _lightElementIDs.size());
  Element::start();

  uint16_t leSize = _lightElementIDs.size();
  _lightElementsCount = 0;

  _lightElements = (LightElement **)malloc(leSize * sizeof(LightElement *));

  for (int n = 0; n < leSize; n++) {
    // connected elements
    Element *e = _board->findById(_lightElementIDs[n].c_str());
    if (e) {
      TRACE("  add(%s)", e->id);
      _lightElements[_lightElementsCount++] = static_cast<LightElement *>(e);
    }
  }  // for

  _lightElementIDs.clear();
}  // start()


/*
 * Calculate an color in between 2 colors for fading.
 * @param startColor color used with factor 0.
 * @param endColor  color used with factor 255 and higher.
 * @param factor 0...255
 */
uint32_t fadeColor(uint32_t startColor, uint32_t endColor, int factor) {
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
  }  // if
  return (nextColor);
}


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void ColorElement::loop() {
  unsigned long now = millis();  // current (relative) time in msecs.
  uint32_t nextValue = _toValue;
  uint16_t nextBrightness = _toBrightness;

  if ((_mode == Mode::fix) && (_value != _toValue)) {
    _needValueUpdate = true;

  // } else if (now < _lastTime + 50) {
  //   // no new automation step more often than 20 times per second.
  //   return;

  } else if ((_mode == Mode::fade) && ((_value != _toValue) || (_brightness != _toBrightness))) {
    unsigned long d = now - _startTime;  // duration up to now
    if (d >= _duration) {
      // nextValue = _toValue;
      // nextBrightness = _toBrightness;

    } else {
      int p = (d * 255) / _duration;  // percentage of fade transition in / 0..255
      nextValue = fadeColor(_fromValue, _toValue, p);
      nextBrightness = _fromBrightness + ((_toBrightness-_fromBrightness) * p / 255);
    }
    _needValueUpdate = true;
    _needBrightnessUpdate = true;

  } else if (_mode == Mode::pulse) {
    // pulse brightness 0...255...1 = 256+254 = 510 steps
    int bright = (now % _duration) * 510 / _duration;
    if (bright > 255) {
      bright = 510 - bright;
    }
    nextValue = fadeColor(0x00000000, _toValue, bright);
    _needValueUpdate = true;

  } else if (_mode == Mode::wheel) {
    int hue = (now % _duration) * MAX_HUE / _duration;
    nextValue = hslColor(hue);
    _needValueUpdate = true;
  }

  if (_needValueUpdate || _needBrightnessUpdate) {
    // send to linked light elements
    for (int n = 0; n < _lightElementsCount; n++) {
      TRACE(" %d %s", n, _lightElements[n]->id);
      _lightElements[n]->setColor(nextValue, nextBrightness);
    }

    // dispatch as action
    if (_needBrightnessUpdate) {
      _board->dispatch(_brightnessAction, nextBrightness);
      _brightness = nextBrightness;
      _needBrightnessUpdate = false;
    }

    if (_needValueUpdate && (!_valueAction.isEmpty())) {
      char sColor[38];
      sprintf(sColor, "x%08lx", nextValue);
      _board->dispatch(_valueAction, sColor);
    }
    _needValueUpdate = false;
    _value = nextValue;
  }
  // _lastTime = now;
}  // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void ColorElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  char sColor[38];
  Element::pushState(callback);
  callback("mode", _printInteger((int)_mode));

  if (_mode != Mode::wheel) {
    sprintf(sColor, "x%08lx", _toValue);  // do not report fading and interim colors
    callback(HomeDing::Action::Value, sColor);
  }

  callback("duration", _printInteger(_duration));
  callback("brightness", _printInteger(_brightness));
}  // pushState()


// End
