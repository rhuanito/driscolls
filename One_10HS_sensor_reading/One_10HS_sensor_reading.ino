// No variables declared

void setup() {
  // Initialize serial communication at 57,600 bits per second
  Serial.begin(57600);

  // Set digital pins D22 as output to apply voltage to sensor
  pinMode(22, OUTPUT);
}

void loop() {

  // Start the 10HS measurements
  Serial.print("Taking 10HS measurements... ");

  // Apply power to 10HS sensors, wait 10 ms, measure analog output and turn power off
  // Set digital outputs high
  digitalWrite(22, HIGH);

  // Wait for taking a reading
  delay(10);

  // Read analog inputs
  int  sensorValue = analogRead(A15);

  // Set digital outputs low
  digitalWrite(22, LOW);

  // Convert the analog sensor value reading (which goes from 0 - 1023) to a voltage (0 - 5,000 mV)
  float voltage = sensorValue * (5.0 / 1.023);

  // Convert the voltage to dielectric permitivitty
  float Dielectric = 2.589 * pow(10, -10) * pow(voltage, 4) - 5.010 * pow(10, -7) * pow(voltage, 3) + 3.523 * pow(10, -4) * pow(voltage, 2) - 9.135 * pow(10, -2) * voltage + 7.457;

  // SOIL CALIBRATION: Decagon has generic calibrations (check the 10HS manual at http://manuals.decagon.com/Manuals/13508_10HS_Web.pdf) or you can determine your own calibration
  // Convert the voltage to volumetric water content (VWC) using a generic calibration equation for mineral soils
  float VWC = 2.97 * pow(10, -9) * pow(voltage, 3) - 7.37 * pow(10, -6) * pow(voltage, 2) + 6.69 * pow(10, -3) * voltage - 1.92;

  // On the next line, show the measured raw sensor value
  Serial.print("Sensor Value: ");
  Serial.print(sensorValue);
  Serial.print(", Voltage: ");
  Serial.print(voltage);
  Serial.print(" (mV), Dielectric: ");
  Serial.print(Dielectric);
  Serial.print(", VWC: ");
  Serial.print(VWC);
  Serial.print(" (m3/m3)");

  // Output the succesfull reading message to the serial monitor
  Serial.println(" All good!!!");

  // Run the program every second
  delay(1000);
}
