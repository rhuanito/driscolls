// Call libraries for the SD shield with built-in RTC (real time clock)
#include <SPI.h>       // Arduino built-in library
#include <SD.h>        // Arduino built-in library, needing to be updated at http://learn.adafruit.com/adafruit-data-logger-shield/downloads
#include <Wire.h>      // Arduino built-in library
#include <EEPROM.h>    // Arduino built-in library to write to persistant memory
#include "RTClib.h"    // Downloadable at http://learn.adafruit.com/adafruit-data-logger-shield/downloads
#include "Routines.h"  // Routines file
#include "config.h"    // Config file
#include <LiquidCrystal_I2C.h>

// Declare variables and initilize from config.h
String ProjectID = PROJECTID, ArduinoID = ARDUINOID;
int interval_Unit = INTERVAL_UNIT, Reading_Interval = READING_INTERVAL;

// ==================================================================================

// The setup routine runs once when the program first starts or when you press reset
void setup() {
  Serial.begin(57600);    // Initialize serial communication at 57,600 bits per second
  initRTC();              // Sync RTC with PC system time at upload
  initSD();               // Initialize the SD Shield
  initHeader();
  PINoutput();
  lcd.init();
}

// ==================================================================================

// Main loop executes runMain() every 1 second to check if time conditions are met
void loop() {
  runMain(Reading_Interval, interval_Unit);
  delay(1000);
}

// ==================================================================================

void runMain (int interval, int type) {
  DateTime now = rtc.now();
  int y = now.year(), m = now.month(), d = now.day(),  h = now.hour(),  mn = now.minute(),  s = now.second();
  if (type == 1) {//interval is in seconds
    if (s % interval == 0) {
      String Date = genTimeStamp(y, m, d, h, mn, s);
      Main(Date);
    }
  }
  if (type == 2) {//interval is in minutes
    if (mn % interval == 0 && s == 0) {
      String Date = genTimeStamp(y, m, d, h, mn, s);
      Main(Date);
    }
  }
  if (type == 3) { //interval is in hours
    if (h % interval == 0 && mn == 0 && s == 0) {
      String Date = genTimeStamp(y, m, d, h, mn, s);
      Main(Date);
    }
  }
}

// ==================================================================================

void Main(String Date) {
  meas10HS();
  calc10HS();
  serialPRINT(Date);
  recordDATA(Date);
  }

// ==================================================================================
