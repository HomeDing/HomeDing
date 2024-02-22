/**
 * @file OrElement.h
 * @brief Element that combines boolean input values using the OR and optional NOT operator and sends actions.
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

#pragma once

#include <CalcElement.h>

class OrElement : public CalcElement {
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

protected:
  /**
   * @brief Do the calculation on all input values.
   */
  virtual void _calc() override;
};

/* ===== Register the Element ===== */

#ifdef HOMEDING_REGISTER
// Register the OrElement onto the ElementRegistry.
bool OrElement::registered =
    ElementRegistry::registerElement("or", OrElement::create);
#endif
