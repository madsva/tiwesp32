#pragma once

#include <WiFiManager.h>


class WifiService {
public:
    void begin();
    void loop();
    void startPortal();
    bool isConnected() const;
    void reset();

private:
    WiFiManager wm;
    bool portalRunning = false;
    bool hasCredentials() const;
    

};