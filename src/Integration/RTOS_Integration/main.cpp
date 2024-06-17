#include <Arduino.h>
#include <Wire.h>
#include "Max3010xSensor.h"
#include <Arduino.h>
#include "GPSModule.h"
#include <TinyGPSPlus.h>
#include <MPU9250.h>
#include <Wire.h>
#include "MPU9250Module.h"
// TinyGPSPlus gps;

GPSModule gps(Serial2, 115200, GPSMode::RAN_DLT_SPOOF_GPS); // Assuming the GPS module is connected to Serial2 at 9600 baud
MPU9250Module imu(Wire);
Max3010xSensor bloodOxygen(100);

void gpsTask(void *pvParameters) {
  gps.begin();
  for (;;) {
    gps.update();
    Serial.println(gps.toJSON());
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void mpuTask(void *pvParameters) {
  imu.begin();
  for (;;) {
    imu.update();
    Serial.println(imu.toJSON());
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void max3010xTask(void *pvParameters) {
  bloodOxygen.begin();
  for (;;) {
    bloodOxygen.updateState();
    Serial.println(bloodOxygen.toJSONString());
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void setup() {
    // Hardware
    Serial.begin(115200);
    Wire.begin(21, 22); // Specify I2C pins

    while (!Serial) {}

    xTaskCreatePinnedToCore(gpsTask, "GPSTask", 4096*2, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(mpuTask, "MPUTask", 4096*2, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(max3010xTask, "Max3010xTask", 4096*2, NULL, 1, NULL, 1);
}

void loop() {}