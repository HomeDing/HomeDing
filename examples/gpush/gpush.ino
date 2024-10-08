// Include required libraries
#include "WiFi.h"
#include <HTTPClient.h>
#include "time.h"

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800;
const int daylightOffset_sec = 0;

// WiFi credentials
const char* ssid = "KHMH";               // change SSID
const char* password = "hk-2012FD2926";  // change password

// Google script ID and required credentials
String GOOGLE_SCRIPT_ID = "AKfycbwezP1pKBKnRtC44_4uLVKMZIB68WH0NrN8ZcUrE2MPx3345XOJrZYt9IOyVt2s0ey5MA";  // change Gscript ID
int count = 0;

String urlPattern = "https://script.google.com/macros/s/%id/exec?time=%t&device=%d&data=%v";


bool sendData(String value) {
  String url = urlPattern;
  char buffer[22];  // 22 chars should be enough for "yyyy-mm-dd HH:MM:SS"

  time_t now = time(nullptr);
  strftime(buffer, sizeof(buffer), "%Y-%m-%d+%H:%M:%S", localtime(&now));
  String sTime(buffer);

  value.replace(".", ",");

url.replace("%id", GOOGLE_SCRIPT_ID);
url.replace("%t", sTime);
url.replace("%d", "outside/temp");
url.replace("%v", value);

}


void setup() {
  delay(1000);
  Serial.begin(115200);
  delay(1000);

  // connect to WiFi
  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

// <https://script.google.com/macros/s/AKfycbwezP1pKBKnRtC44_4uLVKMZIB68WH0NrN8ZcUrE2MPx3345XOJrZYt9IOyVt2s0ey5MA/exec?device=devding/dht&data=12.7>


void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    static bool flag = false;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    }
    char timeStringBuff[50];  // 50 chars should be enough
    strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%d %H:%M:%S", &timeinfo);

    String asString(timeStringBuff);
    asString.replace(" ", "+");
    Serial.print("Time:");
    Serial.println(asString);

    float value = 15 + (float)(random(1000)) / 100.00;
    String valString(value, 2);
    valString.replace(".", ",");

    String urlFinal =
      "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec"
      + "?time=" + asString
      + "&device=devding/dht"
      + "&data=" + valString;

    Serial.print("GET:");
    Serial.println(urlFinal);
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();

    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    //---------------------------------------------------------------------
    // getting response from google sheet
    String payload;
    if (httpCode > 0) {
      payload = http.getString();
      Serial.println("Payload: " + payload);
    }
    //---------------------------------------------------------------------
    http.end();
  }
  count++;
  delay(1000);
}