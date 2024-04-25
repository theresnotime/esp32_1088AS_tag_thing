#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Uncomment according to your hardware type
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
//#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW

// Defining size, and output pins
#define MAX_DEVICES 8
#define CS_PIN 5

MD_Parola Display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

char* messageArray[] = {
  "They/Them",
  "User:TheresNoTime",
  "https://theresnotime.co.uk",
  "I merged to core and nothing broke.        Honest."
};

const int messageArraySize = sizeof(messageArray) / sizeof(messageArray[0]);

const int long_hold = 5000;
const int med_hold = 5000;

void setup() {
  Display.begin();
  Display.setIntensity(0);
}

void displayName() {
  Display.displayClear();
  Display.displayText("Sammy Fox", PA_CENTER, 40, long_hold, PA_WIPE_CURSOR, PA_WIPE_CURSOR);
  Display.displayClear();
  Display.displayReset();
}

void loop() {
  int i = 0;

  if (Display.displayAnimate()) {
    displayName();

    while (i < messageArraySize) {
      if (Display.displayAnimate()) {
        if (strlen(messageArray[i]) < 12) {
          Display.displayText(messageArray[i], PA_CENTER, 40, med_hold, PA_WIPE_CURSOR, PA_WIPE_CURSOR);
        } else {
          Display.displayScroll(messageArray[i], PA_CENTER, PA_SCROLL_LEFT, 60);
        }
        i++;
      }
    }
  }
}
