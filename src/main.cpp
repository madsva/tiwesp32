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
// Define pins for actuator control
const int in1 = 18;
const int in2 = 19;
const int enA = 23;

// MQTT Broker settings
const char* mqtt_server = "192.168.86.36";
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
String parseActionFromJson(const String& json);

String parseActionFromJson(const String& json){
  const String key = "\"action\"";
  int keyPos = json.indexOf(key);
  if (keyPos < 0) {
    return "";
  }

  int colonPos = json.indexOf(':', keyPos + key.length());
  if (colonPos < 0) {
    return "";
  }

  int quoteStart = json.indexOf('"', colonPos + 1);
  if (quoteStart < 0) {
    return "";
  }

  int quoteEnd = json.indexOf('"', quoteStart + 1);
  if (quoteEnd < 0) {
    return "";
  }

  return json.substring(quoteStart + 1, quoteEnd);
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
    if(strcmp(topic, "window/control") == 0){
      String command = "";
      for (unsigned int i = 0; i < length; i++) {
        command += (char)payload[i];
      }
      String action = parseActionFromJson(command);
      Serial.print("Received command: ");
      Serial.println(command);
      Serial.print("Parsed action: ");
      Serial.println(action);
      if(action == "open"){
        analogWrite(enA, 255); // 0-255 where 255 is max speed
        // Move actuator forward
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        delay(2000); // Move for 3 seconds

        // Stop
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
      } else if(action == "close"){
          analogWrite(enA, 255); // 0-255 where 255 is max speed
        // Move actuator backward
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        delay(2000);

        // Stop
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
      } else if(action == "stop"){
        // Stop
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
      }
    }
}

// Forward declaration
void actuatorSetup();


void actuatorSetup(){

  pinMode(ledPin, OUTPUT);

}


void runActuatorForTest();

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 is alive!");

  wifiService.begin();
   pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  actuatorSetup();
  runActuatorForTest();
}




void runActuatorForTest(){
for(int i=0; i<2; i++){
  delay(1000);
  // Nothing to do here (we did it all in setup)
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enA, OUTPUT);

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
  digitalWrite(in2, LOW);
 }


}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  //Serial.println("Tick");
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


