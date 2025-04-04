#include <WiFiS3.h>
#include <ArduinoBLE.h>

WiFiServer server(80);
WiFiClient client;
BLEService flexService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLECharacteristic flexCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify, 20);

int flexPin1 = A0;
int flexPin2 = A1;
int flexPin3 = A2;
int flexPin4 = A3;
int flexPin5 = A4;
int modeButton = 4; 
bool useWifi = true; // Start with Wi-Fi mode
bool lastButtonState = HIGH;
String html;
String lastFlexData = "0,0,0,0,0";

const char* serverIP = "192.168.4.2";    
const int serverPort = 80;

unsigned long lastSendTime = 0;
const int sendInterval = 50; // ลดเวลาอัพเดทให้เร็วขึ้น

void scanWiFi() {
  html = "<!DOCTYPE html>"
         "<html><head>"
         "<title>Flex Sensor Monitor</title>"
         "<meta charset='UTF-8'>"
         "<meta name='viewport' content='width=device-width, initial-scale=1'>"
         "<style>"
         "body { font-family: Arial, sans-serif; max-width: 600px; margin: 0 auto; padding: 20px; background-color: #f4f4f4; }"
         "h2 { color: #333; text-align: center; }"
         ".sensor-group { display: flex; justify-content: space-between; margin-bottom: 20px; }"
         ".sensor { flex: 1; margin: 0 10px; padding: 15px; background-color: #ffffff; border-radius: 8px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); text-align: center; }"
         ".sensor-label { color: #666; font-size: 14px; margin-bottom: 10px; }"
         ".sensor-value { font-size: 24px; font-weight: bold; color: #007bff; }"
         ".wifi-form { background-color: #ffffff; padding: 20px; border-radius: 8px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); margin-top: 20px; }"
         ".status { text-align: center; margin-top: 20px; font-size: 12px; color: #666; }"
         "</style>"
         "<script>"
         "let isUpdating = false;"
         "let websocket;"
         
         "function setupWebSocket() {"
         "  const wsProtocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';"
         "  const wsUrl = wsProtocol + '//' + window.location.hostname + '/ws';"
         "  try {"
         "    websocket = new WebSocket(wsUrl);"
         "    console.log('WebSocket not supported, falling back to polling');"
         "    startPolling();"
         "  } catch(e) {"
         "    console.log('WebSocket error, falling back to polling');"
         "    startPolling();"
         "  }"
         "}"
         
         "function startPolling() {"
         "  console.log('Starting polling');"
         "  updateSensors();"
         "  setInterval(updateSensors, 200);" // ลดเวลาโพลเพื่อให้ค่าอัพเดทเร็วขึ้น
         "}"
         
         "function updateSensors() {"
         "  if (isUpdating) return;"
         "  isUpdating = true;"
         "  fetch('/sensors?' + new Date().getTime())" // เพิ่ม timestamp เพื่อป้องกัน cache
         "    .then(response => response.text())"
         "    .then(data => {"
         "      const values = data.split(',');"
         "      const sensors = ['flex1', 'flex2', 'flex3', 'flex4', 'flex5'];"
         "      sensors.forEach((sensorId, index) => {"
         "        if (index < values.length) {"
         "          const valueElement = document.getElementById(sensorId + '-value');"
         "          if (valueElement) {"
         "            let value = parseInt(values[index]);"
         "            value = isNaN(value) ? 0 : Math.max(0, value);"
         "            valueElement.textContent = value + '°';"
         "          }"
         "        }"
         "      });"
         "      document.getElementById('update-status').textContent = 'อัพเดทล่าสุด: ' + new Date().toLocaleTimeString();"
         "      isUpdating = false;"
         "    })"
         "    .catch(error => {"
         "      console.error('Error:', error);"
         "      isUpdating = false;"
         "    });"
         "}"
         
         "document.addEventListener('DOMContentLoaded', function() {"
         "  console.log('Page loaded');"
         "  startPolling();"
         "});"
         "</script>"
         "</head><body>"
         "<h2>Flex Sensor Monitor</h2>"
         "<div class='sensor-group'>"
         "  <div class='sensor'>"
         "    <div class='sensor-label'>Thumb</div>"
         "    <div id='flex1-value' class='sensor-value'>0°</div>"
         "  </div>"
         "  <div class='sensor'>"
         "    <div class='sensor-label'>Index finger</div>"
         "    <div id='flex2-value' class='sensor-value'>0°</div>"
         "  </div>"
         "</div>"
         "<div class='sensor-group'>"
         "  <div class='sensor'>"
         "    <div class='sensor-label'>Middle finger</div>"
         "    <div id='flex3-value' class='sensor-value'>0°</div>"
         "  </div>"
         "  <div class='sensor'>"
         "    <div class='sensor-label'>Ring finger</div>"
         "    <div id='flex4-value' class='sensor-value'>0°</div>"
         "  </div>"
         "</div>"
         "<div class='sensor-group'>"
         "  <div class='sensor'>"
         "    <div class='sensor-label'>Little finger</div>"
         "    <div id='flex5-value' class='sensor-value'>0°</div>"
         "  </div>"
         "</div>"
         "<div class='status' id='update-status'>กำลังเชื่อมต่อ...</div>"
         "</body></html>";
}

void setup() {
  Serial.begin(9600); // เพิ่มความเร็ว Serial
  while (!Serial && millis() < 5000); // รอ 5 วินาทีสำหรับ Serial หรือข้ามไปถ้าไม่มี Serial
  
  pinMode(modeButton, INPUT_PULLUP);
  pinMode(flexPin1, INPUT);
  pinMode(flexPin2, INPUT);
  pinMode(flexPin3, INPUT);
  pinMode(flexPin4, INPUT);
  pinMode(flexPin5, INPUT);

  Serial.println("Starting Wi-Fi...");
  WiFi.disconnect();
  WiFi.end();  
  delay(500);

  // Start Access Point ด้วยชื่อที่ชัดเจน
  if (WiFi.beginAP("ArduinoFlex", "12345678") == WL_AP_FAILED) {
    Serial.println("Failed to start Access Point");
    while(1) { 
      delay(1000);
      Serial.println("Retrying AP start...");
      if (WiFi.beginAP("ArduinoFlex", "12345678") != WL_AP_FAILED) {
        break;
      }
    }
  }

  Serial.println("Access Point Started");
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // เตรียม HTML หน้าเว็บ
  scanWiFi();
  
  // เริ่ม server
  server.begin();
  Serial.println("Server started");
}

void loop() {
  checkButtonPress();

  // อัพเดทค่า sensors ทุกๆ sendInterval ms
  if (millis() - lastSendTime >= sendInterval) {
    lastSendTime = millis();
    
    // อ่านค่าและแม็ปเป็นองศา
    int flex1 = max(0, map(analogRead(flexPin1), 632, 718, 0, 180));
    int flex2 = max(0, map(analogRead(flexPin2), 680, 797, 0, 180));
    int flex3 = max(0, map(analogRead(flexPin3), 612, 730, 0, 180));
    int flex4 = max(0, map(analogRead(flexPin4), 632, 745, 0, 180));
    int flex5 = max(0, map(analogRead(flexPin5), 825, 903, 0, 180));
    
    // บันทึกค่าใหม่
    lastFlexData = String(flex1) + "," + String(flex2) + "," + 
                  String(flex3) + "," + String(flex4) + "," + 
                  String(flex5);
  }

  // รับการเชื่อมต่อจาก clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Client connected");
    
    // วันและเวลาที่รับการเชื่อมต่อ
    unsigned long connectionTime = millis();
    
    // อ่านคำขอ
    String request = "";
    while (client.connected() && millis() - connectionTime < 1000) {
      if (client.available()) {
        char c = client.read();
        request += c;
        
        // จบคำขอ HTTP
        if (request.indexOf("\r\n\r\n") != -1) {
          break;
        }
      }
    }
    
    // ตรวจสอบประเภทคำขอ
    if (request.indexOf("GET /sensors") != -1) {
      // คำขอ API เพื่อรับข้อมูลเซ็นเซอร์
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/plain");
      client.println("Cache-Control: no-cache, no-store, must-revalidate");
      client.println("Pragma: no-cache");
      client.println("Expires: 0");
      client.println("Access-Control-Allow-Origin: *");
      client.println("Connection: close");
      client.println();
      client.println(lastFlexData);
      Serial.println("Sent sensor data: " + lastFlexData);
    } 
    else if (request.indexOf("GET / ") != -1 || request.indexOf("GET /index.html") != -1 || request.length() < 10) {
      // คำขอหน้าเว็บหลัก
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("Cache-Control: no-cache");
      client.println("Connection: close");
      client.println();
      client.print(html);
      Serial.println("Sent main page");
    }
    else {
      // คำขอไม่รู้จัก
      client.println("HTTP/1.1 404 Not Found");
      client.println("Connection: close");
      client.println();
    }
    
    // ปิดการเชื่อมต่อ
    delay(1);
    client.stop();
  }

  if(useWifi){
    sendDataWiFi(lastFlexData);
  }else{
    sendDataBLE(lastFlexData);
  }
}

void connectWiFi() {
  WiFiClient webClient = server.available();
  if (webClient) {
    Serial.println("New Web Client Connected!");
    String request = "";
    while (webClient.available()) {
      request += webClient.readStringUntil('\n');
    }

    if (request.indexOf("GET /") >= 0) {
      webClient.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
      webClient.print(html);
    } 
    else if (request.indexOf("POST /connect") >= 0) {
      String ssid = getPostValue(request, "ssid");
      String password = getPostValue(request, "password");

      Serial.println("Connecting to: " + ssid);
      WiFi.disconnect();
      WiFi.begin(ssid.c_str(), password.c_str());

      webClient.print("HTTP/1.1 302 Found\r\nLocation: http://192.168.4.1/\r\n\r\n");

      delay(3000);
      NVIC_SystemReset();
    }

    webClient.stop();
  }
}

String getPostValue(String request, String key) {
  int keyIndex = request.indexOf(key + "=");
  if (keyIndex >= 0) {
    int valueStart = keyIndex + key.length() + 1;
    int valueEnd = request.indexOf("&", valueStart);
    if (valueEnd == -1) valueEnd = request.indexOf(" ", valueStart);
    return request.substring(valueStart, valueEnd);
  }
  return "";
}

void reconnectClient() {
  Serial.println("Connecting to server...");
  
  if (client.connect(serverIP, serverPort)) {
    Serial.println("Server connection successful!");
  } else {
    Serial.println("Cannot connect to server!");
  }
}

void startWiFi() {
  Serial.println("Starting Wi-Fi...");

  WiFi.disconnect();
  WiFi.end();  
  delay(500);

  if (WiFi.beginAP("Arduino_Flex", "12345678")) {
    Serial.println("SoftAP Started!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to start SoftAP");
  }

  scanWiFi();
  server.begin();
}

void startBLE() {
  Serial.println("Starting BLE...");
  if (!BLE.begin()) {
    Serial.println("Failed to initialize BLE!");
    while (1);
  }
  
  BLE.setLocalName("FlexTransmitter");
  BLE.setAdvertisedService(flexService);
  flexService.addCharacteristic(flexCharacteristic);
  BLE.addService(flexService);
  BLE.advertise();
  Serial.println("BLE Transmitter is ready!");
}

void checkButtonPress() {
  bool currentButtonState = digitalRead(modeButton);

  if (currentButtonState == LOW && lastButtonState == HIGH) {
    delay(50); // Debounce
    useWifi = !useWifi;

    if (useWifi) {
      Serial.println("Switching to Wi-Fi mode...");
      
      BLE.stopAdvertise();
      BLE.disconnect();
      BLE.end();
      
      delay(500);
      startWiFi();
    } else {
      Serial.println("Switching to BLE mode...");
      server.end();
      WiFi.disconnect();
      WiFi.end();
      
      delay(500);
      startBLE();
    }
  }

  lastButtonState = currentButtonState;
}

void sendDataWiFi(String data) {
  if (client.connected()) {
    client.println(data);
    Serial.println("Sent data: " + data);
  } else {
    // ไม่ต้องพยายามเชื่อมต่อทุกครั้ง เพราะอาจทำให้ล่าช้า
    static unsigned long lastReconnectAttempt = 0;
    if (millis() - lastReconnectAttempt > 5000) { // ลองเชื่อมต่อทุก 5 วินาที
      lastReconnectAttempt = millis();
      reconnectClient();
    }
  }
}

void sendDataBLE(String data) {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.println("Bluetooth Connected to: " + central.address());
    flexCharacteristic.writeValue(data.c_str());
    Serial.println("Sent BLE data: " + data);
  }
}