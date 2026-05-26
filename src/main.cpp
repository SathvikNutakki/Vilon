#include <Arduino.h>
#define LEFT_IR 35
#define RIGHT_IR 34
#define TRIG_PIN 5
#define ECHO_PIN 18

void setup(){
  Serial.begin(115200);
  
  pinMode(LEFT_IR,INPUT);
  pinMode(RIGHT_IR,INPUT);
  pinMode(TRIG_PIN,OUTPUT);
  pinMode(ECHO_PIN,INPUT);
}


void CHECK_CLIFF(){
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
  
}
void CHECK_OBSTACLE(){
  digitalWrite(TRIG_PIN,LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN,HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN,LOW);

  long duration = pulseIn(ECHO_PIN,HIGH);
  float distance = duration*0.034/2;
  if (distance<10){
    Serial.println("OBSTACLE DETECTED!");

  }
  else{
    Serial.print("Distance : ");
    Serial.print(distance);
    Serial.println(" CM");
    
  }
  
}
void loop(){
  CHECK_OBSTACLE();
  CHECK_CLIFF();
  delay(300);

}