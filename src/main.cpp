#include <Arduino.h>
#include <TFT_eSPI.h>
#include <DHT.h>

TFT_eSPI tft = TFT_eSPI();
DHT dht(13, DHT22);

void setup() {
  Serial.begin(115200);
  delay(500);

  tft.init();
  tft.setRotation(2);
  tft.writecommand(0x36);
  tft.writedata(0xC0);

  // --- SCREEN TEST ---
  // Full screen color fills
  tft.fillScreen(TFT_RED);   delay(800);
  tft.fillScreen(TFT_GREEN); delay(800);
  tft.fillScreen(TFT_BLUE);  delay(800);

  // Corner + center dots on black
  tft.fillScreen(TFT_BLACK);
  tft.fillCircle(0,   0,   6, TFT_WHITE);   // top-left
  tft.fillCircle(239, 0,   6, TFT_WHITE);   // top-right
  tft.fillCircle(0,   319, 6, TFT_WHITE);   // bottom-left
  tft.fillCircle(239, 319, 6, TFT_WHITE);   // bottom-right
  tft.fillCircle(120, 160, 6, TFT_YELLOW);  // center

  tft.setTextColor(TFT_WHITE);
  tft.drawString("TOP-LEFT",    10,  10, 2);
  tft.drawString("TOP-RIGHT",  140,  10, 2);
  tft.drawString("BTM-LEFT",    10, 305, 2);
  tft.drawString("BTM-RIGHT",  140, 305, 2);
  tft.drawString("NOT MIRRORED?", 30, 150, 2);
  delay(3000);
  // --- END TEST ---

  dht.begin();
  tft.fillScreen(TFT_BLACK);
}

void loop() {
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);
  tft.drawString("TEMP :",        20,  10);
  tft.drawString(String(temp) + " C",  20,  50);
  tft.drawString("HUMIDITY:",     20, 110);
  tft.drawString(String(hum)  + " %",  20, 150);

  delay(2000);
}