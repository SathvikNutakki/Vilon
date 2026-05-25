#include <Arduino.h>

void setup(){
  Serial.begin(115200);
  
}
void loop(){
  if (Serial.available()){
     String incoming = Serial.readStringUntil('\n');
     incoming.trim();
     if (incoming == "FORWARD"){
        Serial.println("Moving Forward...");
     }
     else if (incoming == "STOP" ){
      Serial.println("Stopping.");
     }
     else if (incoming == "STATUS"){
      Serial.println("All systems nominal.");
     }
     else{
      Serial.println("Unknown command:" + incoming);
     }
     
  }
  


}