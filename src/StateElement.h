/**
 * @file StateElement.h
 * @author Matthias Hertel (https://www.mathertel.de)
 * 
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 * 
 * @brief State Element class
 * 
 * This class is a interface definition class for State Elements to save state information from Elements.
 * There is maximal 1 State Element registered in the Base class at a time.
 * Configuration is done via the Element configurations.
 * 
 * Changelog:
 * * 18.04.2021 created by Matthias Hertel
 */

#pragma once

/**
 * @brief The StateElement class defines the extra virtual functions for any State Element. 
 */
class StateElement : public Element {
public:
  /**
   * @brief The save function allows saving state information specific for an Element.
   * 
   * @param element The Element that needs the state support.
   * @param key Then key of a state variable of the element.
   * @param value The value of a state variable of the element.
   */
  virtual void save(Element *element, const char *key, const char *value);

  /**
   * @brief Load all state information and dispatch them as actions.
   */
  virtual void load();
};
