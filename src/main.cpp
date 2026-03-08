#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include "WifiService.h"
#include <PubSubClient.h>

WiFiManager wm;
WifiService wifiService;
static constexpr int RESET_BUTTON_PIN = 0;
const int ledPin = 2; // Onboard LED pin (usually GPIO 2)
static constexpr unsigned long RESET_HOLD_MS = 3000;
bool buttonPressed = false;
unsigned long buttonPressStart = 0;

// MQTT Broker settings
const char* mqtt_server = "192.168.86.25";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() {
    while (!client.connected()) {
          Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP32Client")) {
            Serial.println("connected");
            client.subscribe("window/control");
           
        } else {
            delay(5000);
        }
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
    // Handle incoming message
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 is alive!");

  wifiService.begin();
   pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
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
  
  if (wifiService.isConnected()) {
      if (!client.connected()) {
          reconnect();
      }
      client.loop();
      
      // Example: publish a message every 5 seconds
     /** static unsigned long lastMsg = 0;
      if (millis() - lastMsg > 5000) {
          lastMsg = millis();
          client.publish("test/topic", "Hello from ESP32!");
      } */
  }

}


/********* START LINEAR ACTUATOR CODE ***********/
 // for(int i=0; i<2; i++){
 //   delay(1000);
// Nothing to do here (we did it all in setup)
/*pinMode(in1, OUTPUT);
pinMode(in2, OUTPUT);
pinMode(enA, OUTPUT);*/
/*
    // Enable motor driver (PWM, full speed)
    analogWrite(enA, 255); // 0-255 where 255 is max speed

    // Move actuator forward
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    delay(2000); // Move for 3 seconds

    // Stop
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    delay(1000);

    // Move actuator backward
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    delay(2000);

    // Stop
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);*/
//} 
/********* END LINEAR ACTUATOR CODE ***********/


  
//}

