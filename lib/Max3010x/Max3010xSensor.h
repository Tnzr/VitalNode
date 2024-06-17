#pragma once
#include <Wire.h>
#include <MAX30105.h>
#include <heartRate.h>
#include <spo2_algorithm.h>
#include <ArduinoJson.h>
#include <time.h>


struct BloodOxygenState {
    long red;
    long ir;
    long green;
    int32_t spo2;
    int32_t heartRate;
    int8_t validSPO2;
    int8_t validHeartRate;
};


class Max3010xSensor {
  public:
    String instanceID;
    static int instanceCounter;
    
    Max3010xSensor(int bufferLength = 50);
    ~Max3010xSensor();

    const StaticJsonDocument<200>& getMetrics() const;
    const StaticJsonDocument<200>& getLastState() const;
    void begin();
    String updateState();
    String toJSONString() const;
    void plotVariables() const;


  private:
    TaskHandle_t taskHandle;
    BloodOxygenState _oxygenState;
    uint32_t *irBuffer;
    uint32_t *redBuffer;
    int32_t bufferLength;
    StaticJsonDocument<200> CurrentState;
    String getTimestamp();
    void readSensorData();
    static void calculateHeartRateAndSpO2Task(void *pvParameters);
    
};
