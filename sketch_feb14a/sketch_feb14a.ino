#include <DIYables_4Digit7Segment_74HC595.h>

#define SCLK  D5  // The ESP8266 pin connected to SCLK
#define RCLK  D6  // The ESP8266 pin connected to RCLK
#define DIO   D7  // The ESP8266 pin connected to DIO

DIYables_4Digit7Segment_74HC595 display = DIYables_4Digit7Segment_74HC595(CLK, DIO);
// display.setNumber(1, 9);  // set 9 at the 1st digit


void setup() {
  Serial.begin(9600);
  display.clear();

}

void loop() {
   display.loop();

}
