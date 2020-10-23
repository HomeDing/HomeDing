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
 * * 24.11.2019 simplifications, using serveStatic for delivering filesystem files.
 * * 13.12.2019 no upload and delete when running in safemode
 */

#ifndef FILESERVER_H
#define FILESERVER_H

#include <HomeDing.h>

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
  bool canHandle(HTTPMethod requestMethod, UNUSED String requestUri) override
  {
    return ((!_board->isSafeMode) && ((requestMethod == HTTP_POST) || (requestMethod == HTTP_DELETE)));
  } // canHandle()


  bool canUpload(String requestUri) override
  {
    // only allow upload on root fs level.
    return ((!_board->isSafeMode) && (requestUri == "/"));
  } // canUpload()


  bool handle(ESP8266WebServer &server, HTTPMethod requestMethod,
              String requestUri) override
  {
    if (requestMethod == HTTP_POST) {
      server.send(200); // all done in upload. no other forms.

    } else if (requestMethod == HTTP_DELETE) {
      if (_fs.exists(requestUri)) {
        _fs.remove(requestUri);
      } else {
        LOGGER_ERR("File <%s> doesn't exist.", requestUri.c_str());
      }

    } // if
    server.send(200); // all done.
    return (true);
  } // handle()


  void upload(UNUSED ESP8266WebServer &server, String requestUri, HTTPUpload &upload) override
  {
    // LOGGER_TRACE("upload...<%s>", upload.filename.c_str());
    if (!upload.filename.startsWith("/")) {
      // LOGGER_TRACE("no /...");
    } else if (upload.filename.indexOf('#') > 0) {
      // LOGGER_TRACE("no #...");
    } else if (upload.filename.indexOf('$') > 0) {
      // LOGGER_TRACE("no $...");

    } else if (upload.status == UPLOAD_FILE_START) {
      if (_fs.exists(upload.filename)) {
        _fs.remove(upload.filename);
      } // if

      _fsUploadFile = _fs.open(upload.filename, "w");

    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (_fsUploadFile)
        _fsUploadFile.write(upload.buf, upload.currentSize);
      yield();

    } else if (upload.status == UPLOAD_FILE_END) {
      if (_fsUploadFile)
        _fsUploadFile.close();
    } // if
  } // upload()

protected:
  FS _fs;
  File _fsUploadFile;
  Board *_board;

  String _cache_header;
};

#endif
