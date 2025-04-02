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

int accel = 0x53; // I2C address for this sensor (from data sheet)
float x, y, z;
float x2, y2, z2;
float x3, y3, z3;
float total_accel_1, total_accel_2, total_accel_3;
float accel_diff, accel_diff_2;
float diff = 20000;
float xavg, yavg, zavg;
bool base_1, base_2;
// float diff_1, diff_2;
int step = 0;
int timeout = 0;

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

  total_accel_1 = sqrt(pow((x-xavg),2)+pow((y-yavg),2)+pow((z-zavg),2));
  // Serial.println(total_accel_1);

  delay(50);

  Wire.beginTransmission(accel);
  Wire.write(0x32); // Prepare to get readings for sensor (address from data sheet)
  Wire.endTransmission(false);
  Wire.requestFrom(accel, 6, true); // Get readings (2 readings per direction x 3 directions = 6 values)
  x2 = (Wire.read() | Wire.read() << 8); // Parse x values
  y2 = (Wire.read() | Wire.read() << 8); // Parse y values
  z2 = (Wire.read() | Wire.read() << 8); // Parse z values

  total_accel_2 = sqrt(pow((x2-xavg),2)+pow((y2-yavg),2)+pow((z2-zavg),2));
  // Serial.println(total_accel_2);

  accel_diff = abs(total_accel_2 - total_accel_1);

  if (accel_diff>diff){
    step++;
  }

  Serial.println(step);

  delay(100);

}