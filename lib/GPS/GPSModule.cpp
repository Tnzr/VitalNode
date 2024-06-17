#include "GPSModule.h"

int GPSModule::instanceCounter = 0;

GPSModule::GPSModule(HardwareSerial& serial, uint32_t baud, GPSMode mode)
    : _serial(serial), _baud(baud), _latitude(0), _longitude(0), _altitude(0),
      _satellites(0), _spoofingEnabled(0), _latitudeDelta(0), _longitudeDelta(0), _gen(_rd()) {
        instanceCounter++;
        setMode(mode);
    }

std::pair<double, double> GPSModule::generateCoordinates(){
    
    // Latitude ranges from -90 to 90
    std::uniform_real_distribution<double> latDist(-90.0, 90.0);
    
    // Longitude ranges from -180 to 180
    std::uniform_real_distribution<double> lonDist(-180.0, 180.0);

    double latitude = latDist(_gen);
    double longitude = lonDist(_gen);
    
    return std::make_pair(latitude, longitude);
}

void GPSModule::updateSpoof(double newLongitude, double newLatitude){
    _longitudeDelta = newLongitude - _longitude;
    _latitudeDelta = newLatitude - _latitude;
}

void GPSModule::setMode(GPSMode mode) {
    _mode = mode;
    
    std::pair<double, double> location;
    double lat, lon;

    switch (mode) {
    case GPSMode::RAN_DLT_SPOOF_GPS:
        location = generateCoordinates();
        lat = location.first;
        lon = location.second;
        updateSpoof(lon, lat);
        break;
    case GPSMode::STATIC_SPOOF:
        // Implement STATIC_SPOOF functionality here
        break;
    default:
        break;
    }
}

void GPSModule::begin() {
    _serial.begin(_baud);
}

void GPSModule::update() {
    while (_serial.available()) {    
        // Serial.write(_serial.read());//Forward what Serial received to Software Serial Port
        char c = _serial.read();
        // Serial.write(c);
        _gps.encode(c);
    }
    if (_gps.location.isUpdated()) {
        switch (_mode)
        {
        case GPSMode::TRUE_GPS:
            _latitude = _gps.location.lat();
            _longitude = _gps.location.lng();
            _altitude = _gps.altitude.meters();
            _satellites = _gps.satellites.value();
            break;

        case GPSMode::RAN_DLT_SPOOF_GPS:
            _latitude = _gps.location.lat() + _latitudeDelta ;
            _longitude = _gps.location.lng() + _latitudeDelta ;
            _altitude = _gps.altitude.meters();
            _satellites = _gps.satellites.value();
            break;
        
        default:
            break;
        }

    }
}

void GPSModule::displayInfo() {
    Serial.print(F("Location: "));
    if (_gps.location.isValid()) {
        Serial.print(_gps.location.lat(), 6);
        Serial.print(F(","));
        Serial.print(_gps.location.lng(), 6);
    } else {
        Serial.print(F("INVALID"));
    }
    Serial.print(F("  Date/Time: "));
    if (_gps.date.isValid()) {
        Serial.print(_gps.date.month());
        Serial.print(F("/"));
        Serial.print(_gps.date.day());
        Serial.print(F("/"));
        Serial.print(_gps.date.year());
    } else {
        Serial.print(F("INVALID"));
    }
    Serial.print(F(" "));
    if (_gps.time.isValid()) {
        if (_gps.time.hour() < 10) Serial.print(F("0"));
        Serial.print(_gps.time.hour());
        Serial.print(F(":"));
        if (_gps.time.minute() < 10) Serial.print(F("0"));
        Serial.print(_gps.time.minute());
        Serial.print(F(":"));
        if (_gps.time.second() < 10) Serial.print(F("0"));
        Serial.print(_gps.time.second());
        Serial.print(F("."));
        if (_gps.time.centisecond() < 10) Serial.print(F("0"));
        Serial.print(_gps.time.centisecond());
    } else {
        Serial.print(F("INVALID"));
    }
    Serial.println();
}

String GPSModule::toJSON() {
    JsonDocument doc;
    doc["timestamp"] = getTimestamp();
    doc["serialNumber"] = instanceCounter;
    doc["device_name"] = _devicetype + "_"+ String(instanceCounter);
    doc["latitude"] = _latitude;
    doc["longitude"] = _longitude;
    doc["altitude"] = _altitude;
    doc["satellites"] = _satellites;
    String output;
    serializeJson(doc, output);
    return output;
}


String GPSModule::getTimestamp() {
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  char timestamp[20];
  sprintf(timestamp, "%04d-%02d-%02d %02d:%02d:%02d",
          p_tm->tm_year + 1900, p_tm->tm_mon + 1, p_tm->tm_mday,
          p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);
  return String(timestamp);
}