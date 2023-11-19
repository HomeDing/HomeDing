/**
 * @file SDMMCElement.h
 * @brief Supporting a mmc sd card reader as file storage and Element for the HomeDing Library.
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

#if defined(ESP32) && (! CONFIG_IDF_TARGET_ESP32C3)

/**
 * @brief
 */
class SDMMCElement : public Element {
public:
  /**
   * @brief Factory function to create a SDMMCElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Construct a new SDMMCElement
   */
  SDMMCElement();

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

  // pin settings

#if defined(CONFIG_IDF_TARGET_ESP32S3)
  int _mmc_d0_pin = -1;   // pin used for mmc data 0
  int _mmc_d1_pin = -1;   // pin used for mmc data 1
  int _mmc_d2_pin = -1;   // pin used for mmc data 2
  int _mmc_d3_pin = -1;   // pin used for mmc data 3
  int _mmc_clk_pin = -1;  // pin used for mmc spi clock
  int _mmc_cmd_pin = -1;  // pin used for mmc spi command
#endif
};

/* ===== Register the Element ===== */

#ifdef HOMEDING_REGISTER
// Register the SDMMCElement in the ElementRegistry.
bool SDMMCElement::registered =
  ElementRegistry::registerElement("sdmmc", SDMMCElement::create);
 #endif

#endif
