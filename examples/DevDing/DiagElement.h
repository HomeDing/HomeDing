/**
 * @file DiagElement.h
 * @brief This element is to be included into a sketch that helps creating new elements and new board adoptions.
 * It adds output to the Serial debug output about several board and i2c characteristics.
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
 * * 30.07.2020 created by Matthias Hertel
 */

#ifndef DIAGELEMENT_H
#define DIAGELEMENT_H

/**
 * @brief DiagElement implements...
 * @details
@verbatim

The DiagElement can ...

@endverbatim
 */

class DiagElement : public Element
{
public:
  /**
   * @brief Factory function to create a DiagElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Construct a new DiagElement
   */
  DiagElement();

  /**
   * @brief initialize a new Element.
   * @param board The board reference.
   */
  virtual void init(Board *board);

  /**
   * @brief Set a parameter or property to a new value or start an action.
   * @param name Name of property.
   * @param value Value of property.
   * @return true when property could be changed and the corresponding action
   * could be executed.
   */
  virtual bool set(const char *name, const char *value);

  /**
   * @brief Activate the Element.
   * @return true when the Element could be activated.
   * @return false when parameters are not usable.
   */
  virtual void start();

  /**
   * @brief Give some processing time to the timer to check for next action.
   */
  virtual void loop();

  /**
   * @brief stop all activities and go inactive.
   */
  // virtual void term();

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback);

private:
  /**
   * @brief The actual value.
   */
  int _value;


  /**
   * @brief The _xAction holds the actions that is submitted when ...
   */
  String _xAction;
};

#endif