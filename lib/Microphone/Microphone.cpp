#include "Microphone.h"

// Constructor to initialize the microphone
Microphone::Microphone(int micPin, MicType type, int rate, int res) {
  pin = micPin;
  micType = type;
  sampleRate = rate;
  resolution = res;
}

// Method to initialize the microphone
void Microphone::begin() {
  if (micType == ANALOG) {
    pinMode(pin, INPUT);
    analogReadResolution(resolution);
  } else {
    // Initialize I2S or other interfaces for digital microphone if needed
  }
}

// Method to read audio data
int Microphone::readAudio() {
  if (micType == ANALOG) {
    return analogRead(pin);
  } else {
    // Read data from digital microphone
    // Placeholder: replace with actual digital microphone read code
    return 0;
  }
}

// Method to get the latest audio sample
int Microphone::getSample() {
  return readAudio();
}
