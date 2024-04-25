#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <Arduino_JSON.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "Creds.h"
#include "messages.h"
#include "config.h"
#include <iostream>
#include <algorithm>

using namespace std;

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

// Defining size, and output pins
#define MAX_DEVICES 8
#define CS_PIN 5

MD_Parola Display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

void setup() {
  Serial.begin(115200);
  random_shuffle(begin(message_array), end(message_array));
  Display.begin();
  Display.setIntensity(0);
  Display.print("v" + version_str);
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
      Display.setIntensity(screen_intensity);
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

std::string getNextMessage(String message_type) {
  HTTPClient http;
  if (message_type == "GET_QUOTE") {
    http.begin(quotes_url);
  } else if (message_type == "GET_HASHTAG") {
    http.begin(api_url);
  } else {
    return "Error fetching message from API :3";
  }

  // Send HTTP GET request
  int http_response_code = http.GET();
  delay(100);

  if (http_response_code > 0) {
    std::string message = http.getString().c_str();
    // Free resources
    http.end();
    return message;
  } else {
    // Free resources
    http.end();
    return "Error fetching message from API :3";
  }
  
}

void displayName() {
  int i = 0;
  while (i < init_message_array_size) {
    if (Display.displayAnimate()) {
      Serial.println(init_message_array[i]);
      Display.displayText(init_message_array[i], PA_CENTER, wipe_speed, long_hold, PA_WIPE_CURSOR, PA_WIPE_CURSOR);
      i++;
    }
  }
  i = 0;
}

void loop() {
  // Have a chance to yeild to other tasks first
  yield();

  // Keep shuffled
  random_shuffle(begin(message_array), end(message_array));

  if (Display.displayAnimate()) {
    displayName();

    int i = 0;
    while (i < message_array_size) {
      if (Display.displayAnimate()) {
        char* next_message = message_array[i];
        if (String(next_message) == "GET_HASHTAG" || String(next_message) == "GET_QUOTE") {
          if (!wifi_enabled) {
            Display.displayClear();
            i++;
            continue;
          }
          Serial.println("Getting data from API (" + String(next_message) + ")...");
          next_message = &getNextMessage(String(next_message))[0];
        }
        Serial.println(next_message);
        if (strlen(next_message) < 13) {
          Display.displayText(next_message, PA_CENTER, wipe_speed, med_hold, PA_WIPE_CURSOR, PA_WIPE_CURSOR);
        } else {
          Display.displayScroll(next_message, PA_CENTER, PA_SCROLL_LEFT, scroll_speed);
        }
        i++;
      }
    }
    i = 0;
  }
}
