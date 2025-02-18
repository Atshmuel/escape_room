#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define RedLed 1
#define light A0

WiFiClient client; 

bool a = true;
unsigned long awaitTime = 0;
bool isDimmed= false;


void setup() {
  Serial.begin(9600);
  wifiSetup();
  awaitTime = millis();
  pinMode(RedLed, OUTPUT);

}

void loop() {
  wifiLoop();
  ControlLights();
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

void ControlLights(){
  int potVal = map(analogRead(light), 0, 1023, 0, 255);
  int dimLight= potVal*0.8;
  if(!isDimmed){
    analogWrite(RedLed,dimLight);
    awaitTime= millis();
    isDimmed= true;
  }
  if(isDimmed && millis()- awaitTime = 2000){
    SendData(8);
    analogWrite(RedLed,potVal);
    isDimmed= false;
  }

}


