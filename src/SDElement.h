/**
 * @file SDElement.h
 * @brief Supporting a sd card reader as file storage and Element for the HomeDing Library.
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:
 * * 25.02.2023 created by Matthias Hertel
 */

#pragma once

/**
 * @brief
 */
class SDElement : public Element {
public:
  /**
   * @brief Factory function to create a SDElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Construct a new SDElement
   */
  SDElement();

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

private:

  // reuse spi configuration from board
  // pin settings
  int _csPin = -1;  // pin used for mmc data 0
};

/* ===== Register the Element ===== */

#ifdef HOMEDING_REGISTER
// Register the SDElement in the ElementRegistry.
bool SDElement::registered =
  ElementRegistry::registerElement("sd", SDElement::create);
#endif
