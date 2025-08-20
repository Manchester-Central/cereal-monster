#include "monster.h"

Monster::Monster() {
  m_musicPlayer = new Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);
  m_pixels = new Adafruit_NeoPixel(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
  m_leftEye = new Servo();
  m_rightEye = new Servo();
  m_beamThreshold = 600;
}

void Monster::setup() {
  m_leftEye->attach(LEFT_EYE, 500, 2500);
  m_rightEye->attach(RIGHT_EYE, 500, 2500);

  m_pixels->begin();
  Serial.begin(9600);

  pinMode(BEAM_SENSOR, INPUT);

  int average_value = lightCalibration();
  m_beamThreshold = average_value;
  Serial.print(m_beamThreshold);

  if (!m_musicPlayer->begin()) {  // initialise the music player
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    while (1)
      ;
  }
  Serial.println(F("VS1053 found"));

  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1)
      ;  // don't do anything more
  }

  m_musicPlayer->setVolume(VOLUME, VOLUME);
  m_musicPlayer->useInterrupt(VS1053_FILEPLAYER_PIN_INT);
}

void Monster::setEyeColor(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUMPIXELS; i++) {  // For each pixel...
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    m_pixels->setPixelColor(i, m_pixels->Color(r, g, b));
  }
  m_pixels->show();
}

void Monster::setEyeColor(struct Color newColor) {
  setEyeColor(newColor.r, newColor.g, newColor.b);
}

void Monster::setEyebrowPosition(int leftAngle, int rightAngle) {
  m_leftEye->write(leftAngle);
  m_rightEye->write(rightAngle);
}

void Monster::setEyebrowPosition(struct EyebrowPosition newPosition) {
  setEyebrowPosition(newPosition.leftAngle, newPosition.rightAngle);
}

void Monster::playSound(char* file){
  m_musicPlayer->startPlayingFile(file);
}

void Monster::stopSounds() {
  m_musicPlayer->stopPlaying();
}

int Monster::lightCalibration() {
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
  int average_value = ((value_1 + value_2 + value_3 + value_4 + value_5) / 5) * 0.9;
  return average_value;
}

bool Monster::isFed() {
  int value = analogRead(BEAM_SENSOR);
  //Serial.print("sensor: ");
  //Serial.print(value);
  //Serial.print("\n");
  return value < m_beamThreshold;
}