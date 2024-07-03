#include <Servo.h>

Servo myservo;
int flexPin = A1;
int a;
int flexPosition;
void setup()
{
  Serial.begin(9600);
  myservo.attach(9);
}
void loop()
{
  flexPosition = analogRead(flexPin);
  Serial.println(flexPosition);
  a = map(flexPosition, 0, 1028, 0, 180);
  myservo.write(a);
}