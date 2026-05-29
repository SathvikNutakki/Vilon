#include <Arduino.h>
#include <TFT_eSPI.h>
#include <DHT.h>

#define DHTPIN 13
#define DHTTYPE DHT22
#define TRIG_PIN 5
#define ECHO_PIN 16        // updated from 18 cuz 18 is already being used for tft display
#define IR_RIGHT 34
#define IR_LEFT  35

TFT_eSPI tft = TFT_eSPI();
DHT dht(DHTPIN, DHTTYPE);

long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_RIGHT, INPUT);
  pinMode(IR_LEFT, INPUT);
  dht.begin();

  tft.init();
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.writecommand(0x36);
  tft.writedata(0xC0);
  tft.println("Systems check...");
}

void loop() {
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();
  long  dist = getDistance();
  bool  irR  = digitalRead(IR_RIGHT);  // active high so setting it low. 
  bool  irL  = digitalRead(IR_LEFT);

  

  // TFT output
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 10);
  tft.printf("Temp: %.1fC\n", temp);
  tft.printf(" Hum:  %.1f%%\n", hum);
  tft.printf(" Dist: %ld cm\n", dist);
  tft.printf(" IR R: %s\n", irR ? "EDGE!" : "ok");
  tft.printf(" IR L: %s\n", irL ? "EDGE!" : "ok");

  delay(500);
}