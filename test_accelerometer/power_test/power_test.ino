#include <Wire.h>

int ADXL345 = 0x53;

long t, dt, t0 = 0;
float maxI = 0;
float a0, a, a_min, a_max, a_max_imp0, a_max_imp, I, I0 = 0;
float Sz, Az = 0;
float ac, x, y, z, X_out, Y_out, Z_out;
float An = 100.0;
int fl = 0;

int count_min, count_max, count_maxI = 0;

float getZ()
{
  float z = 0;
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32);
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true);
  
  X_out = ( Wire.read()| Wire.read() << 8); // X-axis value
  X_out = X_out/256; //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
  Y_out = ( Wire.read()| Wire.read() << 8); // Y-axis value
  Y_out = Y_out/256;
  Z_out = ( Wire.read()| Wire.read() << 8); // Z-axis value
  Z_out = Z_out/256;
  
  x = X_out;
  y = Y_out;
  z = Z_out;
  ac = sqrt(x * x + y * y + z * z);
  Sz = Sz + ac - Az;
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

  
  t = millis();
}

void loop() 
{
  a = abs(getZ());

  if(a > 1.3)
  {
    if((a0 > a) and fl)
    {
      a_max++;
      fl = 0;
      a_max_imp = a;
    }
    if((a0 < a) and (a > a_max_imp))
    {
      fl  = 1;
    }
    if((a_max_imp0 < a_max_imp) and (a0 < a))
    {
      a_max--;
    }
  }
  else if (a < 1.3)
  {
    a_max_imp = 1.3;
  }
  a0 = a;
  a_max_imp0 = a_max_imp;
  Serial.print(a);
  Serial.print("  ");
  //Serial.print(a_max_imp);
  Serial.print("  ");
  //Serial.print(a_max);
  Serial.print('\n');
  //a_max = abs(a);
  /*
  if((a > 1.0) and (a > a0))
  {
    if(a >= a_max)
    {
      a_max = a;  
    }
  }
  if((a_max != 0) and (a < a_max) and (a < a0) and (a < 1.0))
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
  //Serial.print(" ");
  //Serial.print(a_min);
  //Serial.print(" ");
  //Serial.print(a_max_imp);
  Serial.print(" ");
  //Serial.print(a_min_imp);
  Serial.print(" ");
  //Serial.print(count_max);
  Serial.print(" ");
  //Serial.print(count_min);
  Serial.print("\n");
  a0 = a;
  */
}
