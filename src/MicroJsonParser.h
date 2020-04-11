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
 */


#ifndef MICROJSON_H
#define MICROJSON_H

#define MICROJSON_PATH_SEPARATOR '/'
#define MICROJSON_PATH_SEPARATOR_S "/"

/**
 * @brief Signature of the callback function.
 */
typedef std::function<void(int level, char *path, char *name, char *value)>
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
   * @brief Parse a JSON String
   * @param s The String containing a JSON object.
   */
  void parse(const char *s);

  /**
   * @brief Parse JSON from a file.
   * @param s The File Name.
   */
  void parseFile(const char *fName);

protected:
  /**
   * @brief get the next character from the input and parse.
   * @param ch input character
   */
  void parse(char ch);

  int _state;
  int __level;

  char _path[64];
  int  _index[12]; // -1 for object
  char _name[16];
  char _value[200];
  char _esc[8];

  MicroJsonCallbackFn _callbackFn;
};

#endif

// end.
