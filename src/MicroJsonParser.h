/**
 * @file MicroJsonParser.h
 * 
 * @brief Streaming JSON parser with minimal memory impact.
 * This class is part of the HomeDing Library.
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
 * Changelog:
 * * created by Matthias Hertel
 * * 21.01.2020 supporting arrays
 * * 11.04.2020 better supporting arrays
 * * 18.04.2020 correct handling empty objects a.k.a '{}'
 * * 18.04.2020 reduce memory footprint
 * * 27.06.2020 enable parsing partial JSON in junks
 * * 16.05.2021 use strlcat.
 */


#pragma once

#include <functional>
#include <FS.h>

#define MICROJSON_PATH_SEPARATOR '/'
#define MICROJSON_PATH_SEPARATOR_S "/"

/**
 * @brief Signature of the callback function.
 */
typedef std::function<void(int level, char *path, char *value)>
    MicroJsonCallbackFn;

class MicroJson
{
public:
  /**
   * @brief Construct a new Micro Json object
   * and register a callback function.
   * @param callback
   */
  MicroJson(MicroJsonCallbackFn callback);

  /**
   * @brief Initialize the JSON parser
   */
  void init();

  /**
   * @brief Parse a JSON String.
   * This function can be called multiple times in a row when a partial JSON needs to be processed in multiple steps. 
   * @param s The String containing a JSON object.
   */
  void parse(const char *s);

  /**
   * @brief Parse JSON from a file.
   * @param s The File Name.
   */
  void parseFile(FS *fs, const char *fName);

protected:
  /**
   * @brief get the next character from the input and parse.
   * @param ch input character
   * @return true, when character was parsed. on false: enter same char again.
   */
  bool parseChar(char ch);

  /**
   * @brief parset a sequence of chars.
   * @param s input characters
   */
  void parseChar(const char *s);

  void _sendPath(int _level, char *path);
  void _sendValue(int _level, char *path, char *name, char *value);

  int _state;
  int __level;

  char _path[128];
  int  _index[12]; // -1 for object
  char _name[64];
  char _value[200];
  char _esc[8];

  MicroJsonCallbackFn _callbackFn;
};

// end.
