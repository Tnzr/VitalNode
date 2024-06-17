#include <Arduino.h>
#include <Wire.h>
#include "Max3010xSensor.h"

Max3010xSensor bloodOxygen;

void setup() {
  Serial.begin(9600);
//   Wire.begin();
  bloodOxygen.begin();
}

void loop() {
    bloodOxygen.updateState();
    String jsonOutput1 = bloodOxygen.toJSONString();
    const StaticJsonDocument<200>& state = bloodOxygen.getLastState();

    Serial.println(jsonOutput1);
    bloodOxygen.plotVariables();
//   delay(200);
}
