/********************
      การต่อวงจร
  +++ RGB Color sensor +++
  S0 -> 4
  S1 -> 5
  S3 -> 6
  S2 -> 7
  OUT -> 8
  LED -> 9

  +++ Servo motor +++
  เขียว -> GND
  ส้ม -> VCC
  เหลือง -> 3

  +++ IR Sensor +++
  OUT -> 2

  +++ หลอดไฟสีเขียว +++
  ขายาว -> 12
  ขาสั้น -> GND

  +++ เชื่อมกับ NodeMCU +++
  สายขาว -> 10
  สายเทา -> 11
********************/

#include <Servo.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <TimeLib.h>

// เชื่อมกับ NodeMCU
SoftwareSerial s(10, 11);

// RGB Color sensor
#define S0 4
#define S1 5
#define S3 6
#define S2 7
#define sensorOut 8
#define csLed 9
int rgb[3] = {0, 0, 0};

// Servo motor
Servo servo;
#define svOut 3
int defaultServoDeg = 75;
int nowServoDeg = defaultServoDeg;

// IR Sensor
#define irIn 2
int irVal;

// Status light
#define ledGreen 12

bool first = true;

void setup() {
  setTime(19,14,0,21,11,2020);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  pinMode(csLed, OUTPUT);
  // Setting frequency-scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  servo.attach(svOut);
  servo.write(defaultServoDeg);

  pinMode(irIn, INPUT);

  pinMode(ledGreen, OUTPUT);
  // pinMode(ledRed, OUTPUT);

  s.begin(57600);
  //Serial.begin(9600);
}

void runColorSensor() {
  // RED
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  int r = pulseIn(sensorOut, LOW);
  rgb[0] = map(r, 18, 92, 255, 0);
  delay(100);

  // GREEN
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  int g = pulseIn(sensorOut, LOW);
  rgb[1] = map(g, 19, 93, 255, 0);
  delay(100);

  // BLUE
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  int b = pulseIn(sensorOut, LOW);
  rgb[2] = map(b, 17, 76, 255, 0);
  delay(100);
}

void clearRGBArray() {
  rgb[0] = 0;
  rgb[1] = 0;
  rgb[2] = 0;
}

bool bottleIsAlpha() {
  int r = rgb[0];
  int g = rgb[1];
  int b = rgb[2];
  int alpha = 80;
  int alpha2 = 0;
  if (r < alpha2 && g < alpha2 && b < alpha2) {
    return true;
  }
  return false;
}

void servoWriteSlow(int deg, int dely) {
  if (deg > nowServoDeg) {
    for (int i = nowServoDeg; i <= deg; i++) {
      servo.write(i);
      nowServoDeg = i;
      delay(dely);
    }
  } else if (deg < nowServoDeg) {
    for (int i = nowServoDeg; i >= deg; i--) {
      servo.write(i);
      nowServoDeg = i;
      delay(dely);
    }
  }
}

String nowDateTime() {
  String dateTime = String(day()) + "/" + String(month()) + "/" + String(year()) + " " + String(hour()) + ":" + String(minute()) + ":" + String(second());
  return dateTime;
}

void loop() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  irVal = digitalRead(irIn);
  if (irVal == 0) {
    digitalWrite(ledGreen, HIGH);
    if (first) {
      digitalWrite(csLed, HIGH);
      // รอ 2 วิ แล้วค่อยเริ่มตรวจสี
      delay(2000);
      first = false; // ให้อ่านค่าแค่รอบเดียว
      runColorSensor();
      root["dateTime"] = nowDateTime();
      if (bottleIsAlpha()) {
        root["isColor"] = false;
        servoWriteSlow(10, 10);
      } else {
        root["isColor"] = true;
        servoWriteSlow(150, 10);
      }
      root.printTo(s);
    }
  } else {
    digitalWrite(ledGreen, LOW);
    digitalWrite(csLed, LOW);
    clearRGBArray();
    first = true;
    servoWriteSlow(defaultServoDeg, 10);
  }
  delay(500);
}
