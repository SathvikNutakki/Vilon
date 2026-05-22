#include <Arduino.h>

void setup(){
  ledcSetup(0,5000,8);
  ledcAttachPin(2,0);
  Serial.begin(115200);


}
void loop(){
  
  for (int k=0;k<3;k++)
  {
    for(int i=0;i<255;i++){
      ledcWrite(0,i);
      delay(10);
      
    }
    delay(100);
    for(int i=255;i>=0;i--){
      ledcWrite(0,i);
      delay(10);  
    }
    
    

  }
  while(true);
  
}