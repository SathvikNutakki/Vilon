#include <Arduino.h>
#include <TFT_eSPI.h>
#include <DHT.h>

TFT_eSPI tft = TFT_eSPI();
DHT dht(13,DHT22);

void setup() {
  Serial.begin(115200);
  delay(500);
  tft.init();
  tft.setRotation(1);
  tft.setCursor(0,0);
  tft.fillScreen(TFT_BLACK);
  dht.begin();

  
}

void loop() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);
  tft.drawString("TEMP :",20,10);
  tft.drawString(String(temp) + " C",20,50);
  tft.drawString("HUMIDITY: ",20,110);
  tft.drawString(String(hum) + " %",20,150);
  delay(2000);
}