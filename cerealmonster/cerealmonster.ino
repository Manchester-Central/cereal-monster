#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#include <Adafruit_VS1053.h>
#include <SD.h>
#include <Servo.h>
#include <SPI.h>

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

// #define BEAM_THRESHOLD 600
int BEAM_THRESHOLD = 600;
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

  int average_value = lightCalibration();
  BEAM_THRESHOLD = average_value; 
  Serial.print(BEAM_THRESHOLD);

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

  switch (newState)
  {
    case hangry:
      hangryState_entry();
      break;
    case chewing:
      chewingState_entry();
      break;
    case happy:
      happyState_entry();
      break;
    default:
      break;
  }
}

void hangryState_entry() {
  leftEye.write(90);// - 20);
  rightEye.write(90);//  + 20);
  eyecolor(20, 0, 0);
}

void hangryState() {
  if (isFed()) {
    changeState(chewing);
  }
}

void chewingState_entry()
{
  leftEye.write(90);
  rightEye.write(90);
  eyecolor(15, 5, 0);
  musicPlayer.startPlayingFile("/chewing.mp3");
}

void chewingState() {
  if (getTimePassedMs() > 5000) {
    changeState(happy);
  }
}

void happyState_entry()
{
  leftEye.write(90);// + 30);
  rightEye.write(90);// - 30);
  eyecolor(0, 20, 0);
  musicPlayer.startPlayingFile("/happy.mp3");
}

void happyState() {
  if (getTimePassedMs() > 4000) {
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

int lightCalibration() {
  int value_1 = analogRead(BEAM_SENSOR);
  delay(500);
  int value_2 = analogRead(BEAM_SENSOR);
  delay(500);
  int value_3 = analogRead(BEAM_SENSOR);
  delay(500);
  int value_4 = analogRead(BEAM_SENSOR);
  delay(500);
  int value_5 = analogRead(BEAM_SENSOR);
  delay(500);
  int average_value = ((value_1 + value_2 + value_3 + value_4 + value_5)/5) * 0.9;
  return average_value;
}

bool isFed() {
  int value = analogRead(BEAM_SENSOR);
  //Serial.print("sensor: ");
  //Serial.print(value);
  //Serial.print("\n");
  return value < BEAM_THRESHOLD;
}

unsigned long getTimePassedMs() {
  unsigned long currentTime = millis();
  return currentTime - stateStartTime;
}
