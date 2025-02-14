#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DIYables_4Digit7Segment_74HC595.h>

#define LPIN D3  //LOCK PIN

#define SCLK D5  // The ESP8266 pin connected to SCLK
#define RCLK D6  // The ESP8266 pin connected to RCLK
#define DIO D7   // The ESP8266 pin connected to DIO


DIYables_4Digit7Segment_74HC595 display = DIYables_4Digit7Segment_74HC595(CLK, DIO);

IPAddress apIP(55, 55, 55, 55);

const char* ssid = "Escape_Room";
char* gamePassword = "";
int qCnt = 4;
const char* password = "123123123";
unsigned long wifiTime = 0;

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
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html lang="en">
    <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
     <title>חדר בריחה</title>
    <style>
        body{
            display: flex;
            justify-content: center;
            height: 100dvh;
        }
        form{
            text-align: center;
            width: 30%;
            height: 10rem;
        }
        .fail{
            background-color: red;
            height: fit-content;
            font-size: 1.2rem;
            padding: .5rem;
        }

        .currect{
            background-color: greenyellow;
            height: fit-content;
            font-size: 1.6rem;
            padding: .7rem;
            text-align: center;
        }
    </style>
    </head>
  <body dir="rtl">)rawliteral";
  html += "<form method='get'>";
  html += "<h2>הכנס את הקוד על מנת לצאת מהחדר</h2>";
  html += "<input type='text' name='password'>";
  html += "<input type='button' value='שלח'>";

  if (server.hasArg("password")) {
    if (server.arg("password").length() != 4) {
      html += "<p class='fail'>לא נכון, נסה שוב</p>";
    } else {
      if (server.arg("password") == gamePassword) {
        digitalWrite(lockPin, false);
        html = "<form method='get'>";
        html += "<p class='currect'>הצלחת לצאת מהחדר ! <br> ניפגש בפעם הבאה</p>";
      } else {
        html += : "<p class='fail'>לא נכון, נסה שוב</p>";
      }
    }

    html += "</form>";
    html += "</body>";
    html += "</html>";
    server.send(200, "text/html", html);
  }

  void handleMissionComplited() {
    if (server.hasArg("missionCode")) {
      gamePassword = gamePassword + server.arg("missionCode");
      display.setNumber(qCnt, server.arg("missionCode").toInt());
      qCnt--;
      server.send(200);
    }else{
      server.send(400);
    }
  }


  void wifiSetup() {
    Serial.begin(9600);

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(ssid, password);

    server.on("/", handleRoot);
    server.on("/api", handleMissionComplited);
    server.onNotFound(handleNotFound);

    Serial.print("AP IP address: ");
    Serial.println(apIP);
    pinMode(lockPin, OUTPUT);
    digitalWrite(lockPin, true);
    wifiTime = millis();
    display.clear();
    server.begin();
  }

  void wifiLoop() {
    if (millis() - wifiTime >= 10) {
      wifiTime = millis();
      server.handleClient();
    }
    display.loop();
  }
