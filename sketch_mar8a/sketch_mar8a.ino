#include <DHT.h>

#define LR_MOTOR_A D1
#define LG D3
#define LB D4
#define LY_MOTOR_B D2

#define BR_DHT_PIN D8
#define BG_pinMuxC D7
#define BB_pinMuxB D6
#define BY_pinMuxA D5
#define pinMuxInOut A0



#define NumOfLeds 4
#define MaxNumOfLights 8

#define StartGame 30
#define One 31
#define Two 32
#define Three 33
#define Four 34
#define EndGame 35


#define DHTTYPE DHT11
DHT dht(BR_DHT_PIN, DHTTYPE);
float lastTemp = 0;

int Leds[NumOfLeds] = { LR_MOTOR_A, LG, LB, LY_MOTOR_B };
int Btns[NumOfLeds] = { BR_DHT_PIN, BG_pinMuxC, BB_pinMuxB, BY_pinMuxA };
int ChosenIndexes[MaxNumOfLights];
int btnsPressedIndexes[MaxNumOfLights];
int btnsI = 0;
int val[NumOfLeds];
int lastVal[NumOfLeds];
unsigned long lastPressTime[NumOfLeds];
int VrX;
int VrY;
int state = StartGame;
unsigned long startTime = 0;
bool started = false;


int ReadMuxChannel(byte chnl) {
  int a = (bitRead(chnl, 0) > 0) ? HIGH : LOW;
  int b = (bitRead(chnl, 1) > 0) ? HIGH : LOW;
  int c = (bitRead(chnl, 2) > 0) ? HIGH : LOW;
  digitalWrite(BY_pinMuxA, a);
  digitalWrite(BB_pinMuxB, b);
  digitalWrite(BG_pinMuxC, c);
  int ret = analogRead(pinMuxInOut);
  return ret;
}

void setup() {
  randomSeed(analogRead(A0));
  stageOne_SetUp();
  Leds_Setup();
  Serial.begin(9600);
}

void stageOne_SetUp() {
  pinMode(BY_pinMuxA, OUTPUT);
  pinMode(BB_pinMuxB, OUTPUT);
  pinMode(BG_pinMuxC, OUTPUT);
  pinMode(pinMuxInOut, INPUT);
  pinMode(LR_MOTOR_A, OUTPUT);
  pinMode(LB, OUTPUT);
  pinMode(LY_MOTOR_B, OUTPUT);
  pinMode(LG, OUTPUT);
}
void stageTwo_SetUp() {
  dht.begin();
  pinMode(LR_MOTOR_A, OUTPUT);
  pinMode(LY_MOTOR_B, OUTPUT);
}
void stageThree_SetUp() {
  pinMode(LR_MOTOR_A, OUTPUT);
  pinMode(LB, OUTPUT);
  pinMode(LY_MOTOR_B, OUTPUT);
  pinMode(LG, OUTPUT);
  pinMode(BR_DHT_PIN, INPUT_PULLUP);
  pinMode(BB_pinMuxB, INPUT_PULLUP);
  pinMode(BY_pinMuxA, INPUT_PULLUP);
  pinMode(BG_pinMuxC, INPUT_PULLUP);
}
void stageFour_SetUp() {
  pinMode(BY_pinMuxA, OUTPUT);
  pinMode(BB_pinMuxB, OUTPUT);
  pinMode(BG_pinMuxC, OUTPUT);
  pinMode(pinMuxInOut, INPUT);
}

void setOnFwd() {
  digitalWrite(LR_MOTOR_A, LOW);
  digitalWrite(LY_MOTOR_B, HIGH);
}

void fanOff() {
  digitalWrite(LR_MOTOR_A, LOW);
  digitalWrite(LY_MOTOR_B, LOW);
}

void handleAllLeds(int val) {
  analogWrite(LR_MOTOR_A, val);
  analogWrite(LB, val);
  analogWrite(LG, val);
  analogWrite(LY_MOTOR_B, val);
}
int getRandomNumber() {
  return random(10);
}

void preStart() {
  int val = map(ReadMuxChannel(3), 0, 1023, 0, 255);
  handleAllLeds(val);
  if (val == 255) {
    state = One;
    SendData(getRandomNumber(), false);
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
        state = Two;
        stageTwo_SetUp();
        started = false;
        SendData(getRandomNumber(), true);
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
        state = Three;
        stageThree_SetUp();
        SendData(getRandomNumber(), true);
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
        state = Four;
        stageFour_SetUp();
        SendData(getRandomNumber(), true);
        started = false;
        Serial.println("Stage 3 Success");
        delay(1000);
      }
    }
  }
}

void stageFour() {
  VrX = ReadMuxChannel(4);
  VrY = ReadMuxChannel(5);
  delay(500);
  if ((VrX == 1024 && VrY == 1024) || (VrX == 9 && VrY == 1024) || (VrX == 1024 && VrY == 12) || (VrX == 9 && VrY == 7)) {
    state = EndGame;
    SendData(getRandomNumber(), true);
    Serial.println("Stage 4 Success");
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

  switch (state) {
    case StartGame: preStart(); break;
    case One: stageOne(); break;
    case Two: stageTwo(); break;
    case Three: stageThree(); break;
    case Four: stageFour(); break;
  }
}
