/**
 * @file SceneElement.h
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 * 
 * Changelog:
 * * 05.07.2021 created by Matthias Hertel
 * * 19.01.2022 finalized including delay
 */

#pragma once

/**
 * @brief 
 * The scene element defines a series of actions executed by a single triggering action.
 */
class SceneElement : public Element
{
public:
  /**
   * @brief Factory function to create a SceneElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Construct a new SceneElement
   */
  SceneElement();

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

private:
  /** The next action to be sent. Will be set to -1 after all actions are done. */
  int _count;

  /**
   * @brief The delay between executing the steps in msec
   */
  unsigned long _delay;

  /**
   * @brief The time for the next step.
   */
  unsigned long _nextStep;

  /**
   * @brief The _steps hold a list of actions.
   */
  std::vector<String> _steps;
};

/* ===== Register the Element ===== */

#ifdef HOMEDING_REGISTER
// Register the SceneElement onto the ElementRegistry.
bool SceneElement::registered =
    ElementRegistry::registerElement("scene", SceneElement::create);
#endif
