#include <Wire.h>

int ADXL345 = 0x53;

float time_seconds = 0;
long t0, dt, t, count = 0;
float a, v, dv, v0, e, ie, k1, k2 = 0;
float a_ave, a_min, a_max = 0;
float Sz, Az = 0;

float An = 125.0;

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

float getZ_()
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

  return z;
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

  v0 = 0;
  a_ave = 0;
  k1 = 0.00001;
  k2 = 0.00001;
  /*
  while(micros() < 1000000)
  {
    a = getZ();
    v += a;
    e = v - v0;
    ie += e;
    a_ave = k1 * e + k2 * ie;
    Serial.print(a);
    Serial.print(" ");
    Serial.print(a_ave);
    Serial.print(" ");
    //Serial.print(count);
    //Serial.print(" ");
    Serial.print(v);
    Serial.print(" ");
    Serial.print(dt);
    //Serial.print(" ");
    //Serial.print(dv);
    Serial.print("\n");
  }
  */
 v = 0; 
}

void loop()
{
  time_seconds = micros();
  a = getZ(); 
  t = micros();
  dt = (t - t0);
  t0 = t;
  v -= a * dt;
  v = v - dv;
  dv = v - v0;
  v0 = v;

  if(a > a_max)
  {
    a_max = a;
  }

  if(a < a_min)
  {
    a_min = a;
  }
  Serial.print(a);
  Serial.print(" ");
  Serial.print(a_max);
  Serial.print(" ");
  Serial.print(a_min);
  //Serial.print(" ");
  //Serial.print(v);
  Serial.print(" ");
  //Serial.print(dt);
  //Serial.print(" ");
  //Serial.print(dv);
  Serial.print("\n");
  
}
