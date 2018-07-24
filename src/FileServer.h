/**
 * @file fileserver.h
 * @brief Implementation of extended http based file server.
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
 * * 29.04.2018 created by Matthias Hertel
 */

#ifndef FILESERVER_H
#define FILESERVER_H

#include <functional>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

/**
 * @brief Request Handler implementation for static files in file system.
 * Implements delivering static files and uploading files.
 */
class FileServerHandler : public RequestHandler
{
public:
  /**
   * @brief Construct a new File Server Handler object
   * @param fs The file system to be used.
   * @param path Path to the root folder in the file system that is used for
   * Serving static data down and upload.
   * @param cache_header Cache Header to be used in replies.
   */
  FileServerHandler(FS &fs, const char *path, const char *cache_header)
      : _fs(fs), _path(path), _cache_header(cache_header)
  {
    DEBUG_MSG("init: path=%s cache_header=%s\n", path, cache_header);
    if (_path.endsWith("/"))
      _path.remove(_path.length() - 1);

    _404_response =
        "<html><head><title>Oops! File not found</title></head><body>Oops! "
        "File not found</body></html>";
    DEBUG_MSG("init: _path=%s\n", _path.c_str());
  }

  /**
    @brief check wether the request can be handled by this implementation.
    @param requestMethod method of the http request line.
    @param requestUri request ressource from the http request line.
    @return true when method can be handled.
  */
  bool canHandle(HTTPMethod requestMethod, String requestUri) override
  {
    return ((requestMethod == HTTP_GET) || (requestMethod == HTTP_PUT) ||
            (requestMethod == HTTP_DELETE));
  }

  /**
     @brief stream a file to the client.
     @param server the running server.
     @param path full qualified path to file.
  */
  void handleGet(ESP8266WebServer &server, String path)
  {
    String contentType = getContentType(path);
    unsigned long now = millis();

    DEBUG_MSG("get: path=%s, ct=%s\n", path.c_str(), contentType.c_str());

    // look for gz file, only if the original specified path is not a gz.  So
    // part only works to send gzip via content encoding when a non compressed
    // is asked for if you point the the path to gzip you will serve the gzip as
    // content type "application/x-gzip", not text or javascript etc...
    if (!path.endsWith(".gz") && !_fs.exists(path)) {
      String pathWithGz = path + ".gz";
      if (_fs.exists(pathWithGz))
        path += ".gz";
    }

    File f = _fs.open(path, "r");
    if (!f) {
      DEBUG_MSG("get: file not found.\n");
      server.send(404, "text/html", _404_response);

    } else {
      if (_cache_header.length() != 0)
        server.sendHeader("Cache-Control", _cache_header);
      // server.setContentLength(f.size());
      // server.send(200, contentType, "");
      server.streamFile(f, contentType);
      DEBUG_MSG("  %lu ms.\n", (millis() - now));

    } // if
  } // handleGet()

  /**
     @brief Handle uploading a file using HTTP_PUT method.
     @param server the running server.
     @param path full qualified path to file.
  */
  void handlePut(ESP8266WebServer &server, String path)
  {
    DEBUG_MSG("put: path=%s, size=%d\n", path.c_str(),
              server.arg("plain").length());

    File fsUploadFile = _fs.open(path, "w");
    if (fsUploadFile) {
      // DEBUG_MSG("UploadFile opened.\n");
      // size_t written = fsUploadFile.print(server.arg("plain").c_str());
      // DEBUG_MSG("Written=%d", written);

      size_t written;

      DEBUG_MSG("put: UploadFile opened.\n");
      char *c = (char *)(server.arg("plain").c_str());

      written = 0;
      while (*c) {
        written += fsUploadFile.write((uint8_t)(*c));
        // DEBUG_MSG("%d \n", written);
        c++;
        yield();
      }
      DEBUG_MSG("put: written:%u\n", written);
      DEBUG_MSG("put: done.\n");

      //      size_t written = fsUploadFile.print(server.arg("plain").c_str());
      //      DEBUG_MSG("Written=%d", written);

      fsUploadFile.close();
    }

    server.send(200, "text/plain", "");
  }

  /**
     @brief Handle deleting a file using HTTP_DELETE method.
     @param server the running server.
     @param path full qualified path to file.
  */
  void handleDelete(ESP8266WebServer &server, String path)
  {
    DEBUG_MSG("delete: path=%s\n", path.c_str());

    if (_fs.exists(path)) {
      _fs.remove(path);
      server.send(200, "text/plain", "");
    } else {
      server.send(404, "text/plain", "");
    }
  } // handleDelete()

  bool handle(ESP8266WebServer &server, HTTPMethod requestMethod,
              String requestUri) override
  {
    DEBUG_MSG("handle: request=%s\n", requestUri.c_str());

    String path(_path);
    path += requestUri;

    if (requestMethod == HTTP_GET) {
      handleGet(server, path);
    } else if (requestMethod == HTTP_PUT) {
      handlePut(server, path);
    } else if (requestMethod == HTTP_DELETE) {
      handleDelete(server, path);
    } else {
      return (false);
    } // if

    return (true);
  }

  static String getContentType(const String &path)
  {
    if (path.endsWith(".html"))
      return "text/html";
    else if (path.endsWith(".htm"))
      return "text/html";
    else if (path.endsWith(".css"))
      return "text/css";
    else if (path.endsWith(".txt"))
      return "text/plain";
    else if (path.endsWith(".js"))
      return "application/javascript";
    else if (path.endsWith(".png"))
      return "image/png";
    else if (path.endsWith(".gif"))
      return "image/gif";
    else if (path.endsWith(".jpg"))
      return "image/jpeg";
    else if (path.endsWith(".ico"))
      return "image/x-icon";
    else if (path.endsWith(".svg"))
      return "image/svg+xml";
    else if (path.endsWith(".ttf"))
      return "application/x-font-ttf";
    else if (path.endsWith(".otf"))
      return "application/x-font-opentype";
    else if (path.endsWith(".woff"))
      return "application/font-woff";
    else if (path.endsWith(".woff2"))
      return "application/font-woff2";
    else if (path.endsWith(".eot"))
      return "application/vnd.ms-fontobject";
    else if (path.endsWith(".sfnt"))
      return "application/font-sfnt";
    else if (path.endsWith(".xml"))
      return "text/xml";
    else if (path.endsWith(".pdf"))
      return "application/pdf";
    else if (path.endsWith(".zip"))
      return "application/zip";
    else if (path.endsWith(".gz"))
      return "application/x-gzip";
    else if (path.endsWith(".appcache"))
      return "text/cache-manifest";
    return "application/octet-stream";
  }

protected:
  FS _fs;
  String _path;
  String _cache_header;
  String _404_response;
};

#endif
