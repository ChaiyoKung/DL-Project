#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <FirebaseArduino.h>
#include <TimeLib.h>

SoftwareSerial s(D3, D2);

#define FIREBASE_HOST "***REMOVED***"
#define FIREBASE_AUTH "***REMOVED***"
#define WIFI_SSID "***REMOVED***"
#define WIFI_PASSWORD "***REMOVED***"

#define ledConnStatus D0

void setup() {
  pinMode(ledConnStatus, OUTPUT);

  // Initialize Serial port
  Serial.begin(57600);
  s.begin(57600);
  while (!Serial) continue;

  // Connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ledConnStatus, HIGH);
    Serial.print(".");
    delay(500);
    digitalWrite(ledConnStatus, LOW);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  digitalWrite(ledConnStatus, HIGH);

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  //  Serial.println(now());
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(s);

  if (!root.success() || root.size() == 0) {
    return;
  }

  // ถ้ามีข้อมูลส่งมา
  root.prettyPrintTo(Serial);
  Serial.println();
  String rgb = root["rgb"];
  bool isColor = root["isColor"];
  Serial.println("---------------------------------");

  // set string value
  String id = Firebase.push("records", "");
  Firebase.setString("records/" + id + "/rgb", rgb);
  // handle error
  if (Firebase.failed()) {
    Serial.print("setting /rgb failed:");
    Serial.println(Firebase.error());
    return;
  }

  // set bool value
  Firebase.setBool("records/" + id + "/isColor", isColor);
  // handle error
  if (Firebase.failed()) {
    Serial.print("setting /isColor failed:");
    Serial.println(Firebase.error());
    return;
  }
}
