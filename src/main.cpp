#include <Arduino.h>
#define TRIG_PIN 5
#define ECHO_PIN 18



void setup(){
  Serial.begin(115200);
  pinMode(TRIG_PIN,OUTPUT);
  pinMode(ECHO_PIN,INPUT);
  pinMode(LEFT_IR,INPUT);
  pinMode(RIGHT_IR,INPUT);
  
}
void loop(){
  //for screaming
  digitalWrite(TRIG_PIN,LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN,HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN,LOW);
  //for hearing the echo
  long duration = pulseIn(ECHO_PIN,HIGH);
  float distance = (duration*0.034)/2;
  if (distance<20){
    Serial.println("OBSTACLE DETECTED!!");
  
  }
  else{
    Serial.print("DISTANCE: ");
    Serial.print(distance);
    Serial.println("CM");
  }
  delay(500);
  

}

