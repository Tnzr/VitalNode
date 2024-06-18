#pragma once
#include <Arduino.h>

enum MicType {
  ANALOG_MIC,
  DIGITAL_MIC
};

class Microphone {
  private:
    int pin;
    MicType micType;
    int sampleRate;
    int resolution;

  public:
    // Constructor to initialize the microphone
    Microphone(int micPin, MicType type = ANALOG_MIC, int rate = 1000, int res = 12);

    // Method to initialize the microphone
    void begin();

    // Method to read audio data
    int readAudio();

    // Method to get the latest audio sample
    int getSample();
};