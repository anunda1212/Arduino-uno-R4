#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <WiFiS3.h>
#include <ArduinoBLE.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVOMIN  150  // Min pulse length
#define SERVOMAX  600  // Max pulse length

// Wi-Fi settings
WiFiServer server(80);
WiFiClient client;
const char* ssid = "ArduinoFlex";  
const char* password = "12345678";

// BLE settings
BLEService flexService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLECharacteristic flexCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, 20);

// Servo configuration
const int NUM_SENSORS = 5;
const int servoPins[NUM_SENSORS] = {4, 8, 10, 11, 12}; 

// Mode setup
bool useWiFi = true;  
bool lastButtonState = HIGH;
int modeButton = 8;

// LED indicator pins
const int RED_PIN = 2;
const int GREEN_PIN = 3;
const int BLUE_PIN = 4;

// LED control variables
unsigned long lastBlinkTime = 0;
const unsigned long blinkInterval = 500; 
bool blinkState = false;
bool connectionEstablished = false;

// Timeout for data reception and connection
unsigned long lastDataReceived = 0;
const int dataTimeout = 1000;
const int connectionTimeout = 5000;

void setup() {
  Serial.begin(9600);
  pinMode(modeButton, INPUT_PULLUP);
  
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  
  setRGBColor(255, 0, 0);
  
  pwm.begin();
  pwm.setPWMFreq(50);  

  if (useWiFi) {
    startWiFi();
  } else {
    startBLE();
  }
}

void loop() {
  checkButtonPress();  

  String data = "";
  if (useWiFi) {
    data = receiveDataWiFi();
  } else { 
    BLE.poll();
    data = receiveDataBLE();
  }

  if (data.length() > 0) {
    processFlexData(data);
    lastDataReceived = millis();
    connectionEstablished = true;
    setRGBColor(0, 255, 0);
  }

  if (!connectionEstablished) {
    if (millis() - lastBlinkTime >= blinkInterval) {
      lastBlinkTime = millis();
      blinkState = !blinkState;
      
      if (blinkState) {
        setRGBColor(0, 0, 255);
      } else {
        setRGBColor(0, 0, 0);
      }
    }
  } else if (millis() - lastDataReceived > dataTimeout) {
    if (millis() - lastBlinkTime >= blinkInterval) {
      lastBlinkTime = millis();
      blinkState = !blinkState;
      
      if (blinkState) {
        setRGBColor(255, 0, 0);
      } else {
        setRGBColor(0, 0, 0);
      }
    }
  }

  delay(20);  
}

void setRGBColor(int red, int green, int blue) {
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
}

void processFlexData(String data) {
  int flexValues[NUM_SENSORS] = {0};  
  
  int index = 0;
  int lastCommaIndex = -1;
  
  for (int i = 0; i < NUM_SENSORS; i++) {
    int commaIndex = data.indexOf(',', lastCommaIndex + 1);
    if (commaIndex == -1 && i < NUM_SENSORS - 1) {
      Serial.println("Warning: Incomplete data received");
      return;
    }
    
    String valueStr;
    if (i == NUM_SENSORS - 1 || commaIndex == -1) {
      valueStr = data.substring(lastCommaIndex + 1);
    } else {
      valueStr = data.substring(lastCommaIndex + 1, commaIndex);
    }
    
    flexValues[i] = valueStr.toInt();
    lastCommaIndex = commaIndex;
  }
  
  for (int i = 0; i < NUM_SENSORS; i++) {
    int pulseWidth = map(flexValues[i], 0, 180, SERVOMIN, SERVOMAX);
    pwm.setPWM(servoPins[i], 0, pulseWidth);
  }
}

// ----------------------------- Wi-Fi FUNCTIONS -----------------------------
void startWiFi() {
  connectionEstablished = false;
  
  Serial.println("Connecting to Wi-Fi...");
  WiFi.disconnect();
  WiFi.end();
  WiFi.begin(ssid, password);

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < connectionTimeout) {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi connected successfully!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP()); 
    
    setRGBColor(0, 255, 0);
    connectionEstablished = true;
    
    server.begin();
  } else {
    Serial.println("\nWi-Fi connection failed!");
    setRGBColor(255, 0, 0);
  }
}

String receiveDataWiFi() {
  client = server.available();
  if (client) {
    String data = "";
    unsigned long timeout = millis() + 1000;
    
    while (client.connected() && millis() < timeout) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          Serial.println("Received Wi-Fi data: " + data);
          return data;
        }
        data += c;
      }
    }
    
    if (data.length() > 0) {
      Serial.println("Received Wi-Fi data: " + data);
      return data;
    }
  }
  return "";
}

// ----------------------------- BLE FUNCTIONS -----------------------------
void startBLE() {
  Serial.println("Starting BLE...");
  
  if (!BLE.begin()) {
    Serial.println("BLE initialization failed!");
    setRGBColor(255, 0, 0);
    return;
  }
  
  BLE.scanForUuid("19B10000-E8F2-537E-4F6C-D104768A1214");
  Serial.println("BLE ready to receive data!");
  setRGBColor(0, 255, 0);
  
}

String receiveDataBLE() {
  BLEDevice peripheral = BLE.available();

  if (peripheral && peripheral.localName() == "FlexTransmitter") {
    Serial.println("Found FlexTransmitter device");
    BLE.stopScan();

    if (connectToPeripheral(peripheral)) {
      BLECharacteristic characteristic = peripheral.characteristic("19B10001-E8F2-537E-4F6C-D104768A1214");

      if (characteristic) {
        characteristic.subscribe();
        Serial.println("Subscribed to characteristic");

        while (peripheral.connected()) {
          if (characteristic.valueUpdated()) {
            byte value[20];
            int valueLength = characteristic.readValue(value, sizeof(value));
            String data = "";

            for (int i = 0; i < valueLength; i++) {
              data += (char)value[i];
            }

            if (data.length() > 0) {
              Serial.println("Received BLE data: " + data);
              processFlexData(data);
            }
          }

          delay(10);
        }

        Serial.println("BLE disconnected");
        characteristic.unsubscribe();
        peripheral.disconnect();
      }
    }

    BLE.scanForUuid("19B10000-E8F2-537E-4F6C-D104768A1214");
  }

  return "";
}


bool connectToPeripheral(BLEDevice peripheral) {
  Serial.print("Connecting to ");
  Serial.println(peripheral.address());
  
  if (peripheral.connect()) {
    Serial.println("BLE connected successfully!");
    
    if (peripheral.discoverAttributes()) {
      Serial.println("Attributes discovered");
      return true;
    } else {
      Serial.println("Attribute discovery failed!");
      peripheral.disconnect();
    }
  } else {
    Serial.println("BLE connection failed!");
  }
  
  return false;
}

void checkButtonPress() {
  bool currentButtonState = digitalRead(modeButton);
  
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    delay(50); // Debounce
    useWiFi = !useWiFi;

    if (useWiFi) {
      Serial.println("Switching to Wi-Fi mode...");
      BLE.stopScan();
      BLE.end();
      delay(1000);
      startWiFi();
    } else {
      Serial.println("Switching to BLE mode...");
      server.end();
      WiFi.disconnect();
      WiFi.end();
      delay(1000);
      startBLE();
    }
  }
  
  lastButtonState = currentButtonState;
}

