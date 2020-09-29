void setup() {
  // put your setup code here, to run once:
pinMode (A0, INPUT);
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
 int value = analogRead(A0);
 Serial.print("Value: ");
 Serial.print(value);
 Serial.print("\n");
}
