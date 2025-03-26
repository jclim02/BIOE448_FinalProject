/*
BIOE 448 Final Project -- Step Counter
- Tracks user's steps using ADXL345 accelerometer
- Functions without computer connection
- Tracked metrics displayed on physical device
- Tracked metrics fetch-able from Bluetooth central
- Software shared in GitHub repo
- Device validated with test and benchmarks
*/

#include <Wire.h> // Necessary for I2C communication
#

int accel = 0x53; // I2C address for this sensor (from data sheet)
float x, y, z;
float total_accel;
float xavg, yavg, zavg;

void setup() {
  Serial.begin(9600);
  Wire.begin(); // Initialize serial communications
  Wire.beginTransmission(accel); // Start communicating with the device
  Wire.write(0x2D); // Enable measurement
  Wire.write(8); // Get sample measurement
  Wire.endTransmission();

  Wire.beginTransmission(accel);
  Wire.write(0x32); // Prepare to get readings for sensor (address from data sheet)
  Wire.endTransmission(false);
  Wire.requestFrom(accel, 6, true); // Get readings (2 readings per direction x 3 directions = 6 values)
  xavg = (Wire.read() | Wire.read() << 8); // Parse x values
  yavg = (Wire.read() | Wire.read() << 8); // Parse y values
  zavg = (Wire.read() | Wire.read() << 8); // Parse z values
  Wire.endTransmission();
}

void loop() {
  Wire.beginTransmission(accel);
  Wire.write(0x32); // Prepare to get readings for sensor (address from data sheet)
  Wire.endTransmission(false);
  Wire.requestFrom(accel, 6, true); // Get readings (2 readings per direction x 3 directions = 6 values)
  x = (Wire.read() | Wire.read() << 8); // Parse x values
  y = (Wire.read() | Wire.read() << 8); // Parse y values
  z = (Wire.read() | Wire.read() << 8); // Parse z values

  // Serial.print("x = "); // Print values
  // Serial.println(x);
  // Serial.print(", y = ");
  // Serial.print(y);
  // Serial.print(", z = ");
  // Serial.println(z);

  total_accel = sqrt(pow((x-xavg),2)+pow((y-yavg),2)+pow((z-zavg),2));
  // total_accel = sqrt(pow((x),2)+pow((y),2)+pow((z),2));
  // total_accel = sqrt(x+y+z);
  Serial.println(total_accel);

  delay(200);
}