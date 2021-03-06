// HomeDing.cpp

#include <Arduino.h>

#include <HomeDing.h>

// some common property names, only once in memory

const char *PROP_VALUE = "value";
const char *PROP_DESCRIPTION = "description";
const char *PROP_ADDRESS = "address";
const char *PROP_PIN = "pin";
const char *PROP_INVERSE = "inverse";
const char *PROP_PULLUP = "pullup";
const char *PROP_DURATION = "duration";

const char *ACTION_ONVALUE = "onvalue";
const char *ACTION_ONHIGH = "onhigh";
const char *ACTION_ONLOW = "onlow";
const char *ACTION_ONTEMPERATURE = "ontemperature";
const char *ACTION_ONHUMIDITY = "onhumidity";
const char *ACTION_ONPRESSURE = "onpressure";
