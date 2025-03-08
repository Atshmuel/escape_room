#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DIYables_4Digit7Segment_74HC595.h>

#define LPIN D4  //LOCK PIN

#define SCLK D5  // The ESP8266 pin connected to SCLK
#define RCLK D6  // The ESP8266 pin connected to RCLK
#define DIO D7   // The ESP8266 pin connected to DIO

DIYables_4Digit7Segment_74HC595 display(SCLK, RCLK, DIO);

IPAddress apIP(55, 55, 55, 55);

const char* ssid = "Escape_Room";
const char* password = "123123123";
String gamePassword = "";
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
    body {
        display: flex;
        justify-content: center;
        align-items: center;
        height: 100dvh;
        margin: 0;
        background: url('https://images.unsplash.com/photo-1528459801416-a9e53bbf4e17?ixlib=rb-1.2.1&auto=format&fit=crop&w=1950&q=80') center/cover no-repeat;
        font-family: 'Heebo', sans-serif;
        background-color: #0d1117;
    }
    
    form {
        text-align: center;
        width: 80%;
        max-width: 450px;
        padding: 2.5rem;
        border-radius: 4px;
        background-color: rgba(10, 10, 10, 0.8);
        backdrop-filter: blur(10px);
        box-shadow: 0 15px 35px rgba(0, 0, 0, 0.7), 0 0 0 1px rgba(255, 255, 255, 0.1);
        border: 1px solid rgba(255, 255, 255, 0.1);
    }
    
    h2 {
        color: #ffffff;
        margin-bottom: 1.8rem;
        font-size: 1.8rem;
        font-weight: 300;
        letter-spacing: 0.5px;
        text-shadow: 0 2px 4px rgba(0, 0, 0, 0.5);
    }
    
    input[type='text'] {
        width: 70%;
        padding: 15px 12px;
        margin: 15px 0;
        border: none;
        border-bottom: 2px solid rgba(255, 255, 255, 0.3);
        background-color: rgba(255, 255, 255, 0.05);
        color: #ffffff;
        border-radius: 2px;
        font-size: 1.2rem;
        text-align: center;
        transition: all 0.3s ease;
    }
    
    input[type='text']:focus {
        outline: none;
        border-bottom-color: #ff9100;
        box-shadow: 0 5px 15px rgba(255, 145, 0, 0.2);
        background-color: rgba(255, 255, 255, 0.1);
    }
    
    button {
        background: linear-gradient(45deg, #ff9100, #ff6d00);
        color: white;
        border: none;
        padding: 14px 28px;
        margin-top: 20px;
        border-radius: 50px;
        cursor: pointer;
        font-size: 1.1rem;
        font-weight: 500;
        letter-spacing: 0.5px;
        transition: all 0.3s ease;
        box-shadow: 0 4px 15px rgba(255, 145, 0, 0.4);
    }
    
    button:hover {
        transform: translateY(-3px);
        box-shadow: 0 7px 20px rgba(255, 145, 0, 0.6);
    }
    
    button:active {
        transform: translateY(1px);
    }
    
    .fail {
        background-color: rgba(218, 0, 0, 0.8);
        color: white;
        border-radius: 4px;
        height: fit-content;
        font-size: 1.1rem;
        padding: 1rem;
        margin-top: 15px;
        animation: shake 0.5s cubic-bezier(.36,.07,.19,.97) both;
        box-shadow: 0 5px 15px rgba(218, 0, 0, 0.3);
    }
    
    @keyframes shake {
        0%, 100% { transform: translateX(0); }
        10%, 30%, 50%, 70%, 90% { transform: translateX(-6px); }
        20%, 40%, 60%, 80% { transform: translateX(6px); }
    }
    
    .currect {
        background: linear-gradient(45deg, #00c853, #69f0ae);
        color: #000;
        border-radius: 4px;
        height: fit-content;
        font-size: 1.6rem;
        padding: 1.5rem;
        margin-top: 15px;
        text-align: center;
        animation: success-pulse 2s infinite;
        box-shadow: 0 8px 25px rgba(105, 240, 174, 0.5);
    }
    
    @keyframes success-pulse {
        0% { box-shadow: 0 0 0 0 rgba(105, 240, 174, 0.7); }
        70% { box-shadow: 0 0 0 15px rgba(105, 240, 174, 0); }
        100% { box-shadow: 0 0 0 0 rgba(105, 240, 174, 0); }
    }
    
    @media (max-width: 768px) {
        form {
            width: 85%;
            padding: 2rem;
        }
        
        input[type='text'] {
            width: 85%;
        }
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

  form += "</form>";
  html += form;
  html += "</body>";
  html += "</html>";
  server.send(200, "text/html", html);
}

void handleMissionComplited() {
  Serial.println("GET");
  if(server.hasArg("gameOn")){
    digitalWrite(LPIN, true);
  }
  if (server.hasArg("missionCode") && gamePassword.length() < 4) {
    if (server.arg("missionCode").length() == 1) {
      gamePassword = gamePassword + server.arg("missionCode");
      display.printInt(gamePassword.toInt(), true);
      server.send(200, "text/plain", "ok");
    }
    server.send(404, "text/plain", "password too long");
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
