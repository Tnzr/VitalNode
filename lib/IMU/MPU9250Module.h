#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <MPU9250.h>
#include <ArduinoJson.h>


struct IMUState {
    float accel[3];
    float gyro[3];
    float mag[3];
};

class MPU9250Module {
public:
    MPU9250Module(TwoWire& wire, uint8_t address = 0x68);
    void begin();
    void update();
    String toJSON() const;
    String instanceID;
    static int instanceCounter;

private:
    static uint32_t _nextID;
    uint32_t _id;
    MPU9250 _mpu;
    TwoWire& _wire;
    uint8_t _address;
    IMUState _imuState;
    float _accelBias[3];
    float _gyroBias[3];
    void calibrateSensors();
};
