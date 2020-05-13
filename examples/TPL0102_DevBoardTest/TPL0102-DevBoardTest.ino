
/*
      TI TPL0102 Dev Board Test Example
*/

#include <TPL0102.h>

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

/*Pins setup */
// LEDs
const int PotALED = 10;
const int PotBLED = 9;

// Evil Strings

String serialIn = " ";

/* ***** FLAGS  *********/
bool debug = true;
bool incFlag = false;
bool decFlag = false;
bool serialFinished = false;

TPL0102 pot(PotALED, PotBLED);

void setup() {

  Serial.begin(115200);

  pot.begin(TPL0102_ADDRESS, FAST);

  /*
  for (int i = 0; i < 2; i++) {
    pot.zeroWiper(i);
    delay(200);
  }

  */
  
  delay(200);

  Serial.println(F("****************************************"));
  Serial.println(F("  TPL0102 256 taps Digital Potentiometer "));
  Serial.println(F("               LIBRARY ver 0.1           "));
  Serial.println(F("********  New Development Board   *******"));

}

void loop() {

  if (serialFinished) {

    serialFinished = false;

    Serial.print("In >> ");
    Serial.println(serialIn);

    if (serialIn.toInt() == 4) {

      chanPtr = pot.setChannel(1);

      Serial.print(pot.POT_LABELS[chanPtr]);
      Serial.println(pot.taps(chanPtr));

    } else if (serialIn.toInt() == 6) {

      chanPtr = pot.setChannel(0);

      Serial.print(pot.POT_LABELS[chanPtr]);
      Serial.println(pot.taps(chanPtr));

    } else if (serialIn.toInt() == 8) { // Resistance increase routine

      pot.inc(chanPtr);

      elapsedInc = pot.incMicros();

      incFlag = true;

      readPotData(chanPtr);
      printRetrievedData();


    } else  if (serialIn.toInt() == 2) { // Resistance decrease routine

      pot.dec(chanPtr);

      elapsedDec = pot.decMicros();

      decFlag = true;

      readPotData(chanPtr);
      printRetrievedData();

    }

    serialIn = "";
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

// Serial string management
void serialEvent() {
  // Read serial input:
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      // convert the incoming byte to a char and add it to the string:
      serialIn += (char)inChar;
    }
    // if you get a newline, print the string, then the string's value:
    if (inChar == '\n') {
      // clear the string for new input:
      serialFinished = true;
    }
  }
}
