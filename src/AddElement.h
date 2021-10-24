/**
 * @file AddElement.h
 * @brief Element that adds given input values.
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
 * * 13.10.2021 created by Matthias Hertel
 */

#ifndef ADDELEMENT_H
#define ADDELEMENT_H

#include <HomeDing.h>
#include <CalcElement.h>

class AddElement : public CalcElement {
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
   * @brief Activate the Element.
   */
  virtual void start();


protected:
  /**
   * @brief Do the calculation on all input values.
   */
  virtual void _calc() override;
};

/* ===== Register the Element ===== */

#ifdef HOMEDING_REGISTER
// Register the AddElement in the ElementRegistry.
bool AddElement::registered =
    ElementRegistry::registerElement("add", AddElement::create);
#endif


#endif