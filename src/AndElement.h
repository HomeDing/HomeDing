/**
 * @file AndElement.h
 * @brief Element that combines boolean input values using the AND and optional NOT operator and sends actions.
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
 * * 17.07.2019 created by Matthias Hertel
 * * 27.07.2021 derive from LogicElement
 * * 27.07.2021 allow up to 8 input values using array syntax: value[0..7] = 0/1
 */

#pragma once

#include <HomeDing.h>

#include <CalcElement.h>

class AndElement : public CalcElement {
public:
  /**
   * @brief Factory function to create a AndElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Construct a new AndElement.
   */
  AndElement();


protected:
  /**
   * @brief Do the calculation on all input values.
   */
  virtual void _calc() override;
};

/* ===== Register the Element ===== */

#ifdef HOMEDING_REGISTER
// Register the AndElement onto the ElementRegistry.
bool AndElement::registered =
    ElementRegistry::registerElement("and", AndElement::create);
#endif
