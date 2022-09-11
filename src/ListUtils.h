/**
 * @file ListUtils.h
 *
 * @brief List Utils are some helpful functions to use strings as comma separated lists of strings.
 * Any compare functions are implemented case insensitive.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * 02.07.2022 created.
 */


#pragma once

#include <Arduino.h>

class ListUtils {

public:
  /**
   * @brief calculate length of the list by counting the separators.
   * @param list A comma separated lists of strings.
   * @return int Number of items in the list.
   */
  static int length(const char *list);

  /**
   * @brief Return the item of the list at the index position.
   * @param list A comma separated lists of strings.
   * @param index The index (position) of the array element to be returned.
   * @return String The item as String extracted from the list.
   */
  static String at(String list, int index);

  /**
   * @brief return the item of the list at the index position
   * @param list A comma separated lists of strings.
   * @param index The index (position) of the array element to be returned.
   * @return String The item as String extracted from the list.
   */
  static String at(const char *list, int index);


  /**
   * @brief Search for a item in the list
   * @param list A comma separated lists of strings.
   * @param item The item to be found in the list.
   * @return int The first index of the item in the list. -1 if not found.
   */
  static int indexOf(const char *list, const char *item);
};
