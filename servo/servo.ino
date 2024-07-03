#include<Servo.h>

Servo S1;
Servo S2;
Servo S3;
Servo S4;


int ButtonPin1 = 0;
int ButtonPin2 = 1;
int ButtonPin3 = 2;
int ButtonPin4 = 4;
int ButtonPin5 = 7;
int ButtonPin6 = 8;
int ButtonPin7 = 3;
int ButtonPin8 = 13;
int ButtonPin9 = 18;
int ButtonPin10 = 19;

int ServoPin1 = 5;
int ServoPin2 = 6;
int ServoPin3 = 9;
int ServoPin4 = 10;

int Button1_s = 0;
int Button1_s1 = 0;
int Button2_s = 0;
int Button2_s1 = 0;
int Button3_s = 0;
int Button3_s1 = 0;
int Button4_s = 0;
int Button4_s1 = 0;


void setup() {
  Serial.begin(9600);
  S1.attach(ServoPin1);
  S2.attach(ServoPin2);
  S3.attach(ServoPin3);
  S4.attach(ServoPin4);

  
  pinMode(ButtonPin1, INPUT_PULLUP);
  pinMode(ButtonPin2, INPUT_PULLUP);
  pinMode(ButtonPin3, INPUT_PULLUP);
  pinMode(ButtonPin4, INPUT_PULLUP);
  pinMode(ButtonPin5, INPUT_PULLUP);
  pinMode(ButtonPin6, INPUT_PULLUP);
  pinMode(ButtonPin7, INPUT_PULLUP);
  pinMode(ButtonPin8, INPUT_PULLUP);


  S1.writeMicroseconds(0);
  S2.writeMicroseconds(0);
  S3.writeMicroseconds(0);
  S4.writeMicroseconds(0);

}

void loop() {
  Button1_s = digitalRead(ButtonPin1);
  Button1_s1 = digitalRead(ButtonPin2);
  Button2_s = digitalRead(ButtonPin3);
  Button2_s1 = digitalRead(ButtonPin4);
  Button3_s = digitalRead(ButtonPin5);
  Button3_s1 = digitalRead(ButtonPin6);
  Button4_s = digitalRead(ButtonPin7);
  Button4_s1 = digitalRead(ButtonPin8);

  // S1.write(180);S2.write(180);S3.write(180);S4.write(180);
  // delay(2000);
  // S1.write(0);S2.write(0);S3.write(0);S4.write(0);
  // delay(2000);
  
  if(Button1_s == 0){
     S1.writeMicroseconds(0);
     delay(500);
  }
  if(Button1_s1 == 0){
      S1.writeMicroseconds(3000);
      delay(500);
  }

  if(Button2_s == 0){
     S2.writeMicroseconds(0);
     delay(500);
  }
  if(Button2_s1 == 0){
      S2.writeMicroseconds(3000);
      delay(500);
  }

  if(Button3_s == 0){
     S3.writeMicroseconds(0);
     delay(500);
  }
  if(Button3_s1 == 0){
      S3.writeMicroseconds(3000);
      delay(500);
  }

  if(Button4_s == 0){
     S4.writeMicroseconds(0);
     delay(500);
  }
  if(Button4_s1 == 0){
      S4.writeMicroseconds(3000);
      delay(500);
  }

}

