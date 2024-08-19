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
int ListUtils::length(const char *list) {
  int cnt = 0;

  if (list) {
    cnt++;
    const char *p = list;
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
int ListUtils::length(String &list) {
  return (length(list.c_str()));
}


/**
 * @brief Return the item of the list at the index position.
 * @param list A comma separated lists of strings.
 * @param index The index (position) of the array element to be returned.
 * @return String The item as String extracted from the list.
 */
String ListUtils::at(String list, int index) {
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
  int n = 0;
  const char *p = list;
  const char *pStart = p;
  const char *pEnd = p - 1;

  while (n <= index) {
    if ((*p == LISTSEPARATOR) || (!*p)) {
      pStart = pEnd + 1;
      pEnd = p;
      n++;
    }
    if (!*p) {
      break;
    }
    p++;
  }  // while

  if (n > index) {
    int len = pEnd - pStart;
    char buf[len + 1];
    memcpy(buf, pStart, len);
    buf[len] = NUL;
    ret = buf;
  }

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
