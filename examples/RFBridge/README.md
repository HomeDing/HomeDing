# The RFBridge Example folder

This folder contains a implementations of device that can send and receive RF codes using 433 MHz transmitters and receivers
to build a device that can bridge between these 2 wireless technologies.

This element depends on the [RFCodes library](https://github.com/mathertel/rfcodes)
that you can find also through the Arduino library manager. Be sure to install it before compiling/verifying/uploading.

## System configuration

This is a configuration example for ESP32.

### Device configuration in env.json

``` json
{
  "device": {
    "0": {
      "loglevel": 1,
      "name": "rfbridge",
      "title": "433 keys and switches",
      "description": "ESP32 RF-Bridge",
      "room": "Hallway",
      "logfile": 1,
      "xled": "D0",
      "xbutton": "D4",
      "homepage": "/board.htm",
      "safemode": "false"
    }
  },
  "ota": {
    "0": {
      "passwd": "123"
    }
  },
  "ntptime": {
    "0": {
      "ntpserver": "pool.ntp.org",
      "zone": "CET-1CEST,M3.5.0,M10.5.0/3"
    }
  }
}
```


### Bridge configuration in config.json

``` json

```

## RFCodes Element

An experimental Element for sending and receiving rf433 signals.

