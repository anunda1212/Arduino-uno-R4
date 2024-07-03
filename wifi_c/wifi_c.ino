#include <WiFiS3.h>
#include <Servo.h>

char ssid[] = "OPPO";
char pass[] = "Anunda2545";
const char *host = "192.168.224.138";
int status = WL_IDLE_STATUS;

WiFiClient client;
Servo S1;
Servo S2;
Servo S3;
Servo S4;

int servoPin = 3;
int servoPin1 = 5;
int servoPin2 = 6;
int servoPin3 = 9;
int flexValue;

uint8_t msg[5] ;

void setup() {
  Serial.begin(9600);
  Serial.println("16 channel Servo test!");
  S1.attach(servoPin);
  S2.attach(servoPin1);
  S3.attach(servoPin2);
  S4.attach(servoPin3);

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  Serial.println("connected");
  
  IPAddress serverIP;
  serverIP.fromString(host);

  client.connect(serverIP, 80);
}

void loop() {
  if (client.connected()) {
    while (client.available()) {
        msg[0] = client.read();
        msg[1] = client.read();
        msg[2] = client.read();
        msg[3] = client.read();

        Serial.println("Received Flex Sensor Value: " + String(msg[0]));
        Serial.println("Received Flex Sensor Value1: " + String(msg[1]));
        Serial.println("Received Flex Sensor Value2: " + String(msg[2]));
        Serial.println("Received Flex Sensor Value3: " + String(msg[3]));
        delay(500);
        S1.write(msg[0]);
        S2.write(msg[1]);
        S3.write(msg[2]);
        S4.write(msg[3]);
    }
  }
  else {
    Serial.println("Lost connection to server");
  }
}