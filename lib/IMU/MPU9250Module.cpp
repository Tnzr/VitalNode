#include "MPU9250Module.h"

// Initialize static member
uint32_t MPU9250Module::_nextID = 1;

MPU9250Module::MPU9250Module(TwoWire& wire, uint8_t address)
    : _mpu(MPU9250()), _wire(wire), _id(_nextID++), _address(address) {
    memset(_accelBias, 0, sizeof(_accelBias));
    memset(_gyroBias, 0, sizeof(_gyroBias));
}

void MPU9250Module::begin() {
    _wire.begin();
    if (!_mpu.setup(_address)) {
        Serial.println("MPU9250 initialization unsuccessful");
        Serial.println("Check IMU wiring or try cycling power");
        while (1);
    }
    
    calibrateSensors();
}

void MPU9250Module::calibrateSensors() {
    const int samples = 1000;
    float accelSum[3] = {0, 0, 0};
    float gyroSum[3] = {0, 0, 0};
    
    Serial.println("Calibrating sensors, please keep the device stationary...");
    
    for (int i = 0; i < samples; i++) {
        while (!_mpu.update());
        
        accelSum[0] += _mpu.getAccX();
        accelSum[1] += _mpu.getAccY();
        accelSum[2] += _mpu.getAccZ();
        
        gyroSum[0] += _mpu.getGyroX();
        gyroSum[1] += _mpu.getGyroY();
        gyroSum[2] += _mpu.getGyroZ();
        
        delay(5); // Adjust the delay as needed to get reliable samples
    }
    
    _accelBias[0] = accelSum[0] / samples;
    _accelBias[1] = accelSum[1] / samples;
    _accelBias[2] = accelSum[2] / samples;
    
    _gyroBias[0] = gyroSum[0] / samples;
    _gyroBias[1] = gyroSum[1] / samples;
    _gyroBias[2] = gyroSum[2] / samples;
    _mpu.calibrateMag();
    Serial.println("Calibration done.");
}

void MPU9250Module::update() {
    if (_mpu.update()) {
        _imuState.accel[0] = _mpu.getAccX() - _accelBias[0];
        _imuState.accel[1] = _mpu.getAccY() - _accelBias[1];
        _imuState.accel[2] = _mpu.getAccZ() - _accelBias[2];

        _imuState.gyro[0] = _mpu.getGyroX() - _gyroBias[0];
        _imuState.gyro[1] = _mpu.getGyroY() - _gyroBias[1];
        _imuState.gyro[2] = _mpu.getGyroZ() - _gyroBias[2];
        
        _mpu.update_mag();
        _imuState.mag[0] = _mpu.getMagX();
        _imuState.mag[1] = _mpu.getMagY();
        _imuState.mag[2] = _mpu.getMagZ();
    }
}

String MPU9250Module::toJSON() const {
    JsonDocument doc;
    doc["id"] = _id;
    
    JsonObject accel = doc.createNestedObject("accelerometer");
    accel["x"] = _imuState.accel[0];
    accel["y"] = _imuState.accel[1];
    accel["z"] = _imuState.accel[2];
    
    JsonObject gyro = doc.createNestedObject("gyroscope");
    gyro["x"] = _imuState.gyro[0];
    gyro["y"] = _imuState.gyro[1];
    gyro["z"] = _imuState.gyro[2];

    JsonObject mag = doc.createNestedObject("magnetometer");
    mag["x"] = _imuState.mag[0];
    mag["y"] = _imuState.mag[1];
    mag["z"] = _imuState.mag[2];

    String output;
    serializeJson(doc, output);
    return output;
}

