
void SendData(int val) {
  HTTPClient http;
  String dataURL = "";
  dataURL += "missionCode=" + String(val);
  http.begin(client, "http://55.55.55.55/api/?" + dataURL);
  int httpCode = http.GET();
  Serial.println(httpCode);
  http.end();
}

void setup() {
  wifiSetup();
  Serial.begin(9600);
}

void loop() {
  wifiLoop();
}
