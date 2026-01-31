#include "WifiService.h"
#include <WiFi.h>
#include <Arduino.h>

static const char* AP_NAME = "TIW-Setup";
static const char* AP_PASS = "12345678";

void WifiService::begin() {
    WiFi.mode(WIFI_AP_STA);
    

    if (!wm.autoConnect()) {
        Serial.println("[WiFi] Connection failed, starting portal");
        startPortal();
        return;
    }

    if (!hasCredentials()) {
        Serial.println("[WiFi] No credentials, starting portal");
        startPortal();
        return;
    }

    Serial.println("[WiFi] Connected");
    Serial.println(WiFi.localIP());

    Serial.print("[WiFi] Connected to: ");
    Serial.println(WiFi.SSID());

}


void WifiService::loop() {
 /*if (!portalRunning) return;

  wm.process();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("[WiFi] Portal finished, rebooting");
    delay(1500);
    ESP.restart();
  }*/
 
}

void WifiService::startPortal() {
  Serial.println("[WiFi] Starting config portal");

  wm.setConfigPortalBlocking(true);
  wm.setConfigPortalTimeout(0); // no timeout

  bool success = wm.startConfigPortal(AP_NAME, AP_PASS);

  if (success) {
    Serial.println("[WiFi] Credentials saved, rebooting");
  } else {
    Serial.println("[WiFi] Portal exited without WiFi");
  }

  delay(1500);
  ESP.restart();
}

bool WifiService::hasCredentials() const {
  return WiFi.SSID().length() > 0;
}

bool WifiService::isConnected() const {
  return WiFi.status() == WL_CONNECTED;
}

void WifiService::reset() {
  Serial.println("[WiFi] Resetting credentials");
  wm.resetSettings();
  ESP.restart();
}