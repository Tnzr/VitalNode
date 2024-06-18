#include "DCSpeaker.h"

// Constructor to initialize the speaker
Speaker::Speaker(int speakerPin, int pwmChannel, int pwmResolution) {
  pin = speakerPin;
  channel = pwmChannel;
  resolution = pwmResolution;
  frequency = 0;

  // Configure the PWM functionalities
  ledcSetup(channel, frequency, resolution);
  ledcAttachPin(pin, channel);
}

// Method to play a tone at a specific frequency
void Speaker::playTone(int freq, int duration) {
  frequency = freq;
  ledcWriteTone(channel, frequency);
  delay(duration);
  stopTone();
}

// Method to stop the tone
void Speaker::stopTone() {
  ledcWrite(channel, 0);
}
