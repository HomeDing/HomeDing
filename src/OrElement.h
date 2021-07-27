/**
 * @file OrElement.h
 * @brief Logical Element that combines boolean input values using the AND and optional NOT operator and sends actions.
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
 * * 27.07.2021 created by Matthias Hertel
 */

#ifndef ORELEMENT_H
#define ORELEMENT_H

#include <HomeDing.h>

#include <LogicElement.h>

class OrElement : public LogicElement {
public:
  /**
   * @brief Factory function to create a OrElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Give some processing time to the timer to check for next action.
   */
  virtual void loop() override;
};

/* ===== Register the Element ===== */

#ifdef HOMEDING_REGISTER
// Register the OrElement onto the ElementRegistry.
bool OrElement::registered =
    ElementRegistry::registerElement("or", OrElement::create);
#endif

#endif