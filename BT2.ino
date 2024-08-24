#include <ArduinoBLE.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

uint8_t buffer[sizeof(int)];

const int SERVO_PIN = 0;

void setup() {
  Serial.begin(9600);
  
  if (!BLE.begin()) {
    Serial.println("BLE initialization failed!");
    while (1);
  }

  BLE.scanForUuid("19B10000-E8F2-537E-4F6C-D104768A1214");

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(60);
}

void loop() {
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    if (peripheral.localName() == "FlexSensorBoard") {
      BLE.stopScan();
      controlServo(peripheral);
      BLE.scanForUuid("19B10000-E8F2-537E-4F6C-D104768A1214");
    }
  }
}

void controlServo(BLEDevice peripheral) {
  Serial.println("Connecting ...");

  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }

  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

  BLECharacteristic flexCharacteristic = peripheral.characteristic("19B10001-E8F2-537E-4F6C-D104768A1214");

  if (!flexCharacteristic) {
    Serial.println("Flex characteristic not found!");
    peripheral.disconnect();
    return;
  }

  Serial.println("Reading flex sensor values:");

  while (peripheral.connected()) {
    // if (flexCharacteristic.valueUpdated()) {
    //   int flexValue;
    //   flexCharacteristic.readValue(&flexValue, sizeof(flexValue));
    //   Serial.println(flexValue);
    //   pwm.setPWM(SERVO_PIN, 0, flexValue);
    //   delay(1000);
    // }
    int flexValue;
    while(flexCharacteristic.readValue(&flexValue, sizeof(flexValue))) {
        Serial.print("Flex value: ");
        Serial.println(flexValue);
        pwm.setPWM(SERVO_PIN, 0, flexValue);
        delay(1000);
    }
    
  }

  Serial.println("Peripheral disconnected");
}