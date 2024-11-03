#include <Arduino.h>
#include <Wire.h>

#pragma once
// #ifndef __DS3231_H
// #define __DS3231_H

class DS3231 {  // class name
 public:
  DS3231(uint8_t pin); // The constructor has the same name as the class, it is always public, and it does not have any return value.
  bool begin(); 
  void getDate(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year);
  void setDate(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year);
  void iGetDate();
  void iSetDate();
  String iTimeStr();
  String iDateStr();
  static byte Get_MSB(); // Целое значение температуры
  static byte Get_LSB(); // Дробное значение температуры
  String Temp (void);
 private:   
  uint8_t _pin;  // _Private var accesable inside the class only
};

String formatDigit(int i, int len); // Add one or more zero before digit
byte decToBcd(byte val);
byte bcdToDec(byte val);

//#endif






