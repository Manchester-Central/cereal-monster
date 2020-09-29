#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define PIXEL_PIN     9      // Which pin on the Arduino is connected to the NeoPixels?
#define NUMPIXELS    48      // Total number of pixels

int value;
int red;
int green;
int blue;

Adafruit_NeoPixel pixels(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
   pixels.begin();


}

void eyecolor(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUMPIXELS; i++) { // For each pixel...
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
}

void loop() {
  // put your main code here, to run repeatedly:

  String S = Serial.readStringUntil("\n");

  if (S.length() > 0) {
    int found = sscanf(S.c_str(), "%d %d %d", &red, &green, &blue);
    Serial.print(S.c_str());
    Serial.print("\n");
    if (found == 3) {
      Serial.print("Values: ");
      Serial.print(red);
      Serial.print(" ");
      Serial.print(green);
      Serial.print(" ");
      Serial.print(blue);
      Serial.print("\n");
      eyecolor(red, green, blue);

    }
  }
}
