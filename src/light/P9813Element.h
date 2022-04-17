/**
 * @file P9813Element.h
 * @brief The P8913Element implements communication with P9813 LED driver chips to implement RGB lights.
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
 * Changelog:
 * * 12.01.2021 created by Matthias Hertel
 * * implementing a single chip only. chained chips not yet supported. 
 */

#ifndef P9813ELEMENT_H
#define P9813ELEMENT_H

#include <HomeDing.h>
#include <light/LightElement.h>

/**
 * @brief The P8913Element implements communication with P9813 LED driver chips to implement RGB lights.
 */

class P9813Element : public LightElement
{
public:
  /**
   * @brief Factory function to create a P9813Element.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;


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
   * @brief direct function to show a color and brightness.
   */
  virtual void show(uint32_t color, int brightness);

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback) override;

private:
  /**
   * @brief Send a 32-bit data set for P9813, can be used for start, stop and data.
   */
  void sendPixelData(const uint32_t data);

};

#ifdef HOMEDING_REGISTER
// Register the P9813Element onto the ElementRegistry.
bool P9813Element::registered =
    ElementRegistry::registerElement("p9813", P9813Element::create);
#endif

#endif