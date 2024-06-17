#include <Arduino.h>
#include "GPSModule.h"
#include <TinyGPSPlus.h>
#include <MPU9250.h>
#include <Wire.h>
#include "MPU9250Module.h"
// TinyGPSPlus gps;

GPSModule gps(Serial2, 9600, GPSMode::RAN_DLT_SPOOF_GPS); // Assuming the GPS module is connected to Serial2 at 9600 baud
MPU9250Module imu(Wire);

void setup() {
    Serial.begin(9600);
      while (!Serial) {}
    Wire.begin(21, 22); // Specify I2C pins
    gps.begin();
  imu.begin();
}

void loop() {
    gps.update();
    imu.update();

    // gps.displayInfo();
    String jsonString = gps.toJSON();
    Serial.println(jsonString);
    // display the data
    Serial.println(imu.toJSON());
    delay(100);
}

