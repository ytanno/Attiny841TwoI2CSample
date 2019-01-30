//Receive Side
#include <Wire.h>
#define ADDR 101 //start 101

union t_tag{
  byte b[4];
  float f;
} data;

void setup() 
{ 
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(ADDR); 
  Wire.write(0x03);
  Wire.endTransmission(); 
}

void act()
{
  
  Serial.print(ADDR);
  Serial.print(",");
  Wire.requestFrom(ADDR,8);  //
  int i=0;

  while(Wire.available()>0){
    data.b[i] = Wire.read();
    i++;
    if(i==4){
      Serial.print(data.f);
      Serial.print(",");
      i=0;
    }
  }

  Serial.println();
  
}


void loop() {
  act();
  delay(350);
}
