#include "Arduino.h"
#include "Wire.h"
#include "RTCHelper.h"

RTCHelper::RTCHelper(byte rtcAddress)
{
  _rtcAddress = rtcAddress;
}


byte RTCHelper::read(byte registerAddress)
{
  Wire.beginTransmission(_rtcAddress);
  Wire.write(registerAddress);
  Wire.endTransmission();
  Wire.requestFrom(_rtcAddress, 1);
  return Wire.read();
}

void RTCHelper::write(byte registerAddress, byte value)
{
  Wire.beginTransmission(_rtcAddress);
  Wire.write(registerAddress);
  Wire.write(value);
  Wire.endTransmission();
}

int RTCHelper::bcdToDecimal(byte data)
{
  byte lowerDigit = data & 0xF;
  byte upperDigit = data >> 4;
  int result = upperDigit * 10 + lowerDigit;
  return result;
}

int RTCHelper::readSeconds()
{
  return bcdToDecimal(read(SEC_REG));
}

int RTCHelper::readMinutes()
{
  return bcdToDecimal(read(MIN_REG));
}

String RTCHelper::readHours()
{
  String resultFinal = "";
  byte data = read(HOUR_REG);
  byte lower = data & 0xF;
  int result = lower;
  int c = data >> 4;
  switch(c)
  {
    case 0:
      resultFinal += "0";
      resultFinal += result;
      break;
    case 1:
       result += 10;
       resultFinal += result;
       break;
    case 2:
       result += 20;
       resultFinal += result;
       break;
    case 4:
       resultFinal += "AM 0";
       resultFinal += result;
       break;
    case 5:
       result += 10;
       resultFinal += "AM ";
       resultFinal += result;
       break;
    case 6:
       resultFinal += "PM 0";
       resultFinal += result;
       break;
    case 7:
       result += 10;
       resultFinal += "PM ";
       resultFinal += result;
       break;
    default:
       break;
  }
  return resultFinal;
}

int RTCHelper::readDayOfMonth()
{
  return bcdToDecimal(read(DOM_REG));
}

int RTCHelper::readMonth()
{
  byte data = read(MON_REG) & 0x1F;
  return bcdToDecimal(data);
}

int RTCHelper::readYear()
{
  return bcdToDecimal(read(YEAR_REG)) + 2000;
}

float RTCHelper::readTemperature()
{
  int most = read(MSB_REG);
  float lost = (read(LSB_REG) >> 6) * 0.25;
  return most + lost;
}

String RTCHelper::format(int number)
{
  String result = "";
  if (number < 10)
    result += "0";
  result += number;
  return result;
}

String RTCHelper::getTime()
{
  String result = "";
  result += readHours();
  result += ":";
  result += format(readMinutes());
  result += ":";
  result += format(readSeconds());
  return result;
}

String RTCHelper::getDate()
{
  String result = "";
  result += format(readDayOfMonth());
  result += "/";
  result += format(readMonth());
  result += "/";
  result += readYear();
  return result;
}

void RTCHelper::writeAny(byte *addresses, int *data)
{
  for (int i=0; i<6; i++)
  {
    byte lower = data[i] % 10;
    byte upper = (data[i] / 10) << 4;
    byte bcd = lower | upper;
    write(addresses[i], bcd);
  }
}

byte RTCHelper::toBcd(int data)
{
  byte lower = data % 10;
  byte upper = (data / 10) << 4;
  byte bcd = lower | upper;
  return bcd;
}

void RTCHelper::setHour(int hour, int min, int sec, int type)
{
  byte h = toBcd(hour);
  Serial.print(h, BIN);
  switch(type)
  {
    case 0:  // format 12 hours AM
      h = h | 0x40;
      break;
    case 1:  // format 12 hours PM
      h = h | 0x60;
      break;
    default:
      break;
  }
  write(HOUR_REG, h);
  write(MIN_REG, toBcd(min));
  write(SEC_REG, toBcd(sec));
}

void RTCHelper::setDate(int dom, int mon, int year)
{
  write(DOM_REG, toBcd(dom));
  write(MON_REG, toBcd(mon));
  write(YEAR_REG, toBcd(year));
}

void RTCHelper::changeFormatHour()
{
  byte hour = read(HOUR_REG); 
  byte data = hour >> 5;
  byte result;
  if (data == 0x02) // 12 AM
  {
    if (hour == 0x52)
        result = 0x00;
    else
      result = hour & 0x1F;
  }
  else if (data == 0x03) // 12 PM
  {
    if (hour == 0x72)
      result = 0x12; //antes result = 12;
    else
      result = toBcd(bcdToDecimal(hour & 0x1F) + 12);
  }
  else // 24
  {
    int value = bcdToDecimal(hour);
    if ((value != 0) && (value != 12))
      result = value % 12;
    else
      result = 0x12;
    if (value >= 12)
      result = result | 0x60;
    else
      result = result | 0x40;
  }
  write(HOUR_REG, result);

}


void RTCHelper::setAlarm(int hour, int min, int sec, int type, int nAlarm)
{
  byte h = toBcd(hour);

  switch(type)
  {
    case 0:  // format 12 hours AM
      h = h | 0x40;  //format 12 hours AM
      break;
      
    case 1:  // format 12 hours PM
      h = h | 0x60; //format 12 hours PM
      break;
    default:
      break;
  }

  switch(nAlarm)
  {
    case 1:  // format 12 hours AM
      write(A1_HOUR_REG, h);
      write(A1_MIN_REG, toBcd(min));
      write(A1_SEC_REG, toBcd(sec));
      write(A1_DAY_REG, 0x80);
      write(CONTROL_REG, read(CONTROL_REG) | 0x05);
      break;
      
    case 2:  // format 12 hours PM
      write(A2_HOUR_REG, h);
      write(A2_MIN_REG, toBcd(min));
      write(A2_DAY_REG, 0x80);
      write(CONTROL_REG, (read(CONTROL_REG) | 0x06));
      break;
    default:
      break;
  }
  //write(CONTROL_REG, 0x1C);
}

