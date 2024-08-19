// HomeDing.cpp

#include <Arduino.h>

#include <HomeDing.h>

// some common property names, only once in memory

const char *ACTION_ONTEMPERATURE = "onTemperature";
const char *ACTION_ONHUMIDITY = "onHumidity";
const char *ACTION_ONPRESSURE = "onPressure";

Board homeding;
