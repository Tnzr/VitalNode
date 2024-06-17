#include "Max3010xSensor.h"

MAX30105 particleSensor;
int Max3010xSensor::instanceCounter = 0;

Max3010xSensor::Max3010xSensor(int bufferLength) : bufferLength(bufferLength) {
  instanceCounter++;
  instanceID = "MAX30102_" + String(instanceCounter);
  redBuffer = new uint32_t[bufferLength];
  irBuffer = new uint32_t[bufferLength];
    if (redBuffer == nullptr || irBuffer == nullptr) {
    Serial.println("Error: Failed to allocate memory for buffers.");
    while (1); // Halt execution if memory allocation fails
  }
  xTaskCreatePinnedToCore(calculateHeartRateAndSpO2Task, "HR_SpO2", 4096*2, this, 1, &this->taskHandle, 1);
}

Max3010xSensor::~Max3010xSensor() {
  delete[] redBuffer;
  delete[] irBuffer;
}

void Max3010xSensor::begin() {
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 was not found. Please check wiring/power.");
    while (1);
  }
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A); // Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); // Turn off Green LED
}

void Max3010xSensor::readSensorData() {
  if (!redBuffer || !irBuffer) {
    Serial.println("Error: Buffers not allocated.");
    return;
  }

  _oxygenState.ir = particleSensor.getIR();
  _oxygenState.red = particleSensor.getRed();
  _oxygenState.green = particleSensor.getGreen();

  for (int i = 0; i < bufferLength; i++) {
    while (particleSensor.available() == false) { // Wait until new data is available
      particleSensor.check(); // Check the sensor for new data
    }
    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); // Move to next sample
  }
}

void Max3010xSensor::calculateHeartRateAndSpO2Task(void *pvParameters) {
  Max3010xSensor* sensor = static_cast<Max3010xSensor*>(pvParameters);
  for (;;) {
    sensor->readSensorData();
    maxim_heart_rate_and_oxygen_saturation(
      sensor->irBuffer,
      sensor->bufferLength,
      sensor->redBuffer,
      &sensor->_oxygenState.spo2,
      &sensor->_oxygenState.validSPO2,
      &sensor->_oxygenState.heartRate,
      &sensor->_oxygenState.validHeartRate
    );
    vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
  }
}

const StaticJsonDocument<200>& Max3010xSensor::getMetrics() const {
  return CurrentState;
}

String Max3010xSensor::updateState() {
  readSensorData();
  maxim_heart_rate_and_oxygen_saturation();
  // Update JSON Object
  CurrentState.clear();
  CurrentState["timestamp"] = getTimestamp();
  CurrentState["id"] = instanceID;
  CurrentState["serialNumber"] = instanceCounter;
  CurrentState["red"] = _oxygenState.red;
  CurrentState["ir"] = _oxygenState.ir;
  CurrentState["green"] = _oxygenState.green;
  CurrentState["spo2"] = _oxygenState.validSPO2 ? _oxygenState.spo2 : -1; // Use -1 if invalid
  CurrentState["heartRate"] = _oxygenState.validHeartRate ? _oxygenState.heartRate : -1; // Use -1 if invalid

  String output;
  serializeJson(CurrentState, output);
  return output;
}

String Max3010xSensor::toJSONString() const {
  String output;
  serializeJson(CurrentState, output);
  return output;
}

void Max3010xSensor::plotVariables() const {
  Serial.print(">RED:"); Serial.println(_oxygenState.red);
  Serial.print(">IR:"); Serial.println(_oxygenState.ir);
  Serial.print(">Green:"); Serial.println(_oxygenState.green);
  Serial.print(">SpO2:"); Serial.println(_oxygenState.spo2);
  Serial.print(">HeartBeat:"); Serial.println(_oxygenState.heartRate);
}

const StaticJsonDocument<200>& Max3010xSensor::getLastState() const {
  return CurrentState;
}

String Max3010xSensor::getTimestamp() {
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  char timestamp[20];
  sprintf(timestamp, "%04d-%02d-%02d %02d:%02d:%02d",
          p_tm->tm_year + 1900, p_tm->tm_mon + 1, p_tm->tm_mday,
          p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);
  return String(timestamp);
}
