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
#include <LiquidCrystal.h> // LCD
#include <ArduinoBLE.h> // BLE

// LCD
LiquidCrystal lcd(10, 9, 5, 4, 3, 2);

// Accelerometer
int accel = 0x53; // I2C address for this sensor (from data sheet)
float xavg, yavg, zavg; // reference values from boot
float x, y, z;
float x2, y2, z2;

// BLE
BLEService newService("180A"); // creating the service
BLEByteCharacteristic readChar("2A58", BLERead);
BLEByteCharacteristic writeChar("2A57", BLEWrite);

// Variables
float total_accel_1, total_accel_2;
float accel_diff;
float diff = 20000; // threshold difference
int step = 0; // step count
float distance = 0; // calculated distance
long previousMillis = 0;

void setup() {
  Serial.begin(9600);

  // For accelerometer
  Wire.begin(); // Initialize serial communications
  Wire.beginTransmission(accel); // Start communicating with the device
  Wire.write(0x2D); // Enable measurement
  Wire.write(8); // Get sample measurement
  Wire.endTransmission();

  // Reference accelerometer values on startup
  Wire.beginTransmission(accel);
  Wire.write(0x32); // Prepare to get readings for sensor (address from data sheet)
  Wire.endTransmission(false);
  Wire.requestFrom(accel, 6, true); // Get readings (2 readings per direction x 3 directions = 6 values)
  xavg = (Wire.read() | Wire.read() << 8); // Parse x values
  yavg = (Wire.read() | Wire.read() << 8); // Parse y values
  zavg = (Wire.read() | Wire.read() << 8); // Parse z values
  Wire.endTransmission();

  // BLE
  // for debugging
  while(!Serial); 
  if (!BLE.begin()){
    Serial.println("Waiting for ArduinoBLE");
    while(1);
  }
  // device setup
  BLE.setLocalName("StepTracker");
  BLE.setAdvertisedService(newService);
  newService.addCharacteristic(readChar);
  newService.addCharacteristic(writeChar);
  BLE.addService(newService);
  // instantiate read/write values
  readChar.writeValue(0);
  writeChar.writeValue(0);
  // advertise
  BLE.advertise();
  Serial.println("Bluetooth device active");
}

void loop() {
  // BLE
  BLEDevice central = BLE.central();

  if (central){
    Serial.print("Connected to central:");
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, HIGH);

    while (central.connected()){
      long currentMillis = millis();
      
      if (currentMillis - previousMillis >= 200) { 
        previousMillis = currentMillis;

        // Step and distance function
        // obtain first set of values
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

        // obtain second set of values
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

        // if the difference in values is greater than the threshold, count a step
        if (accel_diff > diff){
          step++;
        }

        distance = step*5280/2000;

        lcd.begin(16, 2); //Initiate the LCD in a 16x2 configuration
        lcd.print("Steps: ");
        lcd.print(step);
        lcd.setCursor(0, 1);
        lcd.print("Dist: ");
        lcd.print(distance);
        lcd.print("ft");
        Serial.println(distance);

        // BLE: to read from peripheral device
        readChar.writeValue(step);
        
        delay(100);
      } 
    }
  }
}