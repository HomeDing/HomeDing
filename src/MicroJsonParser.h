
// MicroJsonParser.h

#ifndef MICROJSON_H
#define MICROJSON_H

#include <functional>

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
  int _level;

  char _path[64];
  char _name[16];
  char _value[200];

  MicroJsonCallbackFn _callbackFn;
};

#endif

// end.
