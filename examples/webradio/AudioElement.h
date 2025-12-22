/**
 * @file AudioElement.h
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:
 * * 30.01.2022 created by Matthias Hertel
 */

#pragma once

/**
 * @brief
 */
class AudioElement : public Element {
public:
  /**
   * @brief Factory function to create a AudioElement.
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
   * @brief Activate the Element.
   * @return true when the Element could be activated.
   * @return false when parameters are not usable.
   */
  virtual void start() override;

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

  String _url;      ///< Stream URL
  String _station;  ///< station information in stream
  String _bitrate;  ///< bitrate information in stream
  String _title;    ///< title information in stream
  int _volume;      ///< volume
  int _balance;

  int _low;
  int _mid;
  int _high;
  
  bool _mono;

  String _onStation;
  String _onTitle;

private:
  int _i2s_bclk;  // bit clock Pin
  int _i2s_lrc;   // left/right clock Pin
  int _i2s_dout;  // Data Out Pin
};
