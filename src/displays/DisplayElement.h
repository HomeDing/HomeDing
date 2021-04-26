/**
 * @file DisplayElement.h
 * @brief Implementation of the base function for a DisplayXXXElement.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license.
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:
 * * 29.08.2020 created by Matthias Hertel
 */

#ifndef DISPLAYELEMENT_H
#define DISPLAYELEMENT_H

#include <functional>

class DisplayElement : public Element
{
public:
  /* ===== Element functions ===== */

  /**
   * @brief Constructor of a new DisplayElement.
   */
  DisplayElement();

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override;

  /**
   * @brief Activate the Element.
   * @return true when the Element could be activated.
   * @return false when parameters are not usable.
   */
  virtual void start() override;

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback) override;

protected:
  // common properties for display elements

  /** Pin to reset the display chip */
  int _resetpin = -1;

  /** I2C address */
  int _address = 0x3c;

  /** width of display */
  int _width = 128;

  /** height of display */
  int _height = 64;

  /** brightness of display */
  int _brightness = 128;

private:
  void _reset();

};

// This is a base class that will not be registered

#endif // DISPLAYELEMENT_H