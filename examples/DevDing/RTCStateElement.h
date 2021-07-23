/**
 * @file RTCStateElement.h
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 * 
 * Changelog:
 * * 30.07.2018 created by Matthias Hertel
 */

#ifndef RTCSTATEEELEMENT_H
#define RTCSTATEEELEMENT_H

#include "StateElement.h"

/**
 * The RTCStateElement implements saving the state of elements into the RTC memory
 * to be picked up after a reset.
 */
class RTCStateElement : public StateElement {
public:
  /**
   * @brief Factory function to create a RTCStateElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;


  /* ===== Element functions ===== */

  /**
   * @brief initialize a new Element.
   * @param board The board reference.
   */
  virtual void init(Board *board) override;

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value) override;

  /**
   * @brief Give some processing time to the timer to check for next action.
   */
  virtual void loop() override;


  /* ===== State Element functions ===== */

  /**
   * @brief The save function allows saving state information specific for an Element.
   * 
   * @param element The Element that needs the state support.
   * @param key Then key of a state variable of the element.
   * @param value The value of a state variable of the element.
   */
  virtual void save(Element *element, const char *key, const char *value) override;

  /**
 * @brief Load all state information from RTC memory and dispatch them as actions.
   */
  virtual void load() override;

private:
  /**
   * @brief Flag true when state was changed and must be saved soon.
   */
  bool _hasChanged;

  /**
   * @brief The list of actual state actions.
   */
  String _stateList;
};

#endif
