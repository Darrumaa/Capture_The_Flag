#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>
#include <Servo.h>

#include <Adafruit_NeoPixel.h>

#define PIN 16

#define NUM_LEDS 30

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

Servo capture;

ESP8266WiFiMulti WiFiMulti;

const int laserPin = 5;
const char* ssid = "CTF_controller_1";
const char* password = "1234567890";

const char* serverNameAngle1 = "http://192.168.4.1/angle";
const char* serverNameAngle2 = "http://192.168.4.1/angle2";
const char* serverNameLaser = "http://192.168.4.1/laserButton";
const char* serverNameCapture = "http://192.168.4.1/captureButton";

String angle1;
String angle2;
String laser;
String captura;

float sp1;
float sp2;

class Motor {
public:
    int in1, in2, en;
    Motor(int i1, int i2, int e) {
        in1 = i1;
        in2 = i2;
        en = e;
        pinMode(i1, OUTPUT);
        pinMode(i2, OUTPUT);
        pinMode(e, OUTPUT);
        digitalWrite(i1, LOW);
        digitalWrite(i2, LOW);
        analogWrite(en, 0);  // Set initial speed to 0
    }

    void go(int sp) {
        sp = constrain(sp, -255, 255);  // Constrain speed
        analogWrite(en, abs(sp));        // Set PWM based on absolute speed
        digitalWrite(in1, sp >= 0);      // Set direction
        digitalWrite(in2, sp < 0);
    }
};

Motor m1(12, 14, 13);
Motor m2(4, 0, 2);

void setup() {
    Serial.begin(74880);

    Serial.println("Connecting to AP...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) { 
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("Connected to AP");

    pinMode(laserPin, OUTPUT);

    pinMode(15, OUTPUT);

    pinMode(A0, INPUT);

    strip.begin();     
    strip.show();

    capture.attach(15);
}

void loop() {
    if (WiFiMulti.run() == WL_CONNECTED) {
        angle1 = httpGETRequest(serverNameAngle1);
        angle2 = httpGETRequest(serverNameAngle2); 
        laser = httpGETRequest(serverNameLaser);
        captura = httpGETRequest(serverNameCapture); 
//        Serial.print("angle1 = " + angle1 + " ");
//        Serial.println("angle2 = " + angle2);
    }

    float x = angle1.toFloat();
    float y = angle2.toFloat();

    const float deadband = 5.0;  
    if (abs(x) < deadband) x = 0;
    if (abs(y) < deadband) y = 0;

    float moveX = map(x, -45, 45, -255, 255);  
    float moveY = map(y, -45, 45, -255, 255);  

    // Control motors based on mapped values  

    float a = analogRead(A0);
    if (a < 40){
      a = 0.00;  
    }
    else {
      a = 1.00;  
    }

    Serial.println(a);

    if (a == 1.00){
      displayRed();
      sp1 = 0.00;
      sp2 = 0.00;
    }
    else if(a == 0.00){
      displayPurple();
      sp1 = moveY + moveX;  
      sp2 = moveY - moveX;
      Serial.println(sp1);  
    }
    
    
    m1.go(sp1);
    m2.go(sp2);

    digitalWrite(laserPin, (1 - (laser.toFloat())));

    if (captura.toFloat() == 1.00){
      capture.write(0);  
    }
    else{
      capture.write(180);  
    }
}

String httpGETRequest(const char* serverName) {
    WiFiClient client;
    HTTPClient http;
    
    http.begin(client, serverName);
    int httpResponseCode = http.GET();
    
    String payload = "--"; 
    if (httpResponseCode > 0) {
        payload = http.getString();
    } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    
    http.end();
    return payload;
}

void displayRed() {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 0));
  }
  strip.show();    
}

void displayPurple() {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 255)); 
  }
  strip.show();              
}

void switchOff(){
   for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0)); 
  }
  strip.show();
}
