#pragma once
#include <Arduino.h>

class Speaker {
  private:
    int pin;
    int channel;
    int resolution;
    int frequency;

  public:
    // Constructor to initialize the speaker
    Speaker(int speakerPin, int pwmChannel, int pwmResolution = 8);

    // Method to play a tone at a specific frequency
    void playTone(int freq, int duration);

    // Method to stop the tone
    void stopTone();
};

