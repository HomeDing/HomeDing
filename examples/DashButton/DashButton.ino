/**
 * @file DashButton.ino
 * @brief This is an experimental sketch to implement a Dash Button device that triggers a action using a http GET request.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD 3-Clause style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino
 * and https://homeding.github.io/index.htm#page=/examples/develop.md.
 *
 * Changelog:
 * * 12.03.2021 first version created.
 */

// https://github.com/Luigi-Pizzolito/IFTTT-Dash-Button

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// http://bulb03/$board/switch/en?toggle


String ssid = "DevNet";
String password = "pass-wd-here";
String host = "bulb03";
String url = "/$board/switch/en?toggle";

IPAddress hostIPAddr; // ip address of the remote device


int countConnect = 0;

#define STATE_INIT 0
#define STATE_CONNECTED 1
#define STATE_FINDHOST 2
#define STATE_GET 3

#define STATE_STARTSLEEP 5
#define STATE_DONE 99
int state = STATE_INIT;

#define TRACE(...) Serial.printf(__VA_ARGS__)


/*
   Configurable Settings

   Connect CONFIG_PIN(Default GPIO_03[RX]) to GND during startup to enter Config. Mode:
      1. Connect to 'ESP_Button' WiFi Access Point, with the password 'wifibutton'
      2. Visit http://192.168.4.1 to open the configuration page, or  http://192.168.4.1/edit to see the filesystem.
      3. After seeting your values, click on the 'Save' button then the 'Restart'
      4. Your button is now configured.
*/
/* int wc_p;        // max. time in seconds to connect to wifi, before giving up
int gr_p;        // max. times of attemps to perform GET request, before giving up
bool s_vcc;      //wether to send VCC voltage as a parameter in the url request.
bool is_ip;      //wether host adress is IP
String vcc_parm; //parameter to pass VCC voltage by.

//   System Variables

//#define NOT_DEBUG //wether to enable debug or to show indication lights instead
//Normal Mode
ADC_MODE(ADC_VCC);
bool su_mode = true;
//Config. Mode
#define CONFIG_PIN 3
ESP8266WebServer server(80);
File fsUploadFile;
const char *APssid = "ESP_Button";
const char *APpass = "wifibutton";
 */

// startup device, do not wait for network completed

void setup()
{
  //start serial monitor, FS and Config. Pin
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(3000); // wait for

  TRACE("> Starting DashButton...\n");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());

  TRACE("> Connecting to %s ", ssid.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    TRACE(".");
    //if we are taking too long to connect to WiFi give up.
    countConnect++;
    if (countConnect == 20) {
      TRACE("failed too often.\n");
      state = STATE_STARTSLEEP;
    }
  }
  TRACE("\n");

  if (WiFi.status() == WL_CONNECTED) {
    state = STATE_CONNECTED;
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
  }
}

void loop()
{
  if (state == STATE_CONNECTED) {
    TRACE("> connected");
    TRACE("  IP address: %s", WiFi.localIP().toString().c_str());
    state = STATE_FINDHOST;
    countConnect = 0;

  } else if (state == STATE_FINDHOST) {
    TRACE("> find host...\n");

    // ask for IP address
    WiFi.hostByName(host.c_str(), hostIPAddr); // , DNS_TIMEOUT);
    TRACE(".got %s\n", hostIPAddr.toString().c_str());
    state = STATE_GET;

  } else if (state == STATE_GET) {

    WiFiClient client;
    if (client.connect(hostIPAddr, 80)) {

      String request("GET $1 HTTP/1.1\r\nHost: $2\r\nConnection: close\r\n\r\n");
      request.replace("$1", url);
      request.replace("$2", host);

      TRACE("> request:\n%s", request.c_str());
      client.write(request.c_str());

    }
    state = STATE_DONE;


    // if (!client.connect(host.c_str(), httpPort)) {
    //   //try again if the connection fails.
    //   Serial.println("connection failed");
    //   delay(10);
    //   return;
    // }

    // //create the URI for the request
    // if (s_vcc) {
    //   url += "?";
    //   url += vcc_parm;
    //   url += "=";
    //   uint32_t getVcc = ESP.getVcc();
    //   String VccVol = String((getVcc / 1000U) % 10) + "." + String((getVcc / 100U) % 10) + String((getVcc / 10U) % 10) + String((getVcc / 1U) % 10);
    //   url += VccVol;
    // }

    // //request url to server
    // Serial.print("Requesting URL: ");
    // Serial.println(url);

    // client.print(String("GET ") + url + " HTTP/1.1\r\n" +
    //              "Host: " + host.c_str() + "\r\n" +
    //              "Connection: close\r\n\r\n");

    // unsigned long timeout = millis();
    // while (client.available() == 0) {
    //   if (millis() - timeout > 60000) {
    //     //give up if the server takes too long to reply.
    //     Serial.println(">>> Client Timeout !");
    //     client.stop();
    //     //return;
    //     Serial.println("entering deep sleep");
    //     ESP.deepSleep(0);
    //   }
    // }

    // //print response to serial
    // while (client.available()) {
    //   String line = client.readStringUntil('\r');
    //   Serial.print(line);
    // }
    // //finish request and close connections
    // client.stop();
    // Serial.println();
    // Serial.println("closing connection");

    // //enter Deep Sleep
    // Serial.println("entering deep sleep");
    // delay(100);
    // yay();
    // ESP.deepSleep(0);

  } else if (state == STATE_STARTSLEEP) {
    TRACE("enter deep sleep\n");
    delay(100);
    ESP.deepSleep(0);
  } // if
}

/*
   Universal Functions
*/

void fail()
{
  //something has gone wrong, blink an indicator on the LED.
}
