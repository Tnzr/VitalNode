#pragma once
#include <Arduino.h>
#include <TinyGPS++.h>
#include <ArduinoJson.h>
#include <random>

enum class GPSMode {
    TRUE_GPS,
    RAN_DLT_SPOOF_GPS,
    STATIC_SPOOF,
};


struct GPSState {
    double latitude;
    double longitude;
    double altitude;
    unsigned long satellites;
};

class GPSModule {
public:
    GPSModule(HardwareSerial& serial, uint32_t baud, GPSMode mode);
    void begin();
    void update();
    void displayInfo();
    String toJSON();
    void setSpoofedCoordinates(double latitude, double longitude);
    void enabledSpoofing(bool enable);
    void updateSpoof(double newLongitude, double newLatitude);
    void setMode(GPSMode mode);
    String getTimestamp();
    // Since the main purpose of the GPS for now is to demo positional tracking, 
    // there is no need to give exact location, up to the user
    String instanceID;
    static int instanceCounter;

private:
    String sensorType;
    std::pair<double, double> generateCoordinates();
    HardwareSerial& _serial;
    TinyGPSPlus _gps;
    uint32_t _baud;
    const String _devicetype;
    double _latitude;
    double _longitude;
    double _altitude;
    unsigned long _satellites;
    double _spoofedLatitude;
    double _spoofedLongitude;
    bool _spoofingEnabled;
    double _latitudeDelta;
    double _longitudeDelta;
    std::random_device _rd;
    std::mt19937 _gen;
    GPSMode _mode;
};

