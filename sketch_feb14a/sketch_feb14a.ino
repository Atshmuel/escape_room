#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

WiFiClient client; 

bool a = true;
unsigned long awaitTime = 0;


void setup() {
  Serial.begin(9600);
  wifiSetup();
  awaitTime = millis();
}

void loop() {
  wifiLoop();
  if(a && millis() - awaitTime > 5000) {
    Serial.println("FETCH");
    SendData(7);
    a = false;
  }
}


void SendData(int val) {
  HTTPClient http;
  String dataURL = "http://55.55.55.55/api?missionCode=";
  dataURL += String(val);
  Serial.println(dataURL);
  http.begin(client,dataURL);
  int httpCode = http.GET();
  Serial.println(httpCode);
  http.end();
}