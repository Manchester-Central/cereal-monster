#include <Servo.h>
#define LEFT_EYE     10
#define RIGHT_EYE     5

Servo leftEye;
Servo rightEye;
int value;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  leftEye.attach(LEFT_EYE, 500, 2500);
  rightEye.attach(RIGHT_EYE, 500, 2500);

}

void loop() {
  // put your main code here, to run repeatedly:

  String S = Serial.readStringUntil("\n");

  if (S.length() > 0) {
    int found = sscanf(S.c_str(), "%d", &value);
    Serial.print(S.c_str());
    Serial.print("\n");
    if (found > 0) {
      Serial.print("Value: ");
      Serial.print(value);
      Serial.print("\n");
      leftEye.write(value);
      rightEye.write(value);
    }
  }


}
