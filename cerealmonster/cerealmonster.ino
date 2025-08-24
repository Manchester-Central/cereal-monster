#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif
#include <Adafruit_VS1053.h>
#include <SD.h>
#include <Servo.h>
#include <SPI.h>

// AdaFruit MusicMaker mp3 player sheild
const int32_t SHIELD_RESET = -1;  // VS1053 reset pin
const uint32_t SHIELD_CS = 7;     // VS1053 chip select pin (output)
const uint32_t SHIELD_DCS = 6;
const uint32_t CARDCS = 4;  // Card chip select pin
const uint32_t DREQ = 3;    // VS1053 Data request, ideally an Interrupt pin // DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
// 2 x 24 LED AdaFruit NeoPixel rings (P1586)
const uint32_t PIXEL_PIN = 9;   // Which pin on the Arduino is connected to the NeoPixels?
const uint32_t NUMPIXELS = 48;  // Total number of pixels
//servos
const uint32_t LEFT_EYE = 10;
const uint32_t RIGHT_EYE = 5;
// photo-resistor
const uint32_t BEAM_SENSOR = 0xA0;
const float BEAM_CALIBRATION_COEFFICIENT = 0.9;
const uint8_t VOLUME = 20; // 0 is loudest, 255 is off
const uint64_t GETTING_HANGRY_DURATION_MS = 3000;
int beam_threshold = 600; // default value, will be overridden

struct Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t _padding; // ignore the 4th value, it exists solely for word-alignment

  Color(uint8_t i, uint8_t j, uint8_t k) {
    r=i;
    g=j;
    b=k;
    // This constructor means you don't have to include the padding in future calls
    _padding = 0;
  }
};

const Color HANGRY_COLOR{20, 0, 0};
const Color CHEWING_COLOR{15, 5, 0};
const Color HAPPY_COLOR{0, 20, 0};

struct EyePosition {
  uint32_t left_angle;
  uint32_t right_angle;

  EyePosition(uint32_t left_eye_difference) {
    left_angle = 90+left_eye_difference;
    right_angle = 90-left_eye_difference;
  }

  // To support potential weird crazy eye behavior, you can set both at init
  EyePosition(uint32_t left_eye_difference, uint32_t right_eye_difference) {
    left_angle = 90+left_eye_difference;
    // I think +right is better here, as students might try to put the opposite
    // direction in for the other eye if they wanted to set both explicitly
    right_angle = 90+right_eye_difference;
  }
};

const EyePosition HANGRY_EYES{-20};
const EyePosition CHEWING_EYES{0};
const EyePosition HAPPY_EYES{30};

enum class State_T {
  hangry,
  chewing,
  happy,
  getting_hungry
};
uint32_t HAPPY_STATE_DURATION_MS = 4000;
uint32_t CHEWING_STATE_DURATION_MS = 5000;
uint64_t state_loop_count = 0;

// eyebrow motors
Servo leftEye;
Servo rightEye;
// speakers
Adafruit_VS1053_FilePlayer musicPlayer{SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS};
// LED eyes
Adafruit_NeoPixel pixels{NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800};

// state machine
State_T currentState = State_T::happy;
unsigned long stateStartTime;

// Gets the time since the state was entered via changeState
uint64_t getTimePassedMs() {
  uint64_t currentTime = millis();
  return currentTime - stateStartTime;
}

template<typname T>
T linearInterpolate(T start, T end, double alpha) {
  return static_cast<T>((end - start) * alpha + start);
}

EyePosition blendEyePosition(EyePosition& start, EyePosition& end, double alpha) {
  return EyePosition{
    linearInterpolate(start.left_angle, end.left_angle, alpha),
    linearInterpolate(start.right_angle, end.right_angle, alpha)
  };
}

Color blendColor(Color& start, Color& end, double alpha) {
  return Color{
    linearInterpolate(start.r, end.r, alpha),
    linearInterpolate(start.g, end.g, alpha),
    linearInterpolate(start.b, end.b, alpha)
  };
}

void setEyeColor(Color& color) {
  for (int i = 0; i < NUMPIXELS; i++) {  // For each pixel...
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(color.r, color.g, color.b));
  }
  pixels.show();
}

void setEyePosition(EyePosition& position) {
  leftEye.write(position.left_angle);
  rightEye.write(position.right_angle);
}

const char* stateName(State_T s) {
  switch (s) {
    case State_T::hangry:
      return "hangry";
    case State_T::chewing:
      return "chewing";
    case State_T::happy:
      return "happy";
    case State_T::getting_hungry:
      return "getting hungry";
  }
}

void changeState(State_T newState) {
  musicPlayer.stopPlaying();

  Serial.print("State transition: ");
  Serial.print(stateName(currentState));
  Serial.print(" -> ");
  Serial.print(stateName(newState));
  Serial.print("\n");

  currentState = newState;
  stateStartTime = millis();

  switch (newState) {
    case State_T::hangry:
      hangry_entry();
      break;
    case State_T::chewing:
      chewing_entry();
      break;
    case State_T::happy:
      happy_entry();
      break;
    case State_T::getting_hungry:
      getting_hungry_entry();
      break;
  }
  state_loop_count = 0;
}

void hangry_entry() {
  setEyePosition(HANGRY_EYES);
  setEyeColor(HANGRY_COLOR);
}

void hangryState() {
  if (isFed()) {
    changeState(State_T::chewing);
  }
}

void chewing_entry() {
  setEyePosition(CHEWING_EYES);
  setEyeColor(CHEWING_COLOR);
  musicPlayer.startPlayingFile("/chewing.mp3");
}

void chewingState() {
  if (getTimePassedMs() > CHEWING_STATE_DURATION_MS) {
    changeState(State_T::happy);
  }
}

void happy_entry() {
  setEyePosition(HAPPY_EYES);
  setEyeColor(HAPPY_COLOR);
  musicPlayer.startPlayingFile("/happy.mp3");
}

void happyState() {
  if (getTimePassedMs() > HAPPY_STATE_DURATION_MS) {
    changeState(State_T::getting_hungry);
  }
}

void getting_hungry_entry() {
  // No-op, every state loop writes to LEDs and motors
}

void gettinghungryState() {
  uint64_t time_in_state = getTimePassedMs();
  if (time_in_state > GETTING_HANGRY_DURATION_MS) {
    changeState(State_T::hangry);
    return;
  }

  // alpha is a term in many fields, but commonly means "0 to 100%"
  // or some other value in an equation.
  double alpha = static_cast<double>(time_in_state) / GETTING_HANGRY_DURATION_MS;

  // We're running each loop (staggered) every 10 arduino loops
  // Possibly improvement would be to do it every 1 degree that the angle changes,
  // or maybe a student can think of something more clever!
  if (state_loop_count % 10 == 0) {
    setEyePosition(blendEyePosition(HAPPY_EYES, HANGRY_EYES, alpha));
  }

  if (state_loop_count+5 % 10 == 0 ) {
    setEyeColor(blendColor(HAPPY_COLOR, HANGRY_COLOR, alpha));
  }
}

uint32_t lightCalibration() {
  uint32_t sum = 0;
  sum += analogRead(BEAM_SENSOR);
  delay(500);
  sum += analogRead(BEAM_SENSOR);
  delay(500);
  sum += analogRead(BEAM_SENSOR);
  delay(500);
  sum += analogRead(BEAM_SENSOR);
  delay(500);
  sum += analogRead(BEAM_SENSOR);
  delay(500);
  int average_value = sum / 5.0;
  return static_cast<uint32_t>(average_value * BEAM_CALIBRATION_COEFFICIENT);
}

bool isFed() {
  int value = analogRead(BEAM_SENSOR);
  //Serial.print("sensor: ");
  //Serial.print(value);
  //Serial.print("\n");
  return value < beam_threshold;
}

void setup() {
  //             PIN_ID,   MIN, MAX pulse width in microseconds
  leftEye.attach(LEFT_EYE, 500, 2500);
  rightEye.attach(RIGHT_EYE, 500, 2500);

  pixels.begin();
  Serial.begin(9600); // 9600 is something called a baud rate, ask a mentor what that means!

  pinMode(BEAM_SENSOR, INPUT);

  beam_threshold = lightCalibration();
  Serial.print(beam_threshold);

  if (!musicPlayer.begin()) {  // initialise the music player
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    while(true); // don't do anything more
  }
  Serial.println(F("VS1053 found"));

  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while(true); // don't do anything more
  }

  musicPlayer.setVolume(VOLUME, VOLUME);
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);
  changeState(State_T::hangry);
}

void loop() {
  // put your main code here, to run repeatedly:
  switch (currentState) {
    case State_T::hangry:
      hangryState();
      break;
    case State_T::chewing:
      chewingState();
      break;
    case State_T::happy:
      happyState();
      break;
    case State_T::getting_hungry:
      gettinghungryState();
      break;
  }
  state_loop_count++;
}
