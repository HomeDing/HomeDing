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
 * * 04.02.2019 simplifications, saving memory.
 */

#ifndef FILESERVER_H
#define FILESERVER_H

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
  FileServerHandler(FS &fs, const char *cache_header)
      : _fs(fs), _cache_header(cache_header)
  {
  }

  /**
    @brief check wether the request can be handled by this implementation.
    @param requestMethod method of the http request line.
    @param requestUri request ressource from the http request line.
    @return true when method can be handled.
  */
  bool canHandle(HTTPMethod requestMethod, String requestUri) override
  {
    // LOGGER_TRACE("canHandle(%s)", requestUri.c_str());

    bool ret = false;
    if (requestUri.startsWith("/$")) {
      // ret = false;

    } else if ((requestMethod == HTTP_GET) || (requestMethod == HTTP_DELETE)) {
      // file must exist.
      ret = _fs.exists(requestUri);

    } else if (requestMethod == HTTP_POST) {
      // canUpload is handling file uploads
      ret = true;

    } // if
    return (ret);
  } // canHandle()


  bool canUpload(String uri)
  {
    // LOGGER_RAW("++ canUpload(%s)", uri.c_str());
    return true;
  }

  /**
     @brief stream a file to the client.
     @param server the running server.
     @param path full qualified path to file.
  */
  void handleGet(ESP8266WebServer &server, String path)
  {
    // file is existing. was checked in canHandle().
    String contentType = getContentType(path);

    File f = _fs.open(path, "r");
    if (f) {
      if (_cache_header.length() != 0)
        server.sendHeader("Cache-Control", _cache_header);
      server.streamFile(f, contentType);
    } // if
  } // handleGet()


  bool handle(ESP8266WebServer &server, HTTPMethod requestMethod,
              String path) override
  {
    // LOGGER_RAW("handle(%s)", requestUri.c_str());

    if (requestMethod == HTTP_GET) {
      handleGet(server, path);

    } else if (requestMethod == HTTP_POST) {
      server.send(200); // all done in upload. no other forms.

    } else if (requestMethod == HTTP_DELETE) {
      // file is existing. was checked in canHandle().
      _fs.remove(path);
      server.send(200);

    } else {
      return (false);
    } // if

    return (true);
  }

  void upload(ESP8266WebServer &server, String requestUri, HTTPUpload &upload)
  {
    if (upload.status == UPLOAD_FILE_START) {
      if (_fs.exists(upload.filename)) {
        _fs.remove(upload.filename);
      } // if

      _fsUploadFile = _fs.open(upload.filename, "w");

    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (_fsUploadFile)
        _fsUploadFile.write(upload.buf, upload.currentSize);

    } else if (upload.status == UPLOAD_FILE_END) {
      if (_fsUploadFile)
        _fsUploadFile.close();
    }
  }


  /** return http content type from filetype */
  static String getContentType(const String &fName)
  {
    if (fName.endsWith(".htm"))
      return TEXT_HTML;
    // else if (fName.endsWith(".html"))
    //   return TEXT_HTML;
    else if (fName.endsWith(".css"))
      return "text/css";
    else if (fName.endsWith(".txt"))
      return TEXT_PLAIN;
    else if (fName.endsWith(".js"))
      return "application/javascript";
    else if (fName.endsWith(".png"))
      return IMAGE_PNG;
    // else if (fName.endsWith(".gif"))
    //   return "image/gif";
    // else if (fName.endsWith(".jpg"))
    //   return "image/jpeg";
    else if (fName.endsWith(".json"))
      return TEXT_JSON;
    else if (fName.endsWith(".ico"))
      return "image/x-icon";
    else if (fName.endsWith(".svg"))
      return "image/svg+xml";
    // else if (fName.endsWith(".xml"))
    //   return "text/xml";
    // else if (fName.endsWith(".zip"))
    //   return "application/zip";
    return "application/octet-stream";
  }

protected:
  FS _fs;
  File _fsUploadFile;

  String _cache_header;
};

#endif
