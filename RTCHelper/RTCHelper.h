#ifndef RTCHelper_h
#define RTCHelper_h

#include "Arduino.h"

#define SEC_REG      0x00
#define MIN_REG      0x01
#define HOUR_REG     0x02
#define DAY_REG      0x03
#define DOM_REG      0x04
#define MON_REG      0x05
#define YEAR_REG     0x06
#define A1_SEC_REG   0x07
#define A1_MIN_REG   0x08
#define A1_HOUR_REG  0x09
#define A1_DAY_REG   0x0A
#define A2_MIN_REG   0x0B
#define A2_HOUR_REG  0x0C
#define A2_DAY_REG   0x0D
#define CONTROL_REG  0x0E
#define STATUS_REG   0x0F
#define MSB_REG      0x11
#define LSB_REG      0x12
#define AM              0
#define PM              1

class RTCHelper
{
  public:
    RTCHelper(byte rtcAddress);
    byte read(byte registerAddress);
    void write(byte registerAddress, byte value);
    int bcdToDecimal(byte data);
    int readSeconds();
    int readMinutes();
    String readHours();
    int readDayOfMonth();
    int readMonth();
    int readYear();
    float readTemperature();
    void writeAny(byte *addresses, int *data);
    String format(int number);
    String getTime();
    String getDate();
    byte toBcd(int data);
    void setHour(int hour, int min, int sec, int type);
    void setDate(int dom, int mon, int year);
    void changeFormatHour();
    void setAlarm(int hour, int min, int sec, int type, int nAlarm);
  private:
    byte _rtcAddress;
};

#endif
