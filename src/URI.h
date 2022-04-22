#pragma once

#include <cstring>

#define NUL '\0'

// parsing strings like

/**
 * @brief This URI class parses the fragments of a uri from the string notation like `https://user:passwd@server:port/path?query`
 * to allow easy access to the elements through pointers
 * The string to be parsed is copied to an internal buffer to make the char *pointer long term stable.
 */
class URI {
public:
  ~URI() {
    clear();
  };

  // reset all class information and buffer 
  void clear() {
    protocol = nullptr;
    user = nullptr;
    passwd = nullptr;
    server = nullptr;
    port = 0;
    path = nullptr;
    query = nullptr;

    if (buffer) free(buffer);
    buffer = nullptr;
  }

  // parse the URI 
  void parse(const char *uri) {
    Serial.print("--uri=");
    Serial.println(uri);
    if ((uri) && (*uri)) {
      size_t len = strlen(uri);
      char *s = buffer = (char *)malloc(len + 1);  // current parsing pointer into buffer[];

      memcpy(buffer, uri, len + 1);

      // protocol: up to first ':'
      if (char *p = strchr(s, ':')) {
        protocol = s;
        *p = NUL;  // replace ':' with NUL to terminate protocol string
        s = p + 1;
      }

      // skip all '/'s if present
      while (*s == '/') s++;

      // user:passwd up to first '@'
      if (char *p = strchr(s, '@')) {
        user = s;
        char *p2 = strchr(s, ':');
        if (p2 && (p2 < p)) {
          *p2 = NUL;
          passwd = p2 + 1;
        }
        *p = NUL;
        s = p + 1;
      }

      // server + port '/'
      server = s;
      if (char *p = strchr(s, '/')) {
        *p = NUL;  // terminate server (may contain port)
        s = p + 1;
      } else {
        // path and query are optional
        s = nullptr;
      }

      // port in server ?
      if (char *p = strchr(server, ':')) {
        *p++ = NUL;  // terminate server string
        port = strtol(p, nullptr, 10);
      }

      // optional path and query ?
      if (s) {
        path = s;
        if (char *p = strchr(s, '?')) {
          *p = NUL;
          query = p + 1;
        }
      }
    }

    // Serial.printf("--protocol=%s\n", protocol ? protocol : "-");
    // Serial.printf("--user=%s\n", user ? user : "-");
    // Serial.printf("--passwd=%s\n", passwd ? passwd : "-");
    // Serial.printf("--server=%s\n", server ? server : "-");
    // Serial.printf("--port=%d\n", port);
    // Serial.printf("--path=%s\n", path ? path : "-");
    // Serial.printf("--query=%s\n", query ? query : "-");
  }

  char *protocol = nullptr;
  char *user = nullptr;
  char *passwd = nullptr;
  char *server = nullptr;
  int port = 0;
  char *path = nullptr;
  char *query = nullptr;

private:
  char *buffer = nullptr;
};
