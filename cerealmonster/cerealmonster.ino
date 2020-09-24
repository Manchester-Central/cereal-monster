#include "Adafruit_NeoPixel.h"
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// AdaFruit MusicMaker mp3 player shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6
#define CARDCS 4     // Card chip select pin
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin // DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt

// 2 x 24 LED AdaFruit NeoPixel rings (P1586)
#define PIN        9 // Which pin on the Arduino is connected to the NeoPixels?
#define NUMPIXELS 48 // Total number of pixels

enum State_T {
  hangry,
  chewing,
  happy
};

Adafruit_VS1053_FilePlayer musicPlayer =
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

State_T state;
int laserSensor;
unsigned long stateStartTime;

void setup() {
  pixels.begin();
  Serial.begin(9600);

  pinMode(A0, INPUT);

  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));

  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }

  musicPlayer.setVolume(20,20);

  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);
  changeState(hangry);
}

void eyecolor(uint8_t r, uint8_t g, uint8_t b){
  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
}

void changeState(State_T newState) {
  musicPlayer.stopPlaying();
  state = newState;
  stateStartTime = millis();
}

void hangryState() {
  Serial.print(" Hangry ");
  Serial.print("\n");
	eyecolor(10,0,0);

  if (isFed())
  {
  	changeState(chewing);
  }

}

void chewingState() {
  Serial.print(" Chewing ");
  Serial.print("\n");
  eyecolor(10, 2, 0);

  if (musicPlayer.stopped()) {
    musicPlayer.startPlayingFile("/chewing.mp3");
  }
  if (getTimePassedMs() > 5000){
    changeState(happy);
  }
}

void happyState() {
  Serial.print(" Happy ");
  Serial.print("\n");
  eyecolor(0, 10, 0);

  if (musicPlayer.stopped()) {
    musicPlayer.startPlayingFile("/happy.mp3");
  }
  if (getTimePassedMs() > 10000){
    changeState(hangry);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (state == hangry)
  {
    hangryState();
  }
  else if (state == chewing)
  {
    chewingState();
  }
  else
  {
    happyState();
  }
}

bool isFed() {
  int value = analogRead(A0);
  // Serial.print("sensor: ");
  // Serial.print(value);
  // Serial.print("\n");
  return value < 120;
}

unsigned long getTimePassedMs(){
  unsigned long currentTime = millis();
  return currentTime - stateStartTime;
}
