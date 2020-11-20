/********************
      การต่อวงจร
  +++ Green LED +++
  ขายาว -> D0
  ขาสั้น -> GND

  +++ เชื่อมกับ Arduino +++
  สายขาว -> D2
  สายเทา -> D3
********************/

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
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(ledConnStatus, LOW);
    delay(250);
    Serial.print(".");
    digitalWrite(ledConnStatus, HIGH);
    delay(250);
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(ledConnStatus, HIGH);

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ledConnStatus, LOW);
    delay(250);
    Serial.print(".");
    digitalWrite(ledConnStatus, HIGH);
    delay(250);
    return;
  }
  
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(s);

  if (!root.success() || root.size() == 0) {
    return;
  }

  // ถ้ามีข้อมูลส่งมา
  String rgb = root["rgb"];
  bool isColor = root["isColor"];

  // Push id to /records/
  String id = Firebase.push("records", "");
  if (Firebase.success()) {
    // Set string value to /records/{id}/
    Firebase.setString("records/" + id + "/rgb", rgb);
    if (Firebase.success()) {
      // Set bool value to /records/{id}/
      Firebase.setBool("records/" + id + "/isColor", isColor);
      if (Firebase.success()) {
        root.prettyPrintTo(Serial);
        Serial.println();
        Serial.println("---------------------------------");
      }
    }
  }
}
