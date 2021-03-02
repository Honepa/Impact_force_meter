#include <Wire.h>
#include <ESP8266WiFi.h>

#define DEBUG true
#define Serial if(DEBUG)Serial

#define DEVICE (0x53)

#define ADXL345_MG2G_MULTIPLIER (0.004)
#define SENSORS_GRAVITY_STANDARD          (SENSORS_GRAVITY_EARTH)
#define SENSORS_GRAVITY_EARTH             (9.80665F)              /**< Earth's gravity in m/s^2 */

byte _buff[6];
char POWER_CTL = 0x2D;    //Power Control Register
char DATA_FORMAT = 0x31;
char DATAX0 = 0x32;    //X-Axis Data 0
char DATAX1 = 0x33;    //X-Axis Data 1
char DATAY0 = 0x34;    //Y-Axis Data 0
char DATAY1 = 0x35;    //Y-Axis Data 1
char DATAZ0 = 0x36;    //Z-Axis Data 0
char DATAZ1 = 0x37;    //Z-Axis Data 1

float max_x = 0;
float min_x = 0;
float cal_x = 0;
float x = 0;

float angle, x_1, y_1, z_1, x_0, y_0, z_0 = 0;

void readAccel()
{
  //Serial.print("readAccel");
  uint8_t howManyBytesToRead = 6; //6 for all axes
  readFrom( DATAX0, howManyBytesToRead, _buff); //read the acceleration data from the ADXL345
  short x = 0;
  x = (((short)_buff[1]) << 8) | _buff[0];
  short y = (((short)_buff[3]) << 8) | _buff[2];
  short z = (((short)_buff[5]) << 8) | _buff[4];
  //Serial.print(x * ADXL345_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD);
  //Serial.print("  ");
  //Serial.print(y * ADXL345_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD);
  //Serial.print("  ");
  //Serial.print(z * ADXL345_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD);
  //Serial.print('\n');
  //return x * ADXL345_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD;
  //x = x + cal_x;
  x_1 = x * ADXL345_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD;
  y_1 = y * ADXL345_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD;
  z_1 = z * ADXL345_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD;

  //Serial.print("x: ");
  //Serial.print( x*2./512 );
  //Serial.print(" y: ");
  //Serial.print( y*2./512 );
  //Serial.print(" z: ");
  //Serial.print( z*2./512 );
  //Serial.print("X: "); Serial.print( x);

  //Serial.println( sqrtf(x*x+y*y+z*z)*2./512 );

  //getX() = read16(ADXL345_REG_DATAX0);
  //x = getX() * ADXL345_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD;

}

void writeTo(byte address, byte val)
{
  Wire.beginTransmission(DEVICE); // start transmission to device
  Wire.write(address); // send register address
  Wire.write(val); // send value to write
  Wire.endTransmission(); // end transmission
}

void readFrom(byte address, int num, byte _buff[])
{
  Wire.beginTransmission(DEVICE); // start transmission to device
  Wire.write(address); // sends address to read from
  Wire.endTransmission(); // end transmission
  Wire.beginTransmission(DEVICE); // start transmission to device
  Wire.requestFrom(DEVICE, num); // request 6 bytes from device

  int i = 0;
  while (Wire.available()) // device may send less than requested (abnormal)
  {
    _buff[i] = Wire.read(); // receive a byte
    i++;
  }
  Wire.endTransmission(); // end transmission
}

void setup()
{
  Serial.begin(115200);
  //ADXL345
  // i2c bus SDA = GPIO0; SCL = GPIO2
  Wire.begin();

  // Put the ADXL345 into +/- 2G range by writing the value 0x01 to the DATA_FORMAT register.
  // FYI: 0x00 = 2G, 0x01 = 4G, 0x02 = 8G, 0x03 = 16G
  writeTo(DATA_FORMAT, 0x00);

  // Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register.
  writeTo(POWER_CTL, 0x08);
  /*
  int i = 0;
  for (i = 0; i < 11; i++)
  {
    //uint8_t howManyBytesToRead = 6;
    //readFrom( DATAX0, howManyBytesToRead, _buff);
    float calib_x ;//= (((short)_buff[1]) << 8) | _buff[0];
    calib_x = readAccel();
    //if(i==0)
    // cal_x = x;
    if (i > 0)
      cal_x = cal_x + calib_x;
    Serial.println(calib_x);
    delay(100);
  }

  cal_x = cal_x / 10;
  Serial.print("cal_x: "); Serial.println(cal_x);
  */
  readAccel();
  x_0 = x_1;
  y_0 = y_1;
  z_0 = z_1;
}

void loop()
{
  readAccel();
  angle = (x_0 * x_1 + y_0 * y_1 + z_0 * z_1) / (sqrtf(x_0 * x_0 + y_0 * y_0 + z_0 * z_0) * sqrtf(x_1 * x_1 + y_1 * y_1 + z_1 * z_1));
  
  Serial.println(angle);
  delay(100);
  x_0 = x_1;
  y_0 = y_1;
  z_0 = z_1;
}
