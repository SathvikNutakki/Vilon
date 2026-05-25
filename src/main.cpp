#include <Arduino.h>

void setup(){
  Serial.begin(115200);
  
}
void loop(){
  if (Serial.available()){
     String incoming = Serial.readStringUntil('\n');
     Serial.println(incoming);
  }
  


}