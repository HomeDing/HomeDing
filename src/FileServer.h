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

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

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
    LOGGER_RAW("init: path=%s cache_header=%s", path, cache_header);
    if (_path.endsWith("/"))
      _path.remove(_path.length() - 1);

    _404_response =
        "<html><head><title>Oops! File not found</title></head><body>Oops! "
        "File not found</body></html>";
    LOGGER_RAW("init: _path=%s", _path.c_str());
  }

  /**
    @brief check wether the request can be handled by this implementation.
    @param requestMethod method of the http request line.
    @param requestUri request ressource from the http request line.
    @return true when method can be handled.
  */
  bool canHandle(HTTPMethod requestMethod, String requestUri) override
  {
    LOGGER_RAW("canHandle(%s)", requestUri.c_str());
    return (
        (requestMethod == HTTP_GET) ||
        (requestMethod == HTTP_PUT) || // upload text to files
        (requestMethod == HTTP_POST) || // canUpload is handling file uploads
        (requestMethod == HTTP_DELETE));
  }

  bool canUpload(String uri)
  {
    LOGGER_RAW("++ canUpload(%s)", uri.c_str());
    return true;
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
      LOGGER_RAW(" file not found.");
      server.send(404, "text/html", _404_response);

    } else {
      if (_cache_header.length() != 0)
        server.sendHeader("Cache-Control", _cache_header);
      // server.setContentLength(f.size());
      // server.send(200, contentType, "");
      server.streamFile(f, contentType);
      LOGGER_RAW(" %lu ms. %s", (millis() - now), contentType.c_str());

    } // if
  } // handleGet()

  /**
     @brief Handle uploading a file using HTTP_PUT method.
     Can only handle text based files, not containing a \0 character.
     @param server the running server.
     @param path full qualified path to file.
  */
  void handlePut(ESP8266WebServer &server, String path)
  {
    LOGGER_RAW(" size=%d", server.arg("plain").length());

    File fsUploadFile = _fs.open(path, "w");
    if (fsUploadFile) {
      size_t written;

      LOGGER_RAW(" UploadFile opened.");
      char *c = (char *)(server.arg("plain").c_str());

      written = 0;
      while (*c) {
        written += fsUploadFile.write((uint8_t)(*c));
        c++;
        yield();
      }
      LOGGER_RAW(" %d bytes written", written);

      fsUploadFile.close();
    } // if
    server.send(200);
  } // handlePut()

  /**
   * @brief handle post requests. Not in use as today.
   * @param server
   * @param path
   */
  void handlePost(ESP8266WebServer &server, String path)
  {
    LOGGER_RAW("post: path=%s", path.c_str());
    server.send(200);
  }


  /**
     @brief Handle deleting a file using HTTP_DELETE method.
     @param server the running server.
     @param path full qualified path to file.
  */
  void handleDelete(ESP8266WebServer &server, String path)
  {
    LOGGER_RAW("delete: path=%s", path.c_str());

    if (_fs.exists(path)) {
      _fs.remove(path);
      server.send(200, "text/plain", "");
    } else {
      server.send(404, "text/plain", "");
    } // if
  } // handleDelete()

  bool handle(ESP8266WebServer &server, HTTPMethod requestMethod,
              String requestUri) override
  {
    LOGGER_RAW("handle(%s)", requestUri.c_str());

    String path(_path);
    path += requestUri;

    if (requestMethod == HTTP_GET) {
      handleGet(server, path);

    } else if (requestMethod == HTTP_PUT) {
      handlePut(server, path);

    } else if (requestMethod == HTTP_POST) {
      handlePost(server, path);

    } else if (requestMethod == HTTP_DELETE) {
      handleDelete(server, path);

    } else {
      return (false);
    } // if

    return (true);
  }

  void upload(ESP8266WebServer &server, String requestUri, HTTPUpload &upload)
  {
    LOGGER_RAW("++ upload(%s)", requestUri.c_str());
    LOGGER_RAW("++ .filename='%s'", upload.filename.c_str());

    (void)server;
    if (upload.status == UPLOAD_FILE_START) {
      LOGGER_RAW("++ ->UPLOAD_FILE_START");
      // remove existing file to retain file system space.
      if (_fs.exists(upload.filename)) {
        _fs.remove(upload.filename);
      } // if

      _fsUploadFile = _fs.open(upload.filename, "w");

      // DBG_OUTPUT_PORT.print("Upload: START, filename: ");
      // DBG_OUTPUT_PORT.println(upload.filename);

    } else if (upload.status == UPLOAD_FILE_WRITE) {
      LOGGER_RAW("++ ->UPLOAD_FILE_WRITE(%d)", upload.currentSize);
      if (_fsUploadFile) {
        _fsUploadFile.write(upload.buf, upload.currentSize);
      }
      // DBG_OUTPUT_PORT.print("Upload: WRITE, Bytes: ");
      // DBG_OUTPUT_PORT.println(upload.currentSize);

    } else if (upload.status == UPLOAD_FILE_END) {
      LOGGER_RAW("++ ->UPLOAD_FILE_END(%d)", upload.totalSize);
      if (_fsUploadFile) {
        _fsUploadFile.close();
      }
      // DBG_OUTPUT_PORT.print("Upload: END, Size: ");
      // DBG_OUTPUT_PORT.println(upload.totalSize);
    }
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
    else if (path.endsWith(".json"))
      return "application/json";
    else if (path.endsWith(".ico"))
      return "image/x-icon";
    else if (path.endsWith(".svg"))
      return "image/svg+xml";
    else if (path.endsWith(".xml"))
      return "text/xml";
    else if (path.endsWith(".zip"))
      return "application/zip";
    else if (path.endsWith(".gz"))
      return "application/x-gzip";
    return "application/octet-stream";
  }

protected:
  FS _fs;
  File _fsUploadFile;

  String _path;
  String _cache_header;
  String _404_response;
};

#endif
