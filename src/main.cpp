#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include "WifiService.h"

WiFiManager wm;
WifiService wifiService;
static constexpr int RESET_BUTTON_PIN = 0;
static constexpr unsigned long RESET_HOLD_MS = 3000;
bool buttonPressed = false;
unsigned long buttonPressStart = 0;

//Notes:
//Reset WiFi if button held for 3 seconds

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 is alive!");

  wifiService.begin();
   pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
}



void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  Serial.println("Tick");
  wifiService.loop();

  if (digitalRead(RESET_BUTTON_PIN) == LOW) {
      if (!buttonPressed) {
        buttonPressed = true;
        buttonPressStart = millis();
      } else if (millis() - buttonPressStart >= RESET_HOLD_MS) {
        Serial.println("[BTN] WiFi reset requested");
        delay(300);
        wifiService.reset(); // clears credentials + restarts
      }
    } else {
      buttonPressed = false;
    }

}

