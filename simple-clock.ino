/*
* Name: clock and temp project
* Author: Simon Olesen
* Date: 2024-10-22
* Description: This project uses a ds3231 to measure time and displays the time to an 1306 oled display,
* Further, it measures temprature with a analog temprature module and displays a mapped value to a 9g-servo-motor
*/

// Include libraries
#include <Servo.h>
#include "U8glib.h"
#include <RTClib.h>
#include <Wire.h>
RTC_DS3231 rtc;

#define DS3231_I2C_ADDR             0x68
#define DS3231_TEMPERATURE_ADDR     0x11

// Init constants
constexpr int btnPin{ 3 };

// Init global variables
bool buttonIsPressed{ false };
char t[32];

// Construct objects
Servo myServo;

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);

void setup() {
  pinMode(btnPin, INPUT);

  u8g.setFont(u8g_font_unifont);

  myServo.attach(9);
  myServo.write(0);
  
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));
}

void loop() 
{ 
  // oledWrite(getTime());
  // servoWrite(getTemp());
  toggleFunc();
  delay(1000);
}


/*
* This function reads time from an ds3231 module and package the time as a String
* Parameters: Void
* Returns: time in hh:mm:ss as String
*/
String getTime()
{
  DateTime now = rtc.now();
  sprintf(t, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());  

  return ("Time: " + String(t));
}

String getDate()
{
  DateTime now = rtc.now();

  return (String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()));
}

/*
* This function reads an analog pin connected to an analog temprature sensor and calculates the corresponding temp
* Parameters: Void
* Returns: temprature as String
*/
String getTemp()
{
  float rv;
    
  uint8_t temp_msb, temp_lsb;
  int8_t nint;

  Wire.beginTransmission(DS3231_I2C_ADDR);
  Wire.write(DS3231_TEMPERATURE_ADDR);
  Wire.endTransmission();

  Wire.requestFrom(DS3231_I2C_ADDR, 2);
  temp_msb = Wire.read();
  temp_lsb = Wire.read() >> 6;

  if ((temp_msb & 0x80) != 0)
      nint = temp_msb | ~((1 << 8) - 1); // if negative get two's complement
  else
      nint = temp_msb;

  rv = 0.25 * temp_lsb + nint;

  return (String(rv) + " Celsius");
}

/*
* This function takes a string and an optional string and draws it to an oled display
* Parameters: - text, text2: String to write to display, Optional String to write to display
* Returns: void
*/
void oledWrite(String text, String text2 = "")
{
  u8g.firstPage();

  do
  {
    u8g.drawStr(5, 20, text.c_str());
    u8g.drawStr(5, 40, text2.c_str());
  } while (u8g.nextPage());
}

/*
* takes a temprature value and maps it to corresppnding degree on a servo
* Parameters: - value: temperature
* Returns: void
*/
void servoWrite(float value)
{
  myServo.write(value);
}

/*
* Toggles what should be output to the oled display
* Parameters: Void
* Returns: void
*/
void toggleFunc()
{
  if (digitalRead(btnPin) == LOW)
  {
    buttonIsPressed = false;
    oledWrite(getTime(), getDate());
  }

  if ((digitalRead(btnPin) == HIGH) && !buttonIsPressed)
  {
    buttonIsPressed = true;
    oledWrite(String(getTemp()));
  }
}