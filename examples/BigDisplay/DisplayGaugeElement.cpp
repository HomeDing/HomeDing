/**
 * @file DisplayGaugeElement.cpp
 * @brief Output Element for displaying a gauge with a value on a display.
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
 * Changelog, see DisplayGaugeElement.h.
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "DisplayGaugeElement.h"

#include <gfxDraw.h>
#include <gfxDrawGaugeWidget.h>

// enable TRACE for sending detailed output from this Element
#define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)

/**
 * @brief static factory function to create a new DisplayGaugeElement.
 * @return DisplayGaugeElement* as Element* created element
 */
Element *DisplayGaugeElement::create() {
  return (new DisplayGaugeElement());
}  // create()


DisplayGaugeElement::DisplayGaugeElement() {
  TRACE("DisplayGaugeElement()");

  _gConfig = new gfxDraw::gfxDrawGaugeConfig();

  isOpaque = false;
}  // constructor


/// @brief Set a parameter or property to a new value or start an action.
bool DisplayGaugeElement::set(const char *name, const char *value) {
  bool ret = true;

  if (DisplayOutputElement::set(name, value)) {
    // x, y, w, h

  } else if (!active) {
    // setup parameters:

    if (name == HomeDing::Actions::Min) {
      _gConfig->minValue = strtof(value, nullptr);

    } else if (name == HomeDing::Actions::Max) {
      _gConfig->maxValue = strtof(value, nullptr);

    } else if (strcmp(name, "minangle") == 0) {
      int v = _atoi(value);
      _gConfig->minAngle = constrain(v, 0, 360);

    } else if (strcmp(name, "maxangle") == 0) {
      int v = _atoi(value);
      _gConfig->maxAngle = constrain(v, 0, 360);

    } else if (strcmp(name, "pointer") == 0) {
      _pointer = value;

    } else if (strcmp(name, "segment-radius") == 0) {
      // outer radius of the segments in percentages 1...100
      _gConfig->segmentRadius = _atoi(value);

    } else if (strcmp(name, "segment-width") == 0) {
      // width of the segments in percentages 1...100
      _gConfig->segmentWidth = _atoi(value);

    } else if (strcmp(name, "scale-width") == 0) {
      _gConfig->scaleWidth = _atoi(value);

    } else if (strcmp(name, "scale-steps") == 0) {
      _gConfig->scaleSteps = _atoi(value);

    } else if (_stristartswith(name, "segments[")) {
      size_t indx;
      String iName;
      _scanIndexParam(name, indx, iName);

      if (indx >= _segDefinitions.size()) {
        // add a new segment to vector
        _SEGDEF sd;
        _segDefinitions.push_back(sd);
        if (indx > 0) {
          _segDefinitions[indx].minValue = _segDefinitions[indx - 1].maxValue;
        }
      }

      if (iName.equalsIgnoreCase("min")) {
        _segDefinitions[indx].minValue = strtof(value, nullptr);
      } else if (iName.equalsIgnoreCase("max")) {
        _segDefinitions[indx].maxValue = strtof(value, nullptr);
      } else if (iName.equalsIgnoreCase("color")) {
        _segDefinitions[indx].color = _atoColor(value);
      }

    } else {
      ret = false;
    }
  } else {
    ret = false;
  }  // if
  return (ret);
}  // set()


void DisplayGaugeElement::start() {
  TRACE("start()");
  int16_t size = box.qsize();

  if (size > 24) {
    _gConfig->x = box.x_min;
    _gConfig->y = box.y_min;
    _gConfig->w = size;
    _gConfig->pointerColor = _strokeColor;
    _gConfig->segmentColor = _backgroundColor;

    if (_pointer.length() > 0) {
      _gConfig->pointerPath = _pointer.c_str();
    }

    _gWidget = new gfxDraw::gfxDrawGaugeWidget(_gConfig);
    for (_SEGDEF &seg : _segDefinitions) {
      _gWidget->addSegment(seg.minValue, seg.maxValue, seg.color);
    }
    isOpaque = false;
    DisplayOutputElement::start();
  }
}  // start()


/// @brief Draw this output element.
void DisplayGaugeElement::draw() {
  TRACE("draw()");

  _gWidget->setValue(strtof(_value.c_str(), nullptr));

  DisplayOutputElement::draw();

  HomeDing::displayAdapter->startWrite();
  _gWidget->draw(HomeDing::draw);
  HomeDing::displayAdapter->endWrite();
}  // draw()


// End
