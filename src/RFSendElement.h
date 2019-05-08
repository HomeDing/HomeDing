/**
 * @file RFSendElement.h
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
 * * 11.08.2018 using TabRF library
 * * 20.08.2018 TabRF referenced by the Element , no more initialization from external side required.
 */

#ifndef RFSendElement_H
#define RFSendElement_H

/**
 * @brief RFSend Element that produces actions based on timing conditions.
 * @details
@verbatim

The RFSendElement can ...

@endverbatim
 */

class RFSendElement : public Element
{
public:
  /**
   * @brief Factory function to create a RFSendElement.
   * @return Element*
   */
  static Element *create();

  /**
   * @brief static variable to ensure registering in static init phase.
   */
  static bool registered;

  static int _rfpin;

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
   * @brief push the current value of all properties to the callback.
   * @param callback callback function that is used for every property.
   */
  virtual void pushState(
      std::function<void(const char *pName, const char *eValue)> callback);

private:
  /**
   * @brief The actual value.
   */
  String _codeOn;
  String _codeOff;
  bool _lastValue;

};


#ifdef HOMEDING_REGISTER
// Register the RFSendElement onto the ElementRegistry.
bool RFSendElement::registered =
    ElementRegistry::registerElement("rfsend", RFSendElement::create);
#endif


#endif