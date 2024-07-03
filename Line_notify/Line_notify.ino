#include <TridentTD_LineNotify.h>
#include "WiFiS3.h"
#define SSID        "TanaPhat_2.4G"                                     
#define PASSWORD    "00391441"                                   
#define LINE_TOKEN  "01LcNm8RVeik5av6yto08qYrb5CXh5jGG1KUss352h4" 

void setup() {
  Serial.begin(115200); 
  Serial.println(LINE.getVersion());

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.print("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());

  LINE.setToken(LINE_TOKEN);
  //Send Message
  LINE.notify("Hello");
  //Send Number
  LINE.notify(2342);          
  LINE.notify(212.43434, 5);  

  //Send Sticker
  LINE.notifySticker(3, 240);       
  LINE.notifySticker("Hello", 1, 2); 

  //Send Picture
  LINE.notifyPicture("https://cdn.pixabay.com/photo/2023/06/05/01/53/kitten-8041226_1280.jpg");
}

void loop() {
  delay(1);
}