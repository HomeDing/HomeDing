/**
 * @file RadioElement.h
 * @brief Element Template class.
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
 * * 30.07.2018 created by Matthias Hertel
 */

#ifndef RADIOELEMENT_H
#define RADIOELEMENT_H

#include <HomeDing.h>

/**
 * @brief RadioElement implements...
 * @details
@verbatim

The RadioElement can ...

@endverbatim
 */

class RadioElement : public Element
{
public:
  /**
   * @brief Factory function to create a RadioElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  /**
   * @brief Construct a new RadioElement
   */
  RadioElement();

  /**
   * @brief initialize a new Element.
   * @param board The board reference.
   */
  // virtual void init(Board *board);

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
   * @brief The actual volumne.
   */
  int _volume = 0;

  /**
   * @brief The actual frequency.
   */
  int _freq = 0;

  bool _mute = false;
  
  /**
   * @brief The actions that are submitted when a station name is recognized through RDS...
   */
  String _volumeAction;
  String _frequencyAction;
  String _stationAction;
  String _rdsTextAction;
  String _rssiAction;

  int _checkInfo = 3;

};

#endif