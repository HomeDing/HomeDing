// HomeDing.cpp

#include <Arduino.h>

#include <HomeDing.h>

// some common property names, only once in memory

const char *PROP_VALUE = "value";
const char *PROP_ADDRESS = "address";
const char *PROP_PIN = "pin";
const char *PROP_INVERSE = "inverse";

const char *ACTION_ONVALUE = "onValue";
const char *ACTION_ONTEMPERATURE = "onTemperature";
const char *ACTION_ONHUMIDITY = "onHumidity";
const char *ACTION_ONPRESSURE = "onPressure";
