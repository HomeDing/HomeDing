/**
 * @file MQTTElement.cpp
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see MQTTElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "MQTTElement.h"

#include <PubSubClient.h>


#define TRACE(...) LOGGER_EINFO(__VA_ARGS__)


/* ===== Define implementation class (data only) ===== */

class MQTTElementImpl {
public:
  /** States of the Network Connection */
  enum STATE : int {
    DISCONNECTED = 0,
    INITIALIZED = 10,
    CONNECTED = 11,
    UNUSABLE = 99
  };

  PubSubClient *_mqttClient;  ///< mqtt specific communication using _client connection.
  STATE state = STATE::DISCONNECTED;
};


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new MQTTElement
 * @return MQTTElement* created element
 */
Element *MQTTElement::create() {
  return (new MQTTElement());
}  // create()


/* ===== Element functions ===== */

MQTTElement::MQTTElement() {
  _impl = new MQTTElementImpl();
  _needSending = false;
}


void MQTTElement::init(Board *board) {
  TRACE("init()");
  Element::init(board);
  // do something here like initialization
}  // init()



void MQTTElement::_setup() {
  // Secure mode preparations

  // first time initialzation of client
  if (_isSecure) {
    WiFiClientSecure *secClient;

    TRACE("create secure client");
    secClient = new WiFiClientSecure();

    // if a fingerprint signature is given, check in SSL cert of server
    if (!_fingerprint.isEmpty()) {
      secClient->setFingerprint(_fingerprint.c_str());
    } else {
      secClient->setInsecure();
    }

    // probe for supported TLS buffer size
    // see https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/bearssl-client-secure-class.html
    int probeSize = 512;  // start with smallest possible buffer
    while ((_bufferSize == 0) && (probeSize <= 4096)) {
      bool b = secClient->probeMaxFragmentLength(_uri.server, _uri.port, probeSize);
      TRACE("probeSize(%d)=%d", probeSize, b);
      if (b) {
        _bufferSize = probeSize;
      } else {
        probeSize = probeSize * 2;
      }
    }
    if (_bufferSize) {
      secClient->setBufferSizes(_bufferSize, 512);
      _client = secClient;
    }

  } else {
    TRACE("create non-secure client");
    _client = new WiFiClient();
  }

  if (_client) {
    _impl->_mqttClient = new PubSubClient(*_client);
    _impl->_mqttClient->setServer(_uri.server, _uri.port);
    _impl->state = MQTTElementImpl::INITIALIZED;
  } else {
    _impl->state = MQTTElementImpl::UNUSABLE;
  }  // if
}


void MQTTElement::_connect() {
  PubSubClient *client = _impl->_mqttClient;

  if (_impl->state == MQTTElementImpl::INITIALIZED) {
    int8_t ret;
    uint8_t retries = 3;

    do {
      ret = client->connect("myDev", _uri.user, _uri.passwd);
      if (!ret) {
        LOGGER_EERR("connect failed: %d", ret);
        client->disconnect();
        delay(5000);  // wait 5 seconds
        retries--;
      }
    } while ((!ret) && (retries >= 0));

    if (!ret) {
      client->disconnect();
      Element::term();

    } else {
      LOGGER_ETRACE("connected.");
      _impl->state = MQTTElementImpl::CONNECTED;
    }
  }
}


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool MQTTElement::set(const char *name, const char *value) {
  bool ret = true;

  // ===== actions

  if (_stricmp(name, "value") == 0) {
    _value = value;
    _needSending = true;

    // ===== configuration properties

  } else if (_stricmp(name, "server") == 0) {
    _uri.parse(value);
    _isSecure = (_stricmp(_uri.protocol, "mqtts") == 0);

    if (_uri.port == 0) {
      _uri.port = _isSecure ? 8883 : 1883;
    }

  } else if (_stricmp(name, "topic") == 0) {
    _topic = value;

  } else if (_stricmp(name, "fingerprint") == 0) {
    _fingerprint = value;

  } else if (_stricmp(name, "buffersize") == 0) {
    _bufferSize = _atoi(value);

  } else if (_stricmp(name, "qos") == 0) {
    _qos = _atoi(value);

  } else if (_stricmp(name, "retain") == 0) {
    _retain = _atob(value);

    // } else if (_stricmp(name, ACTION_ONVALUE) == 0) {
    // save the actions
    // _xAction = value;

  } else {
    ret = Element::set(name, value);
  }  // if

  return (ret);
}  // set()


/**
 * @brief Activate the MQTTElement.
 */
void MQTTElement::start() {
  TRACE("start()");

  // Verify parameters
  if (_uri.server == nullptr) {
    LOGGER_EERR("missing `server` parameter.");
  } else if (_topic.isEmpty()) {
    LOGGER_EERR("missing `feed` parameter.");
  } else {
    Element::start();
    _impl->state = MQTTElementImpl::STATE::DISCONNECTED;
  }
}  // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 */
void MQTTElement::loop() {
  PubSubClient *client = _impl->_mqttClient;

  if (client)
    client->loop();

  if (_needSending) {
    TRACE("loop: %s", _value.c_str());

    if (_errCount > 12) {
      // dump, stop network activities...
      _needSending = false;

    } else if (_impl->state == MQTTElementImpl::DISCONNECTED) {
      TRACE("setup...");
      _setup();

    } else if (_impl->state == MQTTElementImpl::CONNECTED && (!client->connected())) {
      TRACE("re-connect...");
      _impl->state = MQTTElementImpl::INITIALIZED;

    } else if (_impl->state == MQTTElementImpl::INITIALIZED) {
      TRACE("connect...");
      _connect();

    } else if (_impl->state == MQTTElementImpl::CONNECTED) {
      LOGGER_ETRACE("publish %s='%s'", _topic.c_str(), _value.c_str());
      bool done = client->publish(_topic.c_str(), _value.c_str(), _retain);

      if (done) {
        TRACE("OK!");
        _errCount = -1;
      } else {
        TRACE("Failed");
      }
      _needSending = false;
    }  // if

    _errCount++;
  }  // if
}  // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void MQTTElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback(PROP_VALUE, _value.c_str());
}  // pushState()


void MQTTElement::term() {
  TRACE("term()");
  Element::term();
}  // term()


/* ===== Register the Element ===== */

// As long as the Element is project specific or is a element always used
// the registration is placed here without using a register #define.

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_XXX should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to MQTTElement.h:

// #ifdef HOMEDING_REGISTER
// Register the MQTTElement onto the ElementRegistry.
bool MQTTElement::registered =
  ElementRegistry::registerElement("mqtt", MQTTElement::create);
// #endif

// End
