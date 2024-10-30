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

#include <MQTTElement.h>

#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#include <URI.h>

#define TRACE(...)  // LOGGER_EINFO(__VA_ARGS__)

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
  String lastWill;        ///< last Will for publishing
  String lastWillTopic;   ///> topic for last will
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
#if defined(ESP8266)
      secClient->setFingerprint(_impl->fingerprint.c_str());
#endif
    } else {
      secClient->setInsecure();
    }

    // probe for supported TLS buffer size
    // see https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/bearssl-client-secure-class.html
#if defined(ESP8266)
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

#elif defined(ESP32)
    // secClient->setBufferSizes(512, 512);
#endif

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
    int8_t ret;
    if (_impl->lastWill.isEmpty()) {
      ret = client->connect(_impl->clientID.c_str(), _impl->uri.user, _impl->uri.passwd);
    } else {
      ret = client->connect(_impl->clientID.c_str(), _impl->uri.user, _impl->uri.passwd, _impl->publishTopic.c_str(), _impl->qos, true, _impl->lastWill.c_str());
    }

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

  HomeDing::Actions::push(tmp, payload);
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

  } else if ((active && _impl->hasWildcard) || (name == HomeDing::Actions::Value)) {
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

  } else if (_stricmp(name, "lastWillTopic") == 0) {
    _impl->lastWillTopic = value;

  } else if (_stricmp(name, "lastwill") == 0) {
    _impl->lastWill = value;

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

  } else if (name == HomeDing::Actions::OnValue) {
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
    LOGGER_EERR("missing `server` parameter");

  } else if (_impl->publishTopic.isEmpty() && _impl->subscribeTopic.isEmpty()) {
    LOGGER_EERR("missing `topic` parameter");

  } else {
    Element::start();
    _impl->hasWildcard = (_impl->publishTopic.endsWith("/+"));
    if (_impl->hasWildcard) {
      // remove last '+' character
      _impl->publishTopic.remove(_impl->publishTopic.length() - 1);
    }

    if (_impl->lastWillTopic.isEmpty()) {
      _impl->lastWillTopic = _impl->publishTopic;
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
        LOGGER_EERR("failed");
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
  callback(HomeDing::Actions::Value, _impl->_value.c_str());
}  // pushState()


void MQTTElement::term() {
  TRACE("term()");
  Element::term();
}  // term()


// End
