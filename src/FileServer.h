/**
 * @file fileserver.h
 * @brief Implementation of the extended http based file server to upload and delete files.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog:
 * * 29.04.2018 created by Matthias Hertel
 * * 04.02.2019 simplifications, saving memory.
 * * 24.11.2019 simplifications, using serveStatic for delivering filesystem files.
 * * 13.12.2019 no upload and delete when running in safemode
 * * 16.05.2021 update to ESP8266 board version 3.0
 * * 10.07.2021 add starting '/' to filenames if not present.
 */

#ifndef FILESERVER_H
#define FILESERVER_H

#include <HomeDing.h>

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
  FileServerHandler(FS &fs, const char *cache_header, Board *board)
      : _fs(fs), _board(board), _cache_header(cache_header)
  {
  }


  /**
    @brief check wether the request can be handled by this implementation.
    @param requestMethod method of the http request line.
    @param requestUri request ressource from the http request line.
    @return true when method can be handled.
  */
#if defined(ESP8266)
  bool canHandle(HTTPMethod requestMethod, UNUSED const String &uri) override
#elif defined(ESP32)
  bool canHandle(HTTPMethod requestMethod, UNUSED String uri) override
#endif
  {
    return ((!_board->isSafeMode) && ((requestMethod == HTTP_POST) || (requestMethod == HTTP_DELETE)));
  } // canHandle()


#if defined(ESP8266)
  bool canUpload(const String &uri) override
#elif defined(ESP32)
  bool canUpload(String uri) override
#endif
  {
    // only allow upload on root fs level.
    return ((!_board->isSafeMode) && (uri == "/"));
  } // canUpload()


#if defined(ESP8266)
  bool handle(WebServer &server, HTTPMethod requestMethod, const String &requestUri) override
#elif defined(ESP32)
  bool handle(WebServer &server, HTTPMethod requestMethod, String requestUri) override
#endif
  {
    // ensure that filename starts with '/'
    String fName = requestUri;
    if (! fName.startsWith("/")) {
      fName = "/" + fName;
    }

    // LOGGER_RAW("File:handle(%s)", requestUri.c_str());
    if (requestMethod == HTTP_POST) {
      server.send(200); // all done in upload. no other forms.

    } else if (requestMethod == HTTP_DELETE) {
      if (_fs.exists(fName)) {
        _fs.remove(fName );
      } else {
        LOGGER_ERR("File <%s> doesn't exist.", fName.c_str());
      }

    }                 // if
    server.send(200); // all done.
    return (true);
  } // handle()


#if defined(ESP8266)
  void upload(UNUSED WebServer &server, UNUSED const String &requestUri, HTTPUpload &upload) override
#elif defined(ESP32)
  void upload(UNUSED WebServer &server, UNUSED String requestUri, HTTPUpload &upload) override
#endif
  {
    // ensure that filename starts with '/'
    String fName = upload.filename;
    if (! fName.startsWith("/")) {
      fName = "/" + fName;
    }

    // LOGGER_TRACE("upload...<%s>", fName.c_str());
    if (fName.indexOf('#') > 0) {
      // LOGGER_TRACE("no #...");
    } else if (fName.indexOf('$') > 0) {
      // LOGGER_TRACE("no $...");

    } else if (upload.status == UPLOAD_FILE_START) {
      if (_fs.exists(fName)) {
        _fs.remove(fName);
      } // if

      _fsUploadFile = _fs.open(fName, "w");

    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (_fsUploadFile)
        _fsUploadFile.write(upload.buf, upload.currentSize);
      hd_yield();

    } else if (upload.status == UPLOAD_FILE_END) {
      if (_fsUploadFile)
        _fsUploadFile.close();
    } // if
  }   // upload()

protected:
  FS _fs;
  File _fsUploadFile;
  Board *_board;

  String _cache_header;
};

#endif
