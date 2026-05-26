#include <Arduino.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

void setup(){
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.fillScreen(TFT_BLACK);
  
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(3);
  tft.drawString("HELLO ROBOT",20,20);
}
void loop(){
    tft.fillCircle(60,160,30,TFT_RED);
    tft.drawRect(20,110,80,100,TFT_WHITE);
    tft.fillCircle(180,160,30,TFT_RED);
    tft.drawRect(140,110,80,100,TFT_WHITE);
    delay(1000);
    tft.fillCircle(60,160,30,TFT_BLACK);
    tft.drawRect(20,110,80,100,TFT_WHITE);
    tft.fillCircle(180,160,30,TFT_BLACK);
    tft.drawRect(140,110,80,100,TFT_WHITE);
    delay(1000);
}