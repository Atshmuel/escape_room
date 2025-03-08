#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>


const char* ssid = "Escape_Room";
const char* pswd = "123123123";



WiFiClient client;
int server_port = 80;

unsigned long delayMillis = 0;

void wifi_Setup() {
  Serial.println("wifiSetup");
  WiFi.begin(ssid, pswd);

  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - delayMillis >= 100) {
      Serial.println("trying ...");
      delayMillis = millis();
    }
  }
  Serial.println("Connected to network");
}

void SendData(int val, bool mission) {
  HTTPClient http;
  String dataURL = "http://55.55.55.65/?name=";
  dataUrl += mission ? "missionCode" : "gameOn";

  if (mission) {
    dataURL += String(val);
  }
  Serial.println(dataURL);

  http.begin(client, dataURL);
  int httpCode = http.GET();
  Serial.println(httpCode);
  http.end();
}