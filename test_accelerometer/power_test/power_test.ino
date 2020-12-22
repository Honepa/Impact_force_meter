#include <Wire.h>

int ADXL345 = 0x53;

float a0, a, a_min, a_max, a_min_imp, a_max_imp = 0;
float Sz, Az = 0;

float An = 125.0;

int count_min, count_max = 0;

float getZ()
{
  float z = 0;
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  
  z = ( Wire.read()| Wire.read() << 8);
  z = ( Wire.read()| Wire.read() << 8);
  z = ( Wire.read()| Wire.read() << 8);// Z-axis value
  z = z/256;
  z = (z / 0.97739) * 9.8;

  Sz = Sz + z - Az;
  Az = Sz / An;
  return Az;
}

void setup() 
{
  Serial.begin(115200);
  Serial.println("a v"); 

  Wire.begin(); // Initiate the Wire library
  // Set ADXL345 in measuring mode
  Wire.beginTransmission(ADXL345); // Start communicating with the device 
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  // Enable measurement
  Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
  Wire.endTransmission();
  delay(10);  

  Serial.print("a");
  Serial.print(" ");
  Serial.print("a_max");
  Serial.print(" ");
  Serial.print("a_min");
  Serial.print(" ");
  Serial.print("a_max_imp");
  Serial.print(" ");
  Serial.print("a_min_imp");
  Serial.print("\n");
  
}

void loop() 
{
  a = getZ();
  
  if((a > 0.15) and (a > a0))
  {
    if(a >= a_max)
    {
      a_max = a;  
    }
  }
  if((a_max != 0) and (a < a_max) and (a < a0) and (a < 0.15))
  {
    a_max_imp = a_max;
    a_max = 0;
    count_max++;
  }
  if((a < -1.0) and (a < a0))
  {
    if(a <= a_min)
    {
      a_min = a;  
    }
  }
  if((a_min != 0) and (a > a_min) and (a > a0) and (a > -1.0))
  {
    a_min_imp = a_min;
    a_min = 0;
    count_min++;
  }
  Serial.print(a);
  Serial.print(" ");
  Serial.print(a_max);
  Serial.print(" ");
  Serial.print(a_min);
  Serial.print(" ");
  Serial.print(a_max_imp);
  Serial.print(" ");
  Serial.print(a_min_imp);
  Serial.print(" ");
  Serial.print(count_max);
  Serial.print(" ");
  Serial.print(count_min);
  Serial.print("\n");
  a0 = a;
}
