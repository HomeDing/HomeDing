/**
 * @file StateElement.h
 * @brief The StateElement implements the configuration for saving the state of a device and its elements.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:
 * * 12.08.2023 created by Matthias Hertel
 */


/// @brief The StateElement implements the configuration for saving the state of a device and its elements.
class StateElement : public Element {
public:
  /// @brief Factory function to create a StateElement.
  /// @return Element*
  static Element *create();

  /// @brief static variable to ensure registering in static init phase.
  static bool registered;


  /* ===== Element functions ===== */


  /// @brief Set a parameter or property to a new value or start an action.
  /// @param name Name of property.
  /// @param value Value of property.
  /// @return true when property could be changed or the action could be executed.
  virtual bool set(const char *name, const char *value) override;


  /// @brief trigger saving the current state of the device.
  virtual void loop() override;

private:
  unsigned long _saveDelay;
};
