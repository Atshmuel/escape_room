#include <DHT.h>

#define LR D1
#define LG D3
#define LB D4
#define LY D2

#define BR D8
#define BG D7
#define BB D6
#define BY D5

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

int Leds[NumOfLeds] = {LR, LG, LB, LY };
int Btns[NumOfLeds] = {BR, BG, BB, BY };
int ChosenIndexes[MaxNumOfLights];
int btnsPressedIndexes[MaxNumOfLights];
int btnsI = 0;
int val[NumOfLeds];
int lastVal[NumOfLeds];
unsigned long lastPressTime[NumOfLeds];


String state = "init";
// stageOne = "1";
// stageTwo = "2";
// stageThree = "3";
// stageFour = "4";
// end = "end";


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
  pinMode(pinMuxInOut, INPUT);
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

void stageThree() {
  if (!started) {
    Leds_Setup();
    btnsI = 0;
    for (int i = 0; i < MaxNumOfLights; i++) {
      ChosenIndexes[i] = -1;
      btnsPressedIndexes[i] = -1;
    }
    ChooseRandomLights();
    ShowLights();
    started = true;
  }

  if (started) {
    int btn = GetPressedBtn();
    if (btn != -1 && btnsI < MaxNumOfLights) {
      Serial.println(btn);
      Serial.println(btnsI);

      btnsPressedIndexes[btnsI++] = btn;
    }
    if (btnsI == MaxNumOfLights) {
      for (int i = 0; i < MaxNumOfLights; i++) {
        if (btnsPressedIndexes[i] != ChosenIndexes[i]) {
          started = false;
          break;
        }
      }
      if (started) {
        state = "4";
        SendData(7, true);
        started = false;
        Serial.println("Stage 3 Success");
        delay(1000);
      }
    }
  }
}
int GetPressedBtn() {
  int BtnPressed = -1;
  for (int i = 0; i < NumOfLeds; i++) {
    val[i] = digitalRead(Btns[i]);
    if ((val[i] == LOW) && (lastVal[i] == HIGH) && (millis() - lastPressTime[i] > 150)) {
      lastPressTime[i] = millis();
      BtnPressed = i;
    }
    lastVal[i] = val[i];
  }
  return BtnPressed;
}
void ShowLights() {
  for (int i = 0; i < MaxNumOfLights; i++) {
    LedOn(ChosenIndexes[i]);
    delay(1000);
    LedOff(ChosenIndexes[i]);
    delay(500);
  }
}
void Leds_Setup() {
  for (int i = 0; i < NumOfLeds; i++) {
    LedOff(i);
  }
}

void ChooseRandomLights() {
  int rndNum;
  for (int i = 0; i < MaxNumOfLights; i++) {
    rndNum = random(0, NumOfLeds);
    ChosenIndexes[i] = rndNum;
  }
}
void LedOn(int chnl) {
  digitalWrite(Leds[chnl], HIGH);
}
void LedOff(int chnl) {
  digitalWrite(Leds[chnl], LOW);
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
