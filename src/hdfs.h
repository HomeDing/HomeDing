// implementation of a generic file system helper
// that uses LittleFS as root
// file system, and a mount point for sd cards.

#pragma once

#include <Arduino.h>
#include <HomeDing.h>

#include <FS.h>        // File System for Web Server Files
#include <LittleFS.h>  // File System for Web Server Files

/**
 * A SD or SD_MMC card can be mounted at /sd.
 */
#define HDFS_SD_MOUNTNAME "/sd"
#define HDFS_SD_MOUNTNAME_SLASH "/sd/"

class HomeDingFS {

private:
  static FS *_prep(String &path) {
    FS *efs;
    if (sdFS && path.startsWith(HDFS_SD_MOUNTNAME_SLASH)) {
      // printf("use sdFS\n");
      efs = sdFS;
      path = path.substring(3);
    } else {
      // printf("use rootFS\n");
      efs = rootFS;
    }
    return (efs);
  }

public:
  static FS *rootFS;
  static FS *sdFS;


#if defined(ESP8266)
  static File open(String path, const char *mode = "r") {
    FS *efs = _prep(path);
    return (efs->open(path, mode));
  };

#elif defined(ESP32)

  static File open(String path, const char *mode = FILE_READ, const bool create = false) {
    FS *efs = HomeDingFS::_prep(path);
    return (efs->open(path, mode, create));
  };
#endif

  static bool exists(String path) {
    FS *efs = _prep(path);

    // FS *efs = rootFS;

    // if (sdFS && path.startsWith(HDFS_SD_MOUNTNAME_SLASH)) {
    //   efs = sdFS;
    //   path = path.substring(3);
    // } else {
    //   efs = rootFS;
    // }
    return (efs->exists(path));
  };

  static bool rename(String pathFrom, String pathTo) {
    FS *efs = rootFS;

    if (sdFS && pathFrom.startsWith(HDFS_SD_MOUNTNAME_SLASH)) {
      efs = sdFS;
      pathFrom = pathFrom.substring(3);
      pathTo = pathFrom.substring(3);
    } else {
      efs = rootFS;
    }
    return (efs->rename(pathFrom, pathTo));
  };

  static bool remove(String path) {
    FS *efs = _prep(path);

    // FS *efs = rootFS;

    // if (sdFS && path.startsWith(HDFS_SD_MOUNTNAME_SLASH)) {
    //   efs = sdFS;
    //   path = path.substring(3);
    // } else {
    //   efs = rootFS;
    // }
    return (efs->remove(path));
  };
};
