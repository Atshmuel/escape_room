#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DIYables_4Digit7Segment_74HC595.h>

#define LPIN D3  //LOCK PIN

#define SCLK D5  // The ESP8266 pin connected to SCLK
#define RCLK D6  // The ESP8266 pin connected to RCLK
#define DIO D7   // The ESP8266 pin connected to DIO


DIYables_4Digit7Segment_74HC595 display(SCLK, RCLK, DIO);

IPAddress apIP(55, 55, 55, 55);

const char* ssid = "Escape_Room";
String gamePassword = "";
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
  String form = R"rawliteral(

    <form method='get'>
    <h2>הכנס את הקוד על מנת לצאת מהחדר</h2>
<input type='text' name='password'>
<button type='submit'>שלח</button>
  )rawliteral";


  if (server.hasArg("password")) {
    if (server.arg("password").length() != 4) {
      form += "<p class='fail'>לא נכון, נסה שוב</p>";
    } else {
      if (server.arg("password") == gamePassword) {
        digitalWrite(LPIN, false);
        form = R"rawliteral(
    <form method='get'>
      <p class='currect'>הצלחת לצאת מהחדר ! <br> ניפגש בפעם הבאה</p>
    )rawliteral";
      } else {
        form += "<p class='fail'>לא נכון, נסה שוב</p>";
      }
    }
  }

  form+= "</form>";
  html += form;
  html += "</body>";
  html += "</html>";
  server.send(200, "text/html", html);
}

void handleMissionComplited() {
  Serial.println("GET");
  if (server.hasArg("missionCode")) {
    if (server.arg("missionCode").length() == 1) {
      gamePassword = server.arg("missionCode") + gamePassword;
      display.printInt(gamePassword.toInt(), true);
      server.send(200, "text/plain", "ok");
    }
    server.send(404, "text/plain", "password to long");
  } else {
    server.send(404, "text/plain", "error");
  }
}


void wifiSetup() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);

  server.on("/api", handleMissionComplited);
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  Serial.print("AP IP address: ");
  Serial.println(apIP);
  pinMode(LPIN, OUTPUT);
  digitalWrite(LPIN, true);
  wifiTime = millis();
  display.clear();
  display.printInt(0, true);
  server.begin();
}

void wifiLoop() {
  if (millis() - wifiTime >= 10) {
    wifiTime = millis();
    server.handleClient();
  }
  display.loop();
}
