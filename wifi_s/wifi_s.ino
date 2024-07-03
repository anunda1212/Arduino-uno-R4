#include "WiFiS3.h"

char ssid[] = "OPPO";          
char pass[] = "Anunda2545";  
int status = WL_IDLE_STATUS;

WiFiServer server(80);
int flexPin = A0; 
int flexPin1 = A1;
int flexPin2 = A2;
int flexPin3 = A3;
int flexValue;
int flexValue1;
int flexValue2;
int flexValue3;

uint8_t msg[5];

void setup() {
  Serial.begin(9600);

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }

  server.begin();
  printWifiStatus();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String HTTP_req = "";
    while (client.connected()) {
      if (client.available()) {
       
          flexValue = analogRead(flexPin);
          flexValue = map(flexValue, 0, 1023, 0, 180);
          flexValue1 = analogRead(flexPin1);
          flexValue1 = map(flexValue1, 0, 1023, 0, 180);
          flexValue2 = analogRead(flexPin2);
          flexValue2 = map(flexValue2, 0, 1023, 0, 180);
          flexValue3 = analogRead(flexPin3);
          flexValue3 = map(flexValue3, 0, 1023, 0, 180);
          Serial.println("Flex Sensor Value: " + String(flexValue));
          Serial.println("Flex Sensor Value1: " + String(flexValue1));
          Serial.println("Flex Sensor Value2: " + String(flexValue2));
          Serial.println("Flex Sensor Value3: " + String(flexValue3));
          
          msg[0]=flexValue;
          msg[1]=flexValue1;
          msg[2]=flexValue2;
          msg[3]=flexValue3;
          client.write(msg[0]);
          client.write(msg[1]);
          client.write(msg[2]);
          client.write(msg[3]);
          delay(500);
      }
    }

    client.stop();
    Serial.println("Client disconnected");
  }
}

void printWifiStatus() {
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}   