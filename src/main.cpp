#include <Arduino.h>
#define LEFT_IR 35
#define RIGHT_IR 34


void setup(){
  Serial.begin(115200);
  
  pinMode(LEFT_IR,INPUT);
  pinMode(RIGHT_IR,INPUT);
  
}


void loop(){
  int LEFT = digitalRead(LEFT_IR);
  int RIGHT = digitalRead(RIGHT_IR);
  if (LEFT == 0 && RIGHT == 0){
    Serial.println("ALL CLEAR");

  }
  else if (LEFT == 1 && RIGHT == 0){
    Serial.println("LEFT CLIFF DETECTED");
  }
  else if (LEFT == 0 && RIGHT == 1){
    Serial.println("RIGHT CLIFF DETECTED");
  }
  else{
    Serial.println("BOTH CLIFFS DETECTED, STOP.");
  }
  delay(200);
}