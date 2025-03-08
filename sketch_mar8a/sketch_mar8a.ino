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

#define Fan D4

#define DHT D7



String state = "init";
// stageOne = "1";
// stageTwo = "2";
// stageThree = "3";
// stageFour = "4";
// end = "end";


// stageOne Params:
unsigned long startTime = 0;
bool started = false;
bool success = false;



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
  Serial.begin(9600);
}

void preStart() {
  int val = map(ReadMuxChannel(3), 0, 1023, 0, 255);
  handleAllLeds(val);
  if (val == 255) {
    state = "1";
    Serial.println("START");
  }
  Serial.println(val);
}

void handleAllLeds(int val){
  analogWrite(LR,val);
  analogWrite(LB,val);
  analogWrite(LG,val);
  analogWrite(LY,val);
}

void stageOne() {
  int val = map(ReadMuxChannel(3), 0, 1023, 0, 255);
  
  if (val <= (255 * 0.8) && !started) {
            Serial.println((255 * 0.8));

    startTime = millis();
    started = true;
  }

  if (started) {
    if (val > (255 * 0.8)) {
      started = false;
      startTime = millis();
    } else {
      if (millis() - startTime >= 2000) {
        Serial.println("Stage 1 Success");
        state = "2";
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
    handleAllLeds(val);
  }
  if (state == "2") {
    Serial.println("2");
    delay(10000);
  }
}
