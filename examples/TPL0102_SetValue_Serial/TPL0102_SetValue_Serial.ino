

/*
      TI TPL0102 Library

      Author: Daniel Melendrez
      Code: Example code for library usage. Setting value via the Serial Port
      Ver: 0.1 - initial release
           0.2 - General cleanup
      Date: March 2020
*/

#include <TPL0102.h>
#include <Bounce2.h>    // Library from: https://github.com/thomasfredericks/Bounce2

#define TPL0102_ADDRESS 0x50 //0x5x where x is 0-7 according to A2A1A0 (I removed this from header to allow multiple pots in the same I2C bus)

/* **** VARIABLES *******************/
// Floats
unsigned long elapsed;    // To measure how long it takes to change the value
float Res;        // Hold the theoretical current value
float Rx;   // Holds the desired Resistor value

// Bytes
uint8_t chanPtr = 0;    // Channel pointer
uint8_t tapNum;         // Current tap pointer --> [0,255]
uint8_t target;         // Computed target tap according to the desired value

/*Pins setup */
// LEDs
const int potA_LED = 9;   // Indicator for Pot A selected
const int potB_LED = 10;  // Indicator for Pot B selected

// Push Buttons
const int pinSel = 8;   // Used to toggle between channels

// Strings

String serialData = "";   // I know, I know. Strings are "evil" but what do you want from me?

/* ***** FLAGS  *********/
bool debug = false;           // Enable it to get debugging "powers"
bool serialFinished = false;  // Indicates when serial value has been received upon finding \n

Bounce sel = Bounce();    // Instantiate a new debouncing object
TPL0102 pot = TPL0102(potA_LED, potB_LED, debug);       // Instantiate a new awesome pot

void setup() {

  Serial.begin(115200);

  pinMode(pinSel, INPUT_PULLUP);

  // Assign push button to object
  sel.attach(pinSel);

  // Define the debouncing time
  sel.interval(15);

  delay(200);

  if (digitalRead(pinSel) == LOW ) {

    debug = true;
  }

  delay(200);

  Serial.println(F("*****************************************"));
  Serial.println(F("  TPL0102 256 taps Digital Potentiometer "));
  Serial.println(F("               LIBRARY ver 0.1           "));
  Serial.println(F("     Set Pot value via serial port       "));
  Serial.println(F("*****************************************"));

  pot.begin(TPL0102_ADDRESS);

  toggleLeds();

}

void loop() {

  // Monitor a change in Selection button

  if (sel.update()) {
    if (sel.fell()) {

      chanPtr++;

      if (chanPtr > 1)
        chanPtr = 0;

	  pot.setChannel(chanPtr);
	  
	  Serial.print(pot.POT_LABELS[chanPtr]);
      Serial.println(pot.taps(chanPtr));

      toggleLeds();

    }
  }

  // When serial value has been received send it to the channel pointed  at
  if (serialFinished) {

    serialFinished = false;

    Rx = serialData.toFloat();

    if ((Rx >= 0.0) && (Rx <= NOMINAL_RESISTANCE)) {

      // Read current pot status
      target = pot.setValue(chanPtr, Rx);
      Res = pot.readValue(chanPtr);
      tapNum = pot.taps(chanPtr);
      elapsed = pot.setMicros();


      // Print data on the serial port
      Serial.println(F("**********************"));
      Serial.print(F("Desired Resistance: "));
      Serial.println(Rx, 2);

      Serial.print(F("Current Resistance: "));
      Serial.println(Res, 2);

      Serial.print(F("Target Tap: "));
      Serial.println(target);

      Serial.print(F("Current Tap: "));
      Serial.println(tapNum);

      Serial.print(F("Elapsed: "));
      Serial.println(elapsed);

      Serial.println(F("**********************"));

      serialData = "";

    }
  }
}

void toggleLeds() {

  // Not a fancy way to toggle LEDs, I know ;)
  if (chanPtr == 0) {
    digitalWrite(potA_LED, HIGH);
    digitalWrite(potB_LED, LOW);
  } else {
    digitalWrite(potA_LED, LOW);
    digitalWrite(potB_LED, HIGH);
  }
}

// Serial string management
void serialEvent() {
  // Read serial input:
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      // convert the incoming byte to a char and add it to the string:
      serialData += (char)inChar;
    }
    // if you get a newline, print the string, then the string's value:
    if (inChar == '\n') {
      // clear the string for new input:
      serialFinished = true;
    }
  }
}
