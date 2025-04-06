#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"
#include <WiFiClient.h>

const char* ssid = "CTF_controller_1";
const char* password = "1234567890";

AsyncWebServer server(80);
String angle1;
String angle2;
float laser_demn;
String laser;
float capture_demn;
String capture;

const int laserPin = 5;
const int capturePin = 13;

void setup() {
  Serial.begin(74880);
  delay(2000);

  WiFi.softAP(ssid, password);
  Serial.println("Access point started");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/angle", HTTP_GET, [&angle1](AsyncWebServerRequest *request){
      request->send_P(200, "text/plain", angle1.c_str());
  });
  server.on("/angle2", HTTP_GET, [&angle2](AsyncWebServerRequest *request){
      request->send_P(200, "text/plain", angle2.c_str());
  });
  server.on("/laserButton", HTTP_GET, [&laser](AsyncWebServerRequest *request){
      request->send_P(200, "text/plain", laser.c_str());
  });
  server.on("/captureButton", HTTP_GET, [&capture](AsyncWebServerRequest *request){
      request->send_P(200, "text/plain", capture.c_str());
  });

  server.begin();
  Serial.println("Server started!");

  pinMode(laserPin, INPUT_PULLUP);
  pinMode(capturePin, INPUT_PULLUP);
}

void loop() {
   if (Serial.available()) {
    angle1 = Serial.readStringUntil('\n'); 
    angle2 = Serial.readStringUntil('\n'); 
    Serial.println(angle1);
    laser_demn = digitalRead(laserPin);
    laser = String(laser_demn);
    capture_demn = digitalRead(capturePin);
    capture = String(capture_demn);
  }
}
