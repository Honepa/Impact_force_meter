#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
sensors_event_t event;

float time_seconds = 0;

void setup() 
{
  Serial.begin(115200);
  Serial.println("Accelerometer Test"); Serial.println("");
  if (!accel.begin())
  {
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while (1);
  }

}

void loop() 
{
  time_seconds = micros();
  accel.getEvent(&event);
  float z = event.acceleration.z;

}
