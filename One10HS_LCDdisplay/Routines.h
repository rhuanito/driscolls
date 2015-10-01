// Call the RTC
RTC_DS1307 rtc;

// LCD
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 20-chars and 4-line display
LiquidCrystal_I2C lcd(0x27, 20, 4);

// On the SD shield, chipSelect (CS) is digital pin D53
const int chipSelect = 53;

// Declare variables
int sensorValue, i;
float voltage, Dielectric, VWC, realVWC;

// ==================================================================================

boolean initRTC() {

  // Activate the RTC
  Serial.print("Initializing RTC... ");
  Wire.begin();
  rtc.begin();

  // Checks if RTC has been set and prevents time being overwritten if arduino is reset
  // EEPROM write/read syntax (EEPROM address between 0 and 511, value to be stored to EEPROM)
  int timeValue;
  int time = timeValue;
  time = EEPROM.read(0);
  if (time != timeValue) {
    rtc.adjust(DateTime(__DATE__, __TIME__));
    time = timeValue;
    EEPROM.write(0, time);
  }

  // Test if the RTC is working properly
  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
  }

  // Output the succesfull initialization message to the serial monitor
  Serial.println("RTC initialized.");
}

// ==================================================================================

boolean initSD() {

  // Set default chip select pin CS to output
  pinMode(53, OUTPUT);

  // Activate the SD card
  Serial.print("Initializing SD card... ");

  // See if the card is present and can be initialized
  if (!SD.begin()) {
    Serial.println();
    Serial.println();
    Serial.println("*******************************************");
    Serial.println("       Card failed, or not present         ");
    Serial.println("     WARNING: NO DATA WILL BE COLLECTED!   ");
    Serial.println("CHECK CARD AND ALL CONNECTIONS TO SD SHIELD");
    Serial.println("*******************************************");
  }
  else {
    // If SD card is available, send message to serial port
    Serial.println("SD card initialized.");
  }
}

// ==================================================================================

void initHeader() {

  // Write the file header
  Serial.print("Writting file header... ");

  // Open the data file on the SD card
  File dataFile = SD.open("PEP25-36.csv", FILE_WRITE);

  // If the file is available, write to it
  if (dataFile) {
    // Write the heading
    dataFile.println();
    dataFile.println();
    dataFile.print("Timestamp, ");
    dataFile.print("Raw sensor value #");
    dataFile.print(i);
    dataFile.print(", ");

    dataFile.print("Voltage #");
    dataFile.print(i);
    dataFile.print(" (mV), ");

    dataFile.print("Dielectric #");
    dataFile.print(i);
    dataFile.print(", ");

    dataFile.print("VWC #");
    dataFile.print(i);
    dataFile.print(" (m3 m-3), ");
  }
  dataFile.close();

  // Show a screen message to indicate the file header was writen
  Serial.println("file header written.");
  Serial.println();
}

// ==================================================================================

void PINoutput() {

  // Set digital pins D22 to D33 as outputs to apply voltage to all 12 sensors
  pinMode(22, OUTPUT);
}

// ==================================================================================

void meas10HS() {

  // Start the 10HS measurements
  Serial.print("Taking 10HS measurements... ");

  // Apply power to 10HS sensors, wait 10 ms, measure analog output and turn power off
  // Set digital outputs high
  digitalWrite(22, HIGH);

  // Wait for taking a reading
  delay(10);

  // Read analog inputs
  sensorValue = analogRead(15);

  // Set digital outputs low
  digitalWrite(22, LOW);

  // Output the succesfull reading message to the serial monitor
  Serial.println("complete.");
}

// ==================================================================================

void calc10HS() {

  // Start the calculations from the raw sensor values
  Serial.print("Starting 10HS calculations... ");

  // Calculations
  // Convert the analog sensor value reading (which goes from 0 - 1023) to a voltage (0 - 5,000 mV)
  voltage = sensorValue * (5.0 / 1.023);

  // Convert the voltage to dielectric permitivitty
  Dielectric = 2.589 * pow(10, -10) * pow(voltage, 4) - 5.010 * pow(10, -7) * pow(voltage, 3) + 3.523 * pow(10, -4) * pow(voltage, 2) - 9.135 * pow(10, -2) * voltage + 7.457;

  // SOIL CALIBRATION: Decagon has generic calibrations (check the 10HS manual at http://manuals.decagon.com/Manuals/13508_10HS_Web.pdf) or you can determine your own calibration
  // Convert the voltage to volumetric water content (VWC) using a generic calibration equation for mineral soils
  // VWC = 2.97 * pow(10, -9) * pow(voltage, 3) - 7.37 * pow(10, -6) * pow(voltage, 2) + 6.69 * pow(10, -3) * voltage - 1.92;
  VWC = 3.72 * pow(10,-9) * pow(voltage,3) - 7.83 *pow(10,-6)*pow(voltage,2) + 6.19 * pow(10,-3) * voltage - 1.5;

  // Output the succesfull reading message to the serial monitor
  Serial.println("complete.");
  Serial.println();
}

// ==================================================================================

boolean serialPRINT(String Date) {

  // Print the time stamp
  Serial.println(Date);

  // On the next line, show the measured raw sensor value
  Serial.print("Sensor Value (unitless): ");
  Serial.print(sensorValue);
  Serial.print(", Voltage (mV): ");
  Serial.print(voltage);
  Serial.print(", Dielectric (unitless): ");
  Serial.print(Dielectric);
  Serial.print(", VWC (m3/m3): ");
  Serial.print(VWC);

  // Add extra lines
  Serial.println();

  // Output the readings to the LCD screen
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("SensorValue=");
  lcd.print(sensorValue);
  lcd.print(" ");
  lcd.setCursor(0, 1);
  lcd.print("Voltage=");
  lcd.print(voltage);
  lcd.print(" ");
  lcd.setCursor(0, 2);
  lcd.print("VWC=");
  lcd.print(VWC);
  lcd.print(" or ");
  lcd.print(VWC * 100);
  lcd.print("%");
  lcd.print(" ");
  lcd.setCursor(0, 3);
  lcd.print("Dielectric=");
  lcd.print(Dielectric);
  lcd.print(" ");
}

// ==================================================================================

// Returns number of milliseconds to irrigate (required IRRIGTIME_UNIT, IRRIGATION_TIME as arguments)
int irrigTime(int value, int unit) {
  int n, v = value, u = unit;
  if (u == 1) {//seconds
    n = v * 1000;
  }
  if (u == 2) {//minutes
    n = v * 60000;
  }
  return n;
}

// ==================================================================================

boolean recordDATA(String Date) {

  // Start the procedure to record the data
  Serial.println();
  Serial.print("Recording data... ");

  // Open the data file on the SD card
  File dataFile = SD.open("PEP25-36.csv", FILE_WRITE);

  // If the file is available, write to it
  if (dataFile) {

    // Start with writing current time to the output file
    dataFile.println();
    dataFile.print(Date);
    dataFile.print(", ");

    // Write the raw sensor value (12 values) to the output file
    for (i = 0; i < 12; i++) {
      dataFile.print(sensorValue);
      dataFile.print(", ");
    }

    // Write the voltage (16 values) to the output file
    for (i = 0; i < 12; i++) {
      dataFile.print(voltage, 7);
      dataFile.print(", ");
    }

    // Write the dielectric (12 values) to the output file
    for (i = 0; i < 12; i++) {
      dataFile.print(Dielectric, 7);
      dataFile.print(", ");
    }

    // Write the soil VWC (12 values) to the output file
    for (i = 0; i < 12; i++) {
      dataFile.print(VWC, 7);
      dataFile.print(", ");
    }

    dataFile.close();

    // Print the data recording was succesfull and the data file has been closed
    Serial.println("Success! Closing file.");
  }
  else {
    // Print if the data file was not found
    Serial.println("ERROR: File not found.");
  }

  // Add some extra lines to the serial monitor screen
  Serial.println();
  Serial.println("************************************************************************");
  Serial.println();
}

// ==================================================================================

// Current time is formatted MM/DD/YYYY HH:MM:SS AM/PM
String genTimeStamp(int y, int m, int d, int h, int mn, int s) {
  String Period = "AM";
  int year = y, month = m, day = d, hour = h, minute = mn, second = s;
  // PM if H >= 12 || < 24
  if (hour >= 12 && hour < 24) {
    Period = "PM";
  }
  if (hour == 00) {
    hour = 12;
  }
  if (hour > 12) {
    hour = hour - 12;
  }

  String Year = String(year), Month = String(month), Day = String(day), Hour = String(hour), Minute = String(minute), Second = String(second);
  if (month < 10) {
    Month = "0" + Month;
  }
  if (day < 10) {
    Day = "0" + Day;
  }
  if (hour < 10) {
    Hour = "0" + Hour;
  }
  if (minute < 10) {
    Minute = "0" + Minute;
  }
  if (second < 10) {
    Second = "0" + Second;
  }
  return Month + "/" + Day + "/" + Year + " " + Hour + ":" + Minute + ":" + Second + " " + Period;
}

// ==================================================================================
/*
 * Author: Tim Hirzel   tim@growdown.com    March 2008
 * Converts a float to s String
 * Syntax  (array variable as buffer, float variable to convert, integer representing decimal precision) */
char * floatToString(char * outstr, float value, int places, int minwidth = 0, bool rightjustify = false) {
  // this is used to write a float value to string, outstr.  oustr is also the return value.
  int digit;
  float tens = 0.1;
  int tenscount = 0;
  int i;
  float tempfloat = value;
  int c = 0;
  int charcount = 1;
  int extra = 0;
  // make sure we round properly. this could use pow from <math.h>, but doesn't seem worth the import
  // if this rounding step isn't here, the value  54.321 prints as 54.3209

  // calculate rounding term d:   0.5/pow(10,places)
  float d = 0.5;
  if (value < 0)
    d *= -1.0;
  // divide by ten for each decimal place
  for (i = 0; i < places; i++)
    d /= 10.0;
  // this small addition, combined with truncation will round our values properly
  tempfloat +=  d;

  // first get value tens to be the large power of ten less than value
  if (value < 0)
    tempfloat *= -1.0;
  while ((tens * 10.0) <= tempfloat) {
    tens *= 10.0;
    tenscount += 1;
  }

  if (tenscount > 0)
    charcount += tenscount;
  else
    charcount += 1;

  if (value < 0)
    charcount += 1;
  charcount += 1 + places;

  minwidth += 1; // both count the null final character
  if (minwidth > charcount) {
    extra = minwidth - charcount;
    charcount = minwidth;
  }

  if (extra > 0 and rightjustify) {
    for (int i = 0; i < extra; i++) {
      outstr[c++] = ' ';
    }
  }

  // write out the negative if needed
  if (value < 0)
    outstr[c++] = '-';

  if (tenscount == 0)
    outstr[c++] = '0';

  for (i = 0; i < tenscount; i++) {
    digit = (int) (tempfloat / tens);
    itoa(digit, &outstr[c++], 10);
    tempfloat = tempfloat - ((float)digit * tens);
    tens /= 10.0;
  }

  // if no places after decimal, stop now and return

  // otherwise, write the point and continue on
  if (places > 0)
    outstr[c++] = '.';

  // now write out each decimal place by shifting digits one by one into the ones place and writing the truncated value
  for (i = 0; i < places; i++) {
    tempfloat *= 10.0;
    digit = (int) tempfloat;
    itoa(digit, &outstr[c++], 10);
    // once written, subtract off that digit
    tempfloat = tempfloat - (float) digit;
  }
  if (extra > 0 and not rightjustify) {
    for (int i = 0; i < extra; i++) {
      outstr[c++] = ' ';
    }
  }

  outstr[c++] = '\0';
  return outstr;
}

// ==================================================================================
