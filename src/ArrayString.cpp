/**
 * @file ArrayString.cpp
 *
 * @brief Array of Strings, stored by using minimal heap consumption in c-style.
 * With operators for array and queues.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * Changelog:see ArrayString.h
 */


#include <Arduino.h>
#include <ArrayString.h>

#define CTRACE(...) // Serial.printf(__VA_ARGS__)

void ArrayString::_createCapacity(uint16_t num) {
  CTRACE("_createCapacity(%d)\n", num);

  uint16_t newReserved = (num + 7) & 0xFFF8;
  size_t newSize = newReserved * sizeof(char *);

  if (_capacity == 0) {
    // initial allocation
    array = (char **)malloc(newSize);
    memset(array, 0, newSize);
    _capacity = newReserved;

  } else if (num > _capacity) {
    // increase capacity
    array = (char **)realloc(array, newSize);
    memset(array + _capacity, 0, newSize - (_capacity * sizeof(char *)));
    _capacity = newReserved;
  }
}  // _createCapacity()


void ArrayString::reserve(uint16_t num) {
  _createCapacity(num);
};


/// @brief return String by index
String ArrayString::at(uint16_t index) {
  String ret;

  if (index < _used) {
    ret = array[index];
  }
  return (ret);
}


/// @brief add a String at the given position
void ArrayString::setAt(uint16_t index, const char *s) {
  CTRACE("setAt[%d]=<%s>\n", index, s);
  _createCapacity(index + 1);
  if (array[index]) {
    free(array[index]);
    array[index] = nullptr;
  }
  if (s && *s) {
    unsigned int len = strlen(s) + 1;
    char *mem = (char *)malloc(len);
    memcpy(mem, s, len);
    array[index] = mem;
  }
  if (index >= _used) { _used = index + 1; }
  // dump();
};


void ArrayString::split(const char *s, char delim) {
  CTRACE("AS: split <%s>\n", s);
  clear();
  uint16_t cnt = 0;

  if (s && *s) {
    // calculate number of items
    cnt++;
    const char *p = s;
    while (*p) {
      if (*p++ == delim) { cnt++; }
    }
    _createCapacity(cnt);

    const char *pStart = s;
    while ((*pStart) && (_used < cnt)) {
      const char *pEnd = pStart;
      while (*pEnd && *pEnd != delim) { pEnd++; }

      if (array[_used]) free(array[_used]);

      // copy pStart .. pEnd-1
      uint16_t rawlen = pEnd - pStart;  // len without trailing '\0'

      char *mem = (char *)malloc(rawlen + 1);
      memcpy(mem, pStart, rawlen);
      mem[rawlen] = '\0';

      array[_used++] = mem;

      pStart = pEnd + 1;
    }
  }  // if
  // dump();
}


/// @brief Create a string with all array items separatend by delim.
String ArrayString::concat(char delim) {
  int size = 0;
  String res;

  for (int n = 0; n < _used; n++) {
    size += strlen(array[n]) + 1;
  }

  res.reserve(size + 2);
  for (int n = 0; n < _used; n++) {
    if (n > 0) {
      res.concat(delim);
    }
    res.concat(array[n]);
  }
  return (res);
}  // concat


// remove first string from array and return it
String ArrayString::pop() {
  String ret;

  if (_used > 0) {
    ret = array[0];
    free(array[0]);

    // shift all pointers
    memmove(array, array + 1, (_capacity - 1) * sizeof(char *));
    _used = _used - 1;
    array[_capacity - 1] = nullptr;
  }
  return (ret);
}


// remove a String Entry and return old entry as String
String ArrayString::remove(uint16_t n) {
  String ret;

  if (n < _used) {
    ret = array[n];
    free(array[n]);

    for (int n = 0; n < _capacity - 1; n++) {
      array[n] = array[n + 1];
    }
    _used = _used - 1;
    array[_capacity - 1] = nullptr;
  }
  return (ret);
}  // remove()


// deallocate all.
void ArrayString::clear() {
  for (int n = 0; n < _used; n++) {
    free(array[n]);
  }
  if (array) free(array);
  _capacity = 0;
  _used = 0;
  array = nullptr;
}


/// @brief Find an entry starting with the given string.
/// @param s String to be found.
/// @return index of first found entry or -1 for not found.
int16_t ArrayString::findStartWith(const String &sFind) {
  CTRACE("AS: findStartWith(%s)\n", sFind.c_str());

  const char *s = sFind.c_str();
  for (int n = 0; n < _used; n++) {
    CTRACE("  : (%s)=%08lx\n", array[n], strstr(array[n], s));
    if (strstr(array[n], s)) { return (n); }
  }
  return (-1);
}  // findStartWith()


void ArrayString::dump() {
  CTRACE("_used: %d, _capacity: %d\n", _used, _capacity);
  for (int n = 0; n < _used; n++) {
    CTRACE("%2d:%08x <%s>\n", n, array[n], array[n]);
  }
  CTRACE("\n");
}  // dump()

// End
