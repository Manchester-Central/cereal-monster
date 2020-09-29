#include "Adafruit_NeoPixel.h"
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#include <Servo.h>

#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// AdaFruit MusicMaker mp3 player sheild
#define SHIELD_RESET -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6
#define CARDCS        4      // Card chip select pin
#define DREQ          3      // VS1053 Data request, ideally an Interrupt pin // DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
// 2 x 24 LED AdaFruit NeoPixel rings (P1586)
#define PIXEL_PIN     9      // Which pin on the Arduino is connected to the NeoPixels?
#define NUMPIXELS    48      // Total number of pixels
//servos
#define LEFT_EYE     10
#define RIGHT_EYE     5
// photo-resistor
#define BEAM_SENSOR  A0

#define BEAM_THRESHOLD 120
#define VOLUME         20

Servo leftEye;
Servo rightEye;

enum State_T {
  hangry,
  chewing,
  happy
};

Adafruit_VS1053_FilePlayer musicPlayer =
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

Adafruit_NeoPixel pixels(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

State_T state;
unsigned long stateStartTime;

void setup() {

  leftEye.attach(LEFT_EYE, 500, 2500);
  rightEye.attach(RIGHT_EYE, 500, 2500);

  pixels.begin();
  Serial.begin(9600);

  pinMode(BEAM_SENSOR, INPUT);

  if (! musicPlayer.begin()) { // initialise the music player
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    while (1);
  }
  Serial.println(F("VS1053 found"));

  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }

  musicPlayer.setVolume(VOLUME, VOLUME);
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);
  changeState(hangry);
}

void eyecolor(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUMPIXELS; i++) { // For each pixel...
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
}

const char * stateName(State_T s) {
  if (s == hangry) {
    return "hangry";
  } else if (s == chewing) {
    return "chewing";
  } else if (s == happy) {
    return "happy";
  } else {
    return "(unknown)";
  }
}

void changeState(State_T newState) {
  Serial.print("State transition: ");
  Serial.print(stateName(state));
  Serial.print(" -> ");
  Serial.print(stateName(newState));
  Serial.print("\n");

  musicPlayer.stopPlaying();
  state = newState;
  stateStartTime = millis();
}

void hangryState() {
  leftEye.write(50);
  rightEye.write(50);
  eyecolor(10, 0, 0);

  if (isFed()) {
    changeState(chewing);
  }

}

void chewingState() {
  leftEye.write(100);
  rightEye.write(100);
  eyecolor(10, 2, 0);

  if (musicPlayer.stopped()) {
    musicPlayer.startPlayingFile("/chewing.mp3");
  }
  if (getTimePassedMs() > 5000) {
    changeState(happy);
  }
}

void happyState() {
  leftEye.write(150);
  rightEye.write(150);
  eyecolor(0, 10, 0);

  if (musicPlayer.stopped()) {
    musicPlayer.startPlayingFile("/happy.mp3");
  }
  if (getTimePassedMs() > 10000) {
    changeState(hangry);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (state == hangry) {
    hangryState();
  }  else if (state == chewing) {
    chewingState();
  } else {
    happyState();
  }
}

bool isFed() {
  int value = analogRead(BEAM_SENSOR);
  // Serial.print("sensor: ");
  // Serial.print(value);
  // Serial.print("\n");
  return value < BEAM_THRESHOLD;
}

unsigned long getTimePassedMs() {
  unsigned long currentTime = millis();
  return currentTime - stateStartTime;
}
