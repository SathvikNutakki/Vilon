#include <Arduino.h>
#include <TFT_eSPI.h>
#include <DHT.h>
#include <ESP32Servo.h>
#include "HarveyFace.h"

#define DHTPIN 13
#define TRIG_PIN 5
#define ECHO_PIN 16
#define IR_RIGHT 34
#define IR_LEFT 35
#define BTN_A 27
#define BTN_B 14
#define VC02_RX 17
#define VC02_TX 21

String lastVC02Command = "...";
TFT_eSPI tft = TFT_eSPI();
HarveyFace harvey(tft);
DHT dht(DHTPIN, DHT22);
Servo LServo;
Servo RServo;
HardwareSerial VC02(2);

unsigned long lastSensorSerialMs = 0;
unsigned long lastSerialExprMs = 0;

long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 25000);
  if (duration <= 0) return -1;
  return (long)(duration * 0.034 / 2);
}

void pollVC02() {
  while (VC02.available()) {
    String line = VC02.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) continue;
    lastVC02Command = line;
    harvey.setExpressionFromCommand(line);
    Serial.printf("VC02: %s -> expr %u\n", line.c_str(), (unsigned)harvey.currentExpression());
  }
}

// Send a VC02 command name over USB serial to preview a face, e.g. "MoveForward" or "Scare".
void pollSerialExpressions() {
  if (!Serial.available()) return;
  if (millis() - lastSerialExprMs < 120) return;

  String line = Serial.readStringUntil('\n');
  line.trim();
  if (line.length() == 0) return;

  lastSerialExprMs = millis();
  lastVC02Command = line;
  harvey.setExpressionFromCommand(line);
  Serial.printf("Expression set from serial: %s\n", line.c_str());
}

void logSensorsToSerial() {
  if (millis() - lastSensorSerialMs < 2000) return;
  lastSensorSerialMs = millis();

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  long dist = getDistance();
  Serial.printf("temp=%.1f hum=%.1f dist=%ld irL=%d irR=%d\n", temp, hum, dist, digitalRead(IR_LEFT),
                digitalRead(IR_RIGHT));
}

void moveservos() {
  bool btnA = digitalRead(BTN_A) == LOW;
  bool btnB = digitalRead(BTN_B) == LOW;

  if (btnA) {
    LServo.writeMicroseconds(1000);
    RServo.writeMicroseconds(2000);
  } else if (btnB) {
    LServo.writeMicroseconds(2000);
    RServo.writeMicroseconds(1000);
  } else {
    LServo.writeMicroseconds(1500);
    RServo.writeMicroseconds(1500);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_RIGHT, INPUT);
  pinMode(IR_LEFT, INPUT);
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  dht.begin();

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  // If colors look wrong or image is inverted, try ONE of these:
  // tft.invertDisplay(true);
  // tft.setSwapBytes(true);

  harvey.begin();
  harvey.setExpression(HarveyExpr::Idle, true);

  LServo.attach(25);
  RServo.attach(26);
  VC02.begin(9600, SERIAL_8N1, VC02_RX, VC02_TX);

  Serial.println("Harvey face ready. Send MoveForward, Scare, GoToSleep, etc. on serial to test.");
}

void loop() {
  pollVC02();
  pollSerialExpressions();
  harvey.update();
  logSensorsToSerial();
  moveservos();
}
