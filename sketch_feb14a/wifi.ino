#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

IPAddress apIP(55, 55, 55, 55);

const char* ssid = "Server";
const char* lockPassword = "111222";
const char* password = "123123123";

ESP8266WebServer server(80);

void handleNotFound() {

  String message = "File Not Found \n \n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}

void handleRoot() {
  server.send(200, "text/plain", html);
}


void wifiSetup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  Serial.print("AP IP address: ");
  Serial.println(apIP);

  server.begin();
}


void wifiLoop() {
  server.handleClient();
  delay(100);
}
