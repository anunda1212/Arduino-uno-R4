#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN  100
#define SERVOMAX  1000

int flex = A0;
int flex_val;
uint8_t servonum = 0;
void setup() {
  Serial.begin(9600);
  Serial.println("16 channel Servo test!");
  pwm.setOscillatorFrequency(27000000);
  pwm.begin();
  pwm.setPWMFreq(60);
  
}

void loop() {
  // flex_val = analogRead(flex);
  // int pwmValue = map(flex_val,0, 180, 120,1000  );
  // Serial.println(pwmValue);
  // pwm.setPWM(0,0,pwmValue);

  for (int i= 0; i < servonum; i++){
    for (int microsec = 500; microsec < 1450; microsec++) {
      pwm.writeMicroseconds(i, microsec);
    }
    delay(500);
    for (int microsec = 3000; microsec > 1450; microsec--) {
      pwm.writeMicroseconds(i, microsec);
    }
  }
  delay(1000);
  // for( int angle =0; angle<181; angle +=10){
  //   delay(500);
  //   //pwm.writeMicroseconds(15,angleToPulse(angle));
  //   pwm.setPWM(0, 0,angleToPulse(angle));
  //   pwm.setPWM(1, 0,angleToPulse(angle));
  //   pwm.setPWM(2, 0,angleToPulse(angle));
  //   pwm.setPWM(3, 0,angleToPulse(angle));
  //   pwm.setPWM(4, 0,angleToPulse(angle));
  // }
  // delay(500);
  // pwm.setPWM(4, 0,1000);
  // delay(1000);
  // pwm.setPWM(4, 0,125);
  // delay(1000);


}
int angleToPulse(int ang){
   int pulse = map(ang,0, 180, SERVOMIN,SERVOMAX);// map angle of 0 to 180 to Servo min and Servo max 
   Serial.print("Angle: ");Serial.print(ang);
   Serial.print(" pulse: ");Serial.println(pulse);
   return pulse;
}