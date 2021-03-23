#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <Wire.h>

#define DEBUG true
#define Serial if(DEBUG)Serial

#define DEVICE (0x53)

#define ADXL345_MG2G_MULTIPLIER   (0.004)
#define SENSORS_GRAVITY_STANDARD  (SENSORS_GRAVITY_EARTH)
#define SENSORS_GRAVITY_EARTH     (9.80665F)


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

long t = 0;

float angle, x_1, y_1, z_1, x_0, y_0, z_0 = 0;

float angle0, now_angle, angle_max, angle_max_0 = 0;
float angle_const = 0.3;
int count_imp = 0;
int fl = 0;

long time_const = 100;

String frontend = "<head> <title>IFM - 06</title> <style> body{ background-size: 50px 50px; background-color: #ddd;background-image: -webkit-linear-gradient(transparent 50%, rgba(136, 136, 136, .2) 50%, rgba(136, 136, 136, .2)),-webkit-linear-gradient(0deg, transparent 50%, rgba(136, 136, 136, .2) 50%, rgba(136, 136, 136, .2));background-image: -moz-linear-gradient(transparent 50%, rgba(136, 136, 136, .2) 50%, rgba(136, 136, 136, .2)),-moz-linear-gradient(0deg, transparent 50%, rgba(136, 136, 136, .2) 50%, rgba(136, 136, 136, .2));background-image: linear-gradient(transparent 50%, rgba(136, 136, 136, .2) 50%, rgba(136, 136, 136, .2)),linear-gradient(90deg, transparent 50%, rgba(136, 136, 136, .2) 50%, rgba(136, 136, 136, .2));}</style></head>";

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

float get_angle()
{
  readAccel();
  angle = (x_0 * x_1 + y_0 * y_1 + z_0 * z_1) / (sqrtf(x_0 * x_0 + y_0 * y_0 + z_0 * z_0) * sqrtf(x_1 * x_1 + y_1 * y_1 + z_1 * z_1));

  //Serial.println(angle);
  //delay(100);
  x_0 = x_1;
  y_0 = y_1;
  z_0 = z_1;
  return angle;
}

const char* ssid = "IFM_0001";
const char* password = "ifmhonepa";
long t_red = 0;

//Wifi stat ip set
IPAddress ip(192, 168, 88, 205);
IPAddress geteway(192, 168, 88, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);

Ticker redirect_tic;
Ticker acc;

void setup()
{
  Serial.begin(115200);

  //Wifi setings

  WiFi.begin(ssid, password);
  WiFi.config(ip, geteway, subnet);

  while (WiFi.status() != WL_CONNECTED)
  {//counter i exit to error
    delay(250);
    Serial.print(".");
  }
  WiFi.softAPdisconnect(true);
  Serial.println(WiFi.localIP());
  Serial.println("HTTP server started");
  server.begin();

  t_red = millis();

  //acc setings
  Wire.begin();
  t = micros();

  writeTo(DATA_FORMAT, 0x00);
  writeTo(POWER_CTL, 0x08);
  readAccel();
  x_0 = x_1;
  y_0 = y_1;
  z_0 = z_1;
  t = millis();

  //acc.attach_ms(10, ticker_acc);
}

void loop()
{
  WiFiClient client = server.available();
  String request = client.readStringUntil('\r');
  if (client)
  {
    if (request.indexOf("/40_sec") != -1)
    {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("");
      client.println("<meta charset='utf-8'");
      client.println(frontend);
      client.println("<body>");
      client.println("<h1 align='center'> IFM - 06 </h1>");
      client.println("</body>");
      count_imp = 0;
      t_red = millis();
      while (millis() - t_red < 40000)
      {
        yield();
        now_angle = get_angle();
        if ((now_angle < angle_const) and (millis() - t > time_const))
        {
          count_imp++;
          fl = 1;
        }
        if ((now_angle > angle_const) and fl)
        {
          t = millis();
          fl = 0;
        }
        angle0 = now_angle;
        angle_max_0 = angle_max;
        Serial.println(count_imp);
      }
      client.println("<script>window.location.href = 'http://192.168.88.205'</script>");

    }
    else if (request.indexOf("/60_sec") != -1)
    {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("");
      client.println("<meta charset='utf-8'");
      client.println(frontend);
      client.println("<body>");
      client.println("<h1 align='center'> IFM - 06 </h1>");
      
      client.println("</body>");
      count_imp = 0;
      t_red = millis();
      while (millis() - t_red < 60000)
      {
        yield();
        now_angle = get_angle();
        if ((now_angle < angle_const) and (millis() - t > time_const))
        {
          count_imp++;
          fl = 1;
        }
        if ((now_angle > angle_const) and fl)
        {
          t = millis();
          fl = 0;
        }
        angle0 = now_angle;
        angle_max_0 = angle_max;
        Serial.println(count_imp);
      }
      client.println("<script>window.location.href = 'http://192.168.88.205'</script>");

    }
    else if (request.indexOf("/90_sec") != -1)
    {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("");
      client.println("<meta charset='utf-8'");
      client.println(frontend);
      client.println("<body>");
      client.println("<h1 align='center'> IFM - 06 </h1>");
      client.println("</body>");
      count_imp = 0;
      t_red = millis();
      while (millis() - t_red < 90000)
      {
        yield();
        now_angle = get_angle();
        if ((now_angle < angle_const) and (millis() - t > time_const))
        {
          count_imp++;
          fl = 1;
        }
        if ((now_angle > angle_const) and fl)
        {
          t = millis();
          fl = 0;
        }
        angle0 = now_angle;
        angle_max_0 = angle_max;
        Serial.println(count_imp);
      }
      client.println("<script>window.location.href = 'http://192.168.88.205'</script>");

    }
    else if (request.indexOf("/") != -1)
    {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("");
      client.println("<meta charset='utf-8'");
      client.println(frontend);
      client.println("<body>");
      client.println("<h1 align='center'> IFM - 06</h1>");
      client.println("");
      client.print("<p style='font-size: 100pt; text-align: center'>");
      client.print(count_imp);
      client.print("</p>");
      client.println("");
      client.println("<center>");
      client.println("<a href=\"40_sec\" style='text-decoration: none;'><button style='width:30%; height:20%; display:inline; margin:auto; background-color: PapayaWhip;'>40 секунд</button></a>");
      client.println("<a href=\"60_sec\" style='text-decoration: none;'><button style='width:30%; height:20%; display:inline; margin:auto; background-color: LightGreen;'>60 секунд</button></a>");
      client.println("<a href=\"90_sec\" style='text-decoration: none;'><button style='width:30%; height:20%; display:inline; margin:auto; background-color: LightBlue;'>90 секунд</button></a>");
      client.println("</center>");
      client.println("</body>");

    }
  }

  yield();
}
