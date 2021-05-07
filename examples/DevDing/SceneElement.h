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
 * * 30.07.2018 created by Matthias Hertel
 */

#ifndef SCENEELEMENT_H
#define SCENEELEMENT_H

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


  /**
   * @brief stop all activities and go inactive.
   */
  virtual void term() override;

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback) override;

private:
  /**
   * @brief The actual value.
   */
  String _value;

  /**
   * @brief The actual value.
   */
  bool _newValue;

  /**
   * @brief The _actions hold a list of actions.
   */
  std::vector<String> _actions;
};

#endif