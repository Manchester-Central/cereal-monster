#pragma once

#include <SD.h>
#include <SPI.h>

// AdaFruit MusicMaker mp3 player sheild
#define SHIELD_RESET -1  // VS1053 reset pin (unused!)
#define SHIELD_CS 7      // VS1053 chip select pin (output)
#define SHIELD_DCS 6
#define CARDCS 4  // Card chip select pin
#define DREQ 3    // VS1053 Data request, ideally an Interrupt pin // DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
// 2 x 24 LED AdaFruit NeoPixel rings (P1586)
#define PIXEL_PIN 9   // Which pin on the Arduino is connected to the NeoPixels?
#define NUMPIXELS 48  // Total number of pixels
//servos
#define LEFT_EYE 10
#define RIGHT_EYE 5
// photo-resistor
#define BEAM_SENSOR A0

// #define BEAM_THRESHOLD 600
// int BEAM_THRESHOLD = 600;
#define VOLUME 20

struct Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

struct EyebrowPosition {
  int leftAngle;
  int rightAngle;
};

// These state constants are needed in other files, so they are marked as `extern`, but still defined in their respective files
extern struct Color HANGRY_COLOR;
extern struct EyebrowPosition HANGRY_EYEBROWS;
extern struct Color HAPPY_COLOR;
extern struct EyebrowPosition HAPPY_EYEBROWS;

// Getting Hungry Constants

