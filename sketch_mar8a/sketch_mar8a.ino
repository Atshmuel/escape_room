#include <DHT.h>

#define LR D1
#define LG D2
#define LB D3
#define LY D4

#define BR D1
#define BG D2
#define BB D3
#define BY D4

#define pinMuxA D5
#define pinMuxB D6
#define pinMuxC D7
#define pinMuxInOut A0

#define MOTOR_A D1
#define MOTOR_B D2

#define NumOfLeds 4
#define MaxNumOfLights 8

#define DHTTYPE DHT11  // works with dht11
#define DHT_PIN D8
DHT dht(DHT_PIN, DHTTYPE);
float lastTemp = 0;

int Leds[NumOfLeds] = { LR, LG, LB, LY };
int ChosenIndexes[MaxNumOfLights];

String state = "init";
// stageOne = "1";
// stageTwo = "2";
// stageThree = "3";
// stageFour = "4";
// end = "end";


// stageOne Params:
unsigned long startTime = 0;
bool started = false;


int ReadMuxChannel(byte chnl) {
  int a = (bitRead(chnl, 0) > 0) ? HIGH : LOW;
  int b = (bitRead(chnl, 1) > 0) ? HIGH : LOW;
  int c = (bitRead(chnl, 2) > 0) ? HIGH : LOW;
  digitalWrite(pinMuxA, a);
  digitalWrite(pinMuxB, b);
  digitalWrite(pinMuxC, c);
  int ret = analogRead(pinMuxInOut);
  return ret;
}

void setup() {
  pinMode(pinMuxA, OUTPUT);
  pinMode(pinMuxB, OUTPUT);
  pinMode(pinMuxC, OUTPUT);
  pinMode(LR, OUTPUT);
  pinMode(LB, OUTPUT);
  pinMode(LY, OUTPUT);
  pinMode(LG, OUTPUT);
  pinMode(BR, INPUT_PULLUP);
  pinMode(BB, INPUT_PULLUP);
  pinMode(BY, INPUT_PULLUP);
  pinMode(BG, INPUT_PULLUP);
  pinMode(pinMuxInOut, INPUT);
  pinMode(MOTOR_A, OUTPUT);
  pinMode(MOTOR_B, OUTPUT);
  randomSeed(analogRead(A0));
  for (int k = 0; k < MaxNumOfLights; k++) {
    ChosenIndexes[k] = -1;
  }
  dht.begin();
  Serial.begin(9600);
}

void setOnFwd() {
  digitalWrite(MOTOR_A, LOW);
  digitalWrite(MOTOR_B, HIGH);
}

void fanOff() {
  digitalWrite(MOTOR_A, LOW);
  digitalWrite(MOTOR_B, LOW);
}

void handleAllLeds(int val) {
  analogWrite(LR, val);
  analogWrite(LB, val);
  analogWrite(LG, val);
  analogWrite(LY, val);
}
void preStart() {
  int val = map(ReadMuxChannel(3), 0, 1023, 0, 255);
  handleAllLeds(val);
  if (val == 255) {
    state = "1";
    SendData(0, false);
    Serial.println("START");
    delay(1000);
  }
}

void stageOne() {
  int val = map(ReadMuxChannel(3), 0, 1023, 0, 255);
  handleAllLeds(val);

  if (val <= (255 * 0.8) && !started) {
    Serial.println((255 * 0.8));

    startTime = millis();
    started = true;
  }

  if (started) {
    Serial.println(val);
    if (val > (255 * 0.8)) {
      started = false;
      startTime = millis();
    } else {
      if (millis() - startTime >= 2000) {
        state = "2";
        started = false;
        SendData(5, true);
        Serial.println("Stage 1 Success");
        delay(1000);
      }
    }
  }
}

void stageTwo() {
  setOnFwd();
  if (!started) {
    lastTemp = dht.readTemperature();
    Serial.print("Target: ");
    Serial.println(lastTemp - 2);
    started = true;
  }
  if (started) {
    delay(5000);
    float currentTemp = dht.readTemperature();
    Serial.println(currentTemp);
    if (currentTemp <= lastTemp - 2) {
      if (millis() - startTime >= 2000) {
        state = "3";
        SendData(7, true);
        started = false;
        fanOff();
        Serial.println("Stage 2 Success");
        delay(1000);
      }
    }
  }
}

void loop() {
  if (state == "init") {
    preStart();
  }
  if (state == "1") {
    stageOne();
  }
  if (state == "2") {
    stageTwo();
  }
  if(state == "3"){
    stageThree();
  }
}
