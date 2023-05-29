/**
 * @file RTCVariables.h
 * @author Matthias Hertel (https://www.mathertel.de)
 *
 * @brief This class provides some functions to access the variables in RTC Memory.
 * 
 * The Reset counter supports a counter in the range 0..255.
 * The State String supports a string memory up to 200 chars length (199 chars + NUL).
 * 
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 * 
 * Changelog:
 * * 21.04.2018 creation
 */

class RTCVariables
{
public:
  /**
   * @brief Set the Reset Counter object
   * @param cnt The new counter value.
   * @return int The new counter value. 
   */
  static int setResetCounter(int cnt);

  /**
   * @brief Retrieve the reset counter value.
   * @return int The counter value. 
   */
  static int getResetCounter();

  /**
   * @brief Set the State string.
   * @param state The new state.
   */
  static void setStateString(String &state);

  /**
   * @brief Get the State String object
   * @return String The actual state string
   */
  static String getStateString();
}; // class

// end.
