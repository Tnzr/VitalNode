#include <Arduino.h>
#include <Wire.h>
#include "MAX30105.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

MAX30105 particleSensor;

float last_ir = 0;
double last_read = millis();
double now;
double dt;
float dIR;
int heartbeat;
float derivIR;
float ir;
double lastBeat;
double lastBPM;
float dipThreshold = -0.35;
SemaphoreHandle_t xSemaphore;

void readIR(void *pvParameters) {
  while (1) {
    if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
      ir = particleSensor.getIR();
      Serial.print(">InfraRed:");
      Serial.println(ir); // Send raw data to plotter
      dIR = ir - last_ir;
      now = millis();
      dt = now - last_read;
      last_ir = ir;
      last_read = now;
      xSemaphoreGive(xSemaphore);
    }
    vTaskDelay(10 / portTICK_PERIOD_MS); // Adjust delay as needed
  }
}

void processBeats(void *pvParameters) {
  while (1) {
    if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
      derivIR = dIR / dt;
      Serial.print(">Derivative:");
      Serial.println(derivIR); // Send raw data to plotter
      xSemaphoreGive(xSemaphore);
    }
    vTaskDelay(10 / portTICK_PERIOD_MS); // Adjust delay as needed
  }
}

void LogicalHeartBeat(void *pvParameters) {
  while (1) {
    if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
      bool newHeartBeat = derivIR < dipThreshold;
      if (!heartbeat && newHeartBeat) {
        double currentBeat = millis();
        if (lastBeat > 0) {
          double deltaBeat = (currentBeat - lastBeat) / 1000.0; // Convert to seconds
          if (deltaBeat > 0) {
            double bpm = 60.0 / deltaBeat;
            Serial.print(">HeartRate:");
            Serial.println((bpm+lastBPM)/2);
          }
        }
        lastBeat = currentBeat;
      }
      heartbeat = newHeartBeat;
      Serial.print(">HeartbeatDetection:");
      Serial.println(heartbeat); // Send raw data to plotter
      xSemaphoreGive(xSemaphore);
    }
    vTaskDelay(10 / portTICK_PERIOD_MS); // Adjust delay as needed
  }
}


void setup() {
  Serial.begin(9600);
  Serial.println("Initializing...");

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) { // Use default I2C port, 400kHz speed
    Serial.println("MAX30105 was not found. Please check wiring/power.");
    while (1);
  }

  // Setup to sense a nice looking saw tooth on the plotter
  byte ledBrightness = 0x1F; // Options: 0=Off to 255=50mA
  byte sampleAverage = 8; // Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 3; // Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  int sampleRate = 100; // Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; // Options: 69, 118, 215, 411
  int adcRange = 4096; // Options: 2048, 4096, 8192, 16384

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); // Configure sensor with these settings

  // Pre-populate the plotter so that the Y scale is close to IR values
  const byte avgAmount = 64;
  long baseValue = 0;
  for (byte x = 0; x < avgAmount; x++) {
    baseValue += particleSensor.getIR(); // Read the IR value
  }
  baseValue /= avgAmount;

  xSemaphore = xSemaphoreCreateMutex();

  xTaskCreate(readIR, "Infrared", 8192, NULL, 1, NULL);
  xTaskCreate(processBeats, "processing", 8192, NULL, 1, NULL);
  xTaskCreate(LogicalHeartBeat, "LogicalHeartBeat", 8192, NULL, 1, NULL);
}

void loop() {}
