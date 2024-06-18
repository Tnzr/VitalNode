#include <Arduino.h>
// #include "DCSpeaker.h"
#include "Microphone.h"

const int micPin = 4; // ADC pin connected to the electret microphone
const int speakerPin = 25; // PWM pin connected to the speaker
const int pwmChannel = 0; // PWM channel
const int pwmResolution = 8; // PWM resolution
const int adcResolution = 12; // ADC resolution
// Speaker speaker(2, 0); // Initialize speaker on GPIO 25 with PWM channel 0
Microphone mic(4, ANALOG_MIC); // Initialize analog microphone on GPIO 34

void setup() {
  Serial.begin(115200);
  mic.begin(); // Initialize the microphone
    analogReadResolution(adcResolution);

}

void loop() {
  // int audioValue = mic.getSample(); // Read audio value from the microphone
    int audioValue = analogRead(micPin);

  // int pwmValue = map(audioValue, 0, 4095, 0, 255); // Map ADC value to PWM range
  int pwmValue = map(audioValue, 0, (1 << adcResolution) - 1, 0, (1 << pwmResolution) - 1);

  // speaker.outputPWM(pwmValue); // Output PWM value to speaker

  // Optional: Print the audio value for debugging
  Serial.print(">Mic:");
  Serial.println(audioValue);

  delay(1); // Short delay to allow PWM to stabilize
}
