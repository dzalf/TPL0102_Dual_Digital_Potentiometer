
/*
      TI TPL0102 Library

      Author: Daniel Melendrez
      Ver: 0.1
      Date: March 2020
*/

#include <TPL0102.h>
#include <Bounce2.h>

#define TPL0102_ADDRESS 0x50 //0x5x where x is 0-7 according to A2A1A0 (I removed this from header to allow multiple pots in the same I2C bus)

/* **** VARIABLES *******************/
// Floats
unsigned long elapsedInc;
unsigned long elapsedDec;
float pos = 0;
float Res;

// Bytes
uint8_t chanPtr = 0;
uint8_t tapNum;

// Ints
int potState = HIGH;

/*Pins setup */
// LEDs
const int upLed = 9;
const int dwnLed = 10;
// Push Buttons
const int pinUp = 6;
const int pinDwn = 7;
const int pinSel = 8;

/* ***** FLAGS  *********/
bool debug = false;
bool incFlag = false;
bool decFlag = false;

Bounce up = Bounce();
Bounce dwn = Bounce();
Bounce sel = Bounce();

TPL0102 pot(debug);

void setup() {

  Serial.begin(115200);

  pinMode(pinUp, INPUT_PULLUP);
  pinMode(pinDwn, INPUT_PULLUP);
  pinMode(pinSel, INPUT_PULLUP);
  pinMode(upLed, OUTPUT);
  pinMode(dwnLed, OUTPUT);

  delay(200);

  if (digitalRead(pinSel) == LOW ) {

    debug = true;
  }

  delay(200);

  digitalWrite(upLed, LOW);
  digitalWrite(dwnLed, LOW);

  up.attach(pinUp);
  dwn.attach(pinDwn);
  sel.attach(pinSel);
  // Define the debouncing time
  up.interval(25);
  dwn.interval(25);
  sel.interval(25);

  Serial.println(F("****************************************"));
  Serial.println(F("  TPL0102 256 taps Digital Potentiometer "));
  Serial.println(F("               LIBRARY ver 0.1           "));
  Serial.println(F("*****************************************"));

  pot.begin(TPL0102_ADDRESS);

  toggleLeds();

}

void loop() {

  if (sel.update()) {
    if (sel.fell()) {

      chanPtr++;

      if (chanPtr > 1)
        chanPtr = 0;

      Serial.print(pot.POT_LABELS[chanPtr]);
      Serial.println(pot.taps(chanPtr));
      toggleLeds();

    }
  }

  // Resistance increase routine
  if (up.update()) {
    if (up.fell()) {

      pot.inc(chanPtr);

      elapsedInc = pot.incMicros();

      incFlag = true;

      readPotData(chanPtr);
      printRetrievedData();

    }
  }

  // Resistance decrease routine
  if (dwn.update()) {
    if (dwn.fell()) {

      pot.dec(chanPtr);

      elapsedDec = pot.decMicros();

      decFlag = true;

      readPotData(chanPtr);
      printRetrievedData();

    }
  }
}

void toggleLeds() {

  if (chanPtr == 0) {
    digitalWrite(upLed, HIGH);
    digitalWrite(dwnLed, LOW);
  } else {
    digitalWrite(upLed, LOW);
    digitalWrite(dwnLed, HIGH);
  }
}

void readPotData(uint8_t ch) {

  // Retrieve Pot data
  tapNum = pot.taps(ch);
  pos = pot.wiper(ch);
  Res = pot.readValue(ch);

}

void printRetrievedData() {

  // Print data on serial port
  Serial.println(F("*********************************"));
  Serial.print(pot.POT_LABELS[chanPtr]);
  Serial.print(" Tap --> ");
  Serial.println(tapNum);

  Serial.print(F("Resistance (approx): "));
  Serial.print(Res / 1000, 3);

  Serial.println(F(" kOhms;"));

  Serial.print(F("Wiper[%]: "));
  Serial.println(pos * 100, 2);

  if (incFlag) {
    Serial.print(F("Increment time: "));
    Serial.print(elapsedInc);
    Serial.println(F(" us"));
    Serial.println(F("*********************************"));

    incFlag = false;

  }

  if (decFlag) {

    Serial.print(F("Decrement time: "));
    Serial.print(elapsedDec);
    Serial.println(F(" us"));
    Serial.println(F("*********************************"));

    decFlag = false;
  }
}
