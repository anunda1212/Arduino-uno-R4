#include <ArduinoBLE.h>

const int FLEX_PIN = A0;
BLEService flexService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEIntCharacteristic flexCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

void setup() {
  Serial.begin(9600);
  pinMode(FLEX_PIN, INPUT);
  
  if (!BLE.begin()) {
    Serial.println("BLE initialization failed!");
    while (1);
  }

  BLE.setLocalName("FlexSensorBoard");
  BLE.setAdvertisedService(flexService);
  flexService.addCharacteristic(flexCharacteristic);
  BLE.addService(flexService);
  
  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  BLEDevice central = BLE.central();
  
  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());
    
    while (central.connected()) {
      int flexValue = analogRead(FLEX_PIN);
      flexValue = map(flexValue, 560, 750, 0, 180);
      Serial.println(flexValue);
      flexCharacteristic.writeValue(flexValue);
      delay(1000);
    }
    
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}