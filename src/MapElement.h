/**
 * @file MapElement.h
 * @brief Map incoming values to other values specified by a set of range rules.
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
 * * 29.03.2020 created by Matthias Hertel
 * * 06.06.2021 full implementation of rules.
 */

/**
 * @brief MapElement implements...
 * @details
@verbatim

The MapElement can ...

@endverbatim
 */

class MapElement : public Element
{
public:
  /**
   * @brief Factory function to create a MapElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

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
   * @brief Give some processing time to the timer to check for next action.
   */
  virtual void loop() override;

  /**
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback);

private:
  /**
   * @brief The actual outgoing(mapped) value.
   */
  String _value;

  int _currentMapIndex = -1;
  bool _needUpdate = false;

  bool _isStringType;

  // send out mapped value each time the same value is received.
  bool _resend = true;

  // rules
  ArrayString _mMin;     // lower bound of the range (inclusive)
  ArrayString _mMax;     // higher bound of the range (inclusive)
  ArrayString _mValue;   // new value when rule is choosen
  ArrayString _mActions; // actions when rule is choosen

  /**
   * @brief The _valueAction holds the actions that is submitted when ...
   */
  String _valueAction;

  void _mapValue(const char *value);
};

/* ===== Register the Element ===== */

#ifdef HOMEDING_REGISTER
// Register the MapElement onto the ElementRegistry.
bool MapElement::registered =
    ElementRegistry::registerElement("map", MapElement::create);
#endif
