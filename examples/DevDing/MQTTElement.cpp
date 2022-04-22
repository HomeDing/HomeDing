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
#include "URI.h"

#include <PubSubClient.h>

#define TRACE(...) // LOGGER_EINFO(__VA_ARGS__)

/* ===== Define implementation class (data only) ===== */

#define MAX_ERRORS 6
#define RETRY_TIME 5000

class MQTTElementImpl {
public:
  /** States of the Network Connection */
  enum STATE : int {
    DISCONNECTED = 0,
    INITIALIZED = 10,  // can connect / reconnect
    CONNECTED = 11,
    UNUSABLE = 99
  };

  STATE state = STATE::DISCONNECTED;

  /**
   * @brief The actual values to be published.
   * using a list of topic=value like 'temperature=23.77,humidity=55'
   * or a single value using "value=nnnn"
   */
  String _value;

  /**
   * @brief The _valeAction holds the actions that is submitted when subscription data has arrived.
   * It can use the $k (key) and $v (value) placeholders to allow wildcard subscriptions like `device/+`.
   */
  String _valueAction;

  // MQTT specific settings

  URI uri;                ///< used mqtt server.
  String clientID;        ///< the clientID on the mqtt connection
  bool _isSecure;         ///< establish secure connection
  int bufferSize = 0;     ///< secure buffer size
  String fingerprint;     ///< Server SHA1 fingerprint for secure connections
  String publishTopic;    ///< topic path for publishing (without wildcard)
  String subscribeTopic;  ///< topic path for subscription
  bool hasWildcard;       ///< `+` wildcard in publish topic in use
  int qos;                ///< Quality of Service for subscribed topic
  bool retain;            ///< retain value flag for published values

  // MQTT implementation

  WiFiClient *client;        ///< Secure or unsecure client
  PubSubClient *mqttClient;  ///< mqtt specific communication using client connection.
  int errCount = 0;          ///< counting errors to stop communication after too many.
  unsigned long nextConnect = 0;
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
}


void MQTTElement::init(Board *board) {
  TRACE("init()");
  Element::init(board);
  // do something here like initialization
}  // init()


/**
 * @brief Setup connection parameters and registrations for the given server.
 */
void MQTTElement::_setup() {
  TRACE("setup()");

  // first time initialzation of client
  if (_impl->_isSecure) {
    WiFiClientSecure *secClient;

    TRACE("create secure client");
    secClient = new WiFiClientSecure();

    // if a fingerprint signature is given, check in SSL cert of server
    if (!_impl->fingerprint.isEmpty()) {
      secClient->setFingerprint(_impl->fingerprint.c_str());
    } else {
      secClient->setInsecure();
    }

    // probe for supported TLS buffer size
    // see https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/bearssl-client-secure-class.html
    int probeSize = 512;  // start with smallest possible buffer
    while ((_impl->bufferSize == 0) && (probeSize <= 4096)) {
      bool b = secClient->probeMaxFragmentLength(_impl->uri.server, _impl->uri.port, probeSize);
      TRACE("probeSize(%d)=%d", probeSize, b);
      if (b) {
        _impl->bufferSize = probeSize;
      } else {
        probeSize = probeSize * 2;
      }
    }
    if (_impl->bufferSize) {
      secClient->setBufferSizes(_impl->bufferSize, 512);
      _impl->client = secClient;
    }

  } else {
    TRACE("create non-secure client");
    _impl->client = new WiFiClient();
  }

  if (_impl->client) {
    _impl->mqttClient = new PubSubClient(*_impl->client);
    _impl->mqttClient->setServer(_impl->uri.server, _impl->uri.port);

    if (!_impl->subscribeTopic.isEmpty()) {
      _impl->mqttClient->setCallback([this](char *topic, byte *payload, unsigned int length) {
        char msg[length + 2];
        memcpy(msg, payload, length);
        msg[length] = '\0';
        this->_received(String(topic), String(msg));
      });
    }

    _impl->state = MQTTElementImpl::INITIALIZED;
  } else {
    _impl->state = MQTTElementImpl::UNUSABLE;
  }  // if
}


/**
 * @brief Connect and re-connect to the server. Subscribe in case of subscriptions required.
 */
void MQTTElement::_connect() {
  PubSubClient *client = _impl->mqttClient;
  unsigned long now = millis();

  if (now < _impl->nextConnect) {
    // must wait...

  } else {
    TRACE("connect...");

    // try once only.
    int8_t ret = client->connect(_impl->clientID.c_str(), _impl->uri.user, _impl->uri.passwd);
    if (!ret) {
      client->disconnect();
      _impl->errCount++;                      // will case termination after too many errors
      _impl->nextConnect = now + RETRY_TIME;  // will case retry some time.
      LOGGER_EERR("connect failed (%d)", _impl->errCount);

    } else {
      TRACE("connected.");
      _impl->state = MQTTElementImpl::CONNECTED;

      if (!_impl->subscribeTopic.isEmpty()) {
        TRACE("subscribe to %s", _impl->subscribeTopic.c_str());
        client->subscribe(_impl->subscribeTopic.c_str(), _impl->qos);
      }
    }
  }
}  // connect()


/**
 * @brief  A message was received by the subscription.
 */
void MQTTElement::_received(String topic, String payload) {
  LOGGER_ETRACE("Received: %s:%s", topic.c_str(), payload.c_str());

  String tmp = _impl->_valueAction;
  tmp.replace("$t", topic);
  int n = topic.lastIndexOf('/');
  tmp.replace("$k", topic.substring(n + 1));

  _board->dispatch(tmp, payload);
}  // _received


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool MQTTElement::set(const char *name, const char *value) {
  TRACE("set %s=%s", name, value);
  bool ret = true;

  // ===== actions

  if (Element::set(name, value)) {
    // done.

  } else if ((active && _impl->hasWildcard) || (_stricmp(name, "value") == 0)) {
    // action is treated as data topics
    if (!_impl->_value.isEmpty()) { _impl->_value.concat(','); }
    _impl->_value.concat(name);
    _impl->_value.concat('=');
    _impl->_value.concat(value);

    // ===== configuration properties

  } else if (_stricmp(name, "server") == 0) {
    _impl->uri.parse(value);
    _impl->_isSecure = (_stricmp(_impl->uri.protocol, "mqtts") == 0);
    if (_impl->uri.port == 0) {
      _impl->uri.port = _impl->_isSecure ? 8883 : 1883;  // use standard ports.
    }

  } else if (_stricmp(name, "clientid") == 0) {
    _impl->clientID = value;

  } else if (_stricmp(name, "publish") == 0) {
    _impl->publishTopic = value;

  } else if (_stricmp(name, "subscribe") == 0) {
    _impl->subscribeTopic = value;

  } else if (_stricmp(name, "fingerprint") == 0) {
    _impl->fingerprint = value;

  } else if (_stricmp(name, "buffersize") == 0) {
    _impl->bufferSize = _atoi(value);

  } else if (_stricmp(name, "retain") == 0) {
    _impl->retain = _atob(value);

  } else if (_stricmp(name, "qos") == 0) {
    _impl->qos = _atoi(value);

  } else if (_stricmp(name, "onValue") == 0) {
    _impl->_valueAction = value;

  } else {
    ret = false;
  }  // if

  return (ret);
}  // set()


/**
 * @brief Activate the MQTTElement.
 */
void MQTTElement::start() {
  TRACE("start()");

  if (_impl->clientID.isEmpty()) {
    _impl->clientID = _board->deviceName;
  }

  // Verify parameters
  if (_impl->uri.server == nullptr) {
    LOGGER_EERR("missing `server` parameter.");

  } else if (_impl->publishTopic.isEmpty() && _impl->subscribeTopic.isEmpty()) {
    LOGGER_EERR("missing a `topic` parameter.");

  } else {
    Element::start();
    _impl->hasWildcard = (_impl->publishTopic.endsWith("/+"));
    if (_impl->hasWildcard) {
      // remove last '+' character
      _impl->publishTopic.remove(_impl->publishTopic.length() - 1);
    }

    _impl->state = MQTTElementImpl::STATE::DISCONNECTED;
  }
}  // start()


/**
 * @brief Give some processing time to the Element to check for next actions.
 * Do only one activity at a time no to block too long.
 */
void MQTTElement::loop() {
  PubSubClient *client = _impl->mqttClient;

  if (client)
    client->loop();

  if (!_impl->_value.isEmpty()) {
    // TRACE("loop: %s", _impl->_value.c_str());

    if (_impl->errCount > MAX_ERRORS) {
      // dump, stop network activities...
      LOGGER_EERR("too many errors");
      term();

    } else if (_impl->state == MQTTElementImpl::DISCONNECTED) {
      _setup();

    } else if (_impl->state == MQTTElementImpl::CONNECTED && (!client->connected())) {
      _impl->state = MQTTElementImpl::INITIALIZED;

    } else if (_impl->state == MQTTElementImpl::INITIALIZED) {
      _connect();

    } else if (_impl->state == MQTTElementImpl::CONNECTED) {
      // extract first item with (key '=' value)
      String item = Element::popItemValue(_impl->_value);

      int n = item.indexOf('=');
      String k;
      String v = item.substring(n + 1);

      if (_impl->hasWildcard) {
        // build topic name
        k = _impl->publishTopic + item.substring(0, n);
      } else {
        // publishTopic has full topic name
        k = _impl->publishTopic;
      }

      LOGGER_ETRACE("publish %s %s", k.c_str(), v.c_str());
      bool done = client->publish(k.c_str(), v.c_str(), _impl->retain);

      if (!done) {
        LOGGER_EERR("failed.");
        _impl->errCount++;
      } else {

        TRACE("OK!");
        _impl->errCount = 0;
      }
    }  // if

  }  // if
}  // loop()


/**
 * @brief push the current value of all properties to the callback.
 */
void MQTTElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
  callback(PROP_VALUE, _impl->_value.c_str());
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
