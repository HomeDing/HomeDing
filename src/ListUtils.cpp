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
 * Changelog: see WireUtils.h
 */

#include "ListUtils.h"

#include <string.h>

#define LISTSEPARATOR ','
#define NUL '\0'


#if !defined(_stricmp)
int _stricmp(const char *str1, const char *str2) {
  char c1, c2;

  do {
    c1 = *str1++;
    c2 = *str2++;

    // convert characters from upper to to lowercase
    if ((c1 >= 'A') && (c1 <= 'Z'))
      c1 += 'a' - 'A';
    if ((c2 >= 'A') && (c2 <= 'Z'))
      c2 += 'a' - 'A';
  } while ((c1) && (c1 == c2));
  return (int)(c1 - c2);
}  // _stricmp
#endif


/**
 * @brief calculate length of the list by counting the separators.
 * @param list A comma separated lists of strings.
 * @return int Number of items in the list.
 */
unsigned int ListUtils::length(const char *list) {
  unsigned int cnt = 0;

  if ((list) && (*list)) {
    const char *p = list;
    cnt++;
    while (*p) {
      if (*p++ == LISTSEPARATOR) { cnt++; }
    }
  }
  return (cnt);
}  // length()

/**
 * @brief calculate length of the list by counting the separators.
 * @param list A comma separated lists of strings.
 * @return int Number of items in the list.
 */
unsigned int ListUtils::length(String &list) {
  return (length(list.c_str()));
}


/**
 * @brief Return the item of the list at the index position.
 * @param list A comma separated lists of strings.
 * @param index The index (position) of the array element to be returned.
 * @return String The item as String extracted from the list.
 */
String ListUtils::at(const String &list, int index) {
  return (at(list.c_str(), index));
}


/**
 * @brief Return the item of the list at the index position.
 * @param list A comma separated lists of strings.
 * @param index The index (position) of the array element to be returned.
 * @return String The item as String extracted from the list.
 */
String ListUtils::at(const char *list, int index) {
  String ret;

  if (!list || (index < 0)) {
    return ret;
  }

  const char *start = list;
  int currentIndex = 0;

  for (const char *p = list;; p++) {
    if ((*p == LISTSEPARATOR) || (*p == NUL)) {
      if (currentIndex == index) {
        unsigned int len = (unsigned int)(p - start);

        ret.reserve(len);
        ret.concat(start, len);
        return (ret);
      }
      if (*p == NUL) {
        break;
      }
      currentIndex++;
      start = p + 1;
    }
  }  // for

  return (ret);
}  // at()


/**
 * @brief Search for a item in the list
 * @param list A comma separated lists of strings.
 * @param item The item to be found in the list.
 * @return int The first index of the item in the list. -1 if not found.
 */
int ListUtils::indexOf(const char *list, const char *item) {
  int n = 0;

  do {
    String m = ListUtils::at(list, n);
    if (m.isEmpty()) return (-1);
    if (_stricmp(item, m.c_str()) == 0)
      return (n);
    n++;
  } while (1);
}

// End.
