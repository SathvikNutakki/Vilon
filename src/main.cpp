#include <Arduino.h>
#include <TFT_eSPI.h>
#include <DHT.h>
#include <ESP32Servo.h>
#include "SpecterFace.h"

#define DHTPIN 13
#define TRIG_PIN 5
#define ECHO_PIN 16       
#define IR_RIGHT 34
#define IR_LEFT  35
#define BTN_A 27
#define BTN_B 14


TFT_eSPI tft = TFT_eSPI();
DHT dht(13, DHT22);
Servo LServo;
Servo RServo;
SpecterFace specter(&tft);

unsigned long lastDisplayTime = 0;
unsigned long lastServoTime   = 0;
int servoState = 0;

long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}

int currentVibeIndex = 0;
bool lastBtnA_State = HIGH;
bool lastBtnB_State = HIGH;

const String vibeNames[30] = {
  "IDLE", "BORED", "SUSPICIOUS", "AMUSED", "ANNOYED", 
  "ANGRY", "FOCUSED", "CONFUSED", "SARCASTIC", "SMUG", 
  "DISAPPOINTED", "INTIMIDATING", "SCANNING", "GLITCHING", "SURPRISED", 
  "SLEEPY", "JUDGING", "IMPATIENT", "CALCULATING", "MISCHIEVOUS", 
  "OVERHEATED", "LISTENING", "DISMISSIVE", "PLOTTING", "COCKY", 
  "SHOCKED", "LOADING", "VICTORIOUS", "DEFENSIVE", "MENACING"
};

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
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.writecommand(0x36);
  tft.writedata(0xC0);

  LServo.attach(25);
  RServo.attach(26);

  specter.begin();
  
}

void sensorvaluesdisplay() {
  if (millis() - lastDisplayTime < 500) return;  
  lastDisplayTime = millis();

  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();
  long  dist = getDistance();
  bool  irR  = digitalRead(IR_RIGHT);
  bool  irL  = digitalRead(IR_LEFT);

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 10);
  tft.printf("Temp: %.1fC\n", temp);
  tft.printf(" Hum:  %.1f%%\n", hum);
  tft.printf(" Dist: %ld cm\n", dist);
  tft.printf(" IR R: %s\n", irR ? "EDGE!" : "ok");
  tft.printf(" IR L: %s\n", irL ? "EDGE!" : "ok");
}

void moveservos() {
  bool btnA = digitalRead(BTN_A) == LOW;
  bool btnB = digitalRead(BTN_B) == LOW;

  if (btnA) {
    LServo.writeMicroseconds(1000);
    RServo.writeMicroseconds(1000);
  } else if (btnB) {
    LServo.writeMicroseconds(2000);
    RServo.writeMicroseconds(2000);
  } else {
    LServo.writeMicroseconds(1500);
    RServo.writeMicroseconds(1500);
  }
}

void expressionwithbuttongemini(){
  
  // Read physical input signals
  bool btnA_State = digitalRead(BTN_A);
  bool btnB_State = digitalRead(BTN_B);

  // Button A Clicked -> Next Expression
  if (btnA_State == LOW && lastBtnA_State == HIGH) {
    currentVibeIndex = (currentVibeIndex + 1) % 30;
    specter.setVibe((HarveyVibe)currentVibeIndex);
    specter.setLabel(vibeNames[currentVibeIndex]);
    delay(50); // Harddebounce constraint
  }

  // Button B Clicked -> Previous Expression
  if (btnB_State == LOW && lastBtnB_State == HIGH) {
    currentVibeIndex = (currentVibeIndex - 1 < 0) ? 29 : currentVibeIndex - 1;
    specter.setVibe((HarveyVibe)currentVibeIndex);
    specter.setLabel(vibeNames[currentVibeIndex]);
    delay(50); // Harddebounce constraint
  }

  // Save state tracking variables
  lastBtnA_State = btnA_State;
  lastBtnB_State = btnB_State;

  // Run the core animation updates completely free of main delays
  specter.update();
}


void loop() {
  //sensorvaluesdisplay();
  //moveservos();
  expressionwithbuttongemini();
  specter.update();
  
}