#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
sensors_event_t event;

float time_seconds = 0;
long t0, dt, t, count = 0;
float a, v, dv, v0, e, ie, k1, k2 = 0;
float a_ave = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("a v"); 
  if (!accel.begin())
  {
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while (1);
  }

  v0 = 0;
  a_ave = 0;
  k1 = 1;
  k2 = 1;
  while(micros() < 1000000)
  {
    accel.getEvent(&event);
    a = event.acceleration.z - a_ave;
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
}

void loop()
{
  time_seconds = micros();
  accel.getEvent(&event);
  a = event.acceleration.z - a_ave;
  
  t = micros();
  dt = (t - t0);
  t0 = t;
  
  v += a * dt;
  //dv = v - v0;
  //v0 = v;  
  Serial.print(a * 10000);
  Serial.print(" ");
  //Serial.print(a_ave);
  Serial.print(" ");
  //Serial.print(count);
  //Serial.print(" ");
  Serial.print(v);
  Serial.print(" ");
  //Serial.print(dt);
  //Serial.print(" ");
  //Serial.print(dv);
  Serial.print("\n");
  
}
