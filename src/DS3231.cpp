#include <Arduino.h>
#include <Wire.h>
#include <string.h>
#include "DS3231.h" // include header

const uint8_t DS3231_I2C_ADDR = 0x68;
const uint8_t DS3231_TEMPERATURE_MSB = 0x11;
const uint8_t DS3231_TEMPERATURE_LSB = 0x12;
struct Date_t { byte _second, _minute, _hour, _dayOfWeek, _dayOfMonth, _month, _year, _temperature;};

Date_t DSDate[1]; //lib internal date/time structure

// Constructor
DS3231::DS3231(uint8_t pin){
  //pinMode(pin, OUTPUT);
  _pin = pin;  // sample
}; 

bool DS3231::begin(){
 Serial.println(F("DS3231 > begin"));
 return true;
}

void DS3231::getDate(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year) { 
  Wire.beginTransmission(DS3231_I2C_ADDR);
  Wire.write(byte(0x00));
  Wire.endTransmission();
 
  Wire.requestFrom(DS3231_I2C_ADDR, 7);
  // A few of these need masks because certain bits are control bits
  *second     = bcdToDec(Wire.read() & 0x7f);
  *minute     = bcdToDec(Wire.read());
  *hour       = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek  = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month      = bcdToDec(Wire.read());
  *year       = bcdToDec(Wire.read());
}

void DS3231::setDate(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year) {
  // Use of (byte) type casting and ascii math to achieve result.
  /*second = (byte) ((Serial.read() - 48) * 10 + (Serial.read() - 48));
  minute = (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));
  hour  = (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));
  dayOfWeek = (byte) (Serial.read() - 48);
  dayOfMonth = (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));
  month = (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));
  year= (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));*/
  Wire.beginTransmission(DS3231_I2C_ADDR);
  Wire.write(byte(0x00));
  Wire.write(decToBcd(*second));  // 0 to bit 7 starts the clock
  Wire.write(decToBcd(*minute));
  Wire.write(decToBcd(*hour));    // If you want 12 hour am/pm you need to set
  // bit 6 (also need to change readDateDs1307)
  Wire.write(decToBcd(*dayOfWeek));
  Wire.write(decToBcd(*dayOfMonth));
  Wire.write(decToBcd(*month));
  Wire.write(decToBcd(*year));
  Wire.endTransmission();
}

void DS3231::iGetDate() { 
  Wire.beginTransmission(DS3231_I2C_ADDR);
  Wire.write(byte(0x00));
  Wire.endTransmission();
 
  Wire.requestFrom(DS3231_I2C_ADDR, 7);
  // A few of these need masks because certain bits are control bits
  DSDate[1]._second = bcdToDec(Wire.read() & 0x7f);
  DSDate[1]._minute = bcdToDec(Wire.read());
  DSDate[1]._hour = bcdToDec(Wire.read() & 0x3f);
  DSDate[1]._dayOfWeek = bcdToDec(Wire.read());
  DSDate[1]._dayOfMonth = bcdToDec(Wire.read());
  DSDate[1]._month = bcdToDec(Wire.read());
  DSDate[1]._year = bcdToDec(Wire.read());
}

void DS3231::iSetDate() {
  Wire.beginTransmission(DS3231_I2C_ADDR);
  Wire.write(byte(0x00));
  Wire.write(decToBcd( DSDate[1]._second));  // 0 to bit 7 starts the clock
  Wire.write(decToBcd( DSDate[1]._minute));
  Wire.write(decToBcd( DSDate[1]._hour));    // If you want 12 hour am/pm you need to set
  // bit 6 (also need to change readDateDs1307)
  Wire.write(decToBcd( DSDate[1]._dayOfWeek));
  Wire.write(decToBcd( DSDate[1]._dayOfMonth));
  Wire.write(decToBcd( DSDate[1]._month));
  Wire.write(decToBcd( DSDate[1]._year));
  Wire.endTransmission();
}

String DS3231::iTimeStr() { 
  String sTime;
  sTime = String(formatDigit(DSDate[1]._hour, 2) + ":" + formatDigit(DSDate[1]._minute, 2) + ":" + formatDigit(DSDate[1]._second, 2));
 return sTime;
}

String DS3231::iDateStr() { 
  String sDate;
  sDate = String(formatDigit(DSDate[1]._dayOfMonth, 2) + ":" + formatDigit(DSDate[1]._month, 2) + ":" + formatDigit(DSDate[1]._year, 2));
 return sDate;
}

byte DS3231::Get_MSB(){
  Wire.beginTransmission(DS3231_I2C_ADDR);
  Wire.write(DS3231_TEMPERATURE_MSB);
  Wire.endTransmission();

  Wire.requestFrom(DS3231_I2C_ADDR, 1);
  return  Wire.read();
}

byte DS3231::Get_LSB(){
  Wire.beginTransmission(DS3231_I2C_ADDR);
  Wire.write(DS3231_TEMPERATURE_LSB);
  Wire.endTransmission();

  Wire.requestFrom(DS3231_I2C_ADDR, 1);
  return  Wire.read() >> 6;
}

String DS3231::Temp() { 
  byte temp_msb, temp_lsb;
  String res = "";
  temp_msb = Get_MSB(); // Целая часть температуры
  temp_lsb = Get_LSB(); // Дробная часть температуры
  /*
  temp_msb = DS3231::Get_MSB(); // Целая часть температуры
  temp_lsb = DS3231::Get_LSB(); // Дробная часть температуры
  */
  res = String(temp_msb);

  switch(temp_lsb){
  case 0:
    res += ".00";
    break;
  case 1 :
    res += ".25";
    break;
  case 2:
    break;
    res += ".50";
  case 3:
    res += ".75";
    break;
  }
  return res;
}

String formatDigit(int i, int len) { // Add one or more zero before res
  String s = String(i);
  while (s.length() < len) {
    s = "0" + s; 
  }
  return (s);
}

byte decToBcd(byte val) { // Convert normal decimal numbers to binary coded decimal
  return ( (val / 10 * 16) + (val % 10) );
}

byte bcdToDec(byte val) { // Convert binary coded decimal to normal decimal numbers
  return ( (val / 16 * 10) + (val % 16) );
}
