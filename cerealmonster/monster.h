#pragma once

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif
#include <Adafruit_VS1053.h>
#include <SD.h>
#include <Servo.h>
#include <SPI.h>

#include "constants.h"

/**This class handles the hardware on the monster. */
class Monster {
  private:
    Adafruit_VS1053_FilePlayer* m_musicPlayer;
    Adafruit_NeoPixel* m_pixels;
    Servo* m_leftEye;
    Servo* m_rightEye;
    int m_beamThreshold;
    int lightCalibration();
  public:
    Monster();
    void setup();
    void setEyeColor(uint8_t r, uint8_t g, uint8_t b);
    void setEyeColor(struct Color newColor);
    void setEyebrowPosition(int leftAngle, int rightAngle);
    void setEyebrowPosition(struct EyebrowPosition newPosition);
    void playSound(char* file);
    void stopSounds();
    bool isFed();
};