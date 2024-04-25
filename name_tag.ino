#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <Arduino_JSON.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "Creds.h"
#include "messages.h"
#include "constants.h"

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

// Defining size, and output pins
#define MAX_DEVICES 8
#define CS_PIN 5

MD_Parola Display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

void setup() {
  Serial.begin(115200);
  Display.begin();
  Display.setIntensity(0);
  Display.print("v" + versionStr);
  delay(600);
  Display.displayClear();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if (retry > 30) {
      Display.print("[net]: FAIL");
      delay(600);
      Display.displayClear();
      Display.setIntensity(1);
      return;
    }
    Display.print("[net]: ...");
    delay(200);
    yield();
    retry++;
  }

  wifi_enabled = true;
  Display.print("[net]: OK");
  delay(600);
  Display.displayClear();
  Display.setIntensity(1);
}

std::string getNextMessage() {
  HTTPClient http;
  String api_url = "https://fedihashbadge.staged-on.workers.dev/wikis.world/wmhack";
  http.begin(api_url);
  // Send HTTP GET request
  int httpResponseCode = http.GET();
  delay(100);

  if (!wifi_enabled) {
    Serial.println("getNextMessage() called with no wifi enabled");
  }

  if (httpResponseCode > 0) {
    std::string message = http.getString().c_str();
    // Free resources
    http.end();
    return message;
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    // Free resources
    http.end();
    return "Error fetching message from API :3";
  }
  
}

void displayName() {
  Display.displayClear();
  Display.displayText(" Sammy ", PA_CENTER, wipe_speed, long_hold, PA_WIPE_CURSOR, PA_WIPE_CURSOR);
  Display.displayClear();
  Display.displayReset();
}

void loop() {
  int i = 0;

  // Have a chance to yeild to other tasks first
  yield();

  if (Display.displayAnimate()) {
    displayName();

    while (i < messageArraySize) {
      if (Display.displayAnimate()) {
        char* nextMessage = messageArray[i];
        if (String(nextMessage) == "GET_HASHTAG") {
          if (!wifi_enabled) {
            Display.displayClear();
            i++;
            continue;
          }
          Serial.println("Getting message from API..");
          nextMessage = &getNextMessage()[0];
        }
        Serial.println(nextMessage);
        if (strlen(nextMessage) < 13) {
          Display.displayText(nextMessage, PA_CENTER, wipe_speed, med_hold, PA_WIPE_CURSOR, PA_WIPE_CURSOR);
        } else {
          Display.displayScroll(nextMessage, PA_CENTER, PA_SCROLL_LEFT, scroll_speed);
        }
        i++;
      }
    }
  }
}
