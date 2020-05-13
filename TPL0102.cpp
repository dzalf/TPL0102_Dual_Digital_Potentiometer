/*
    Microchip 64 taps Single Digital Potentiometer
    Simple two-wire UP/DOWN interface
    Author: Daniel Melendrez
    Date: March 2020 (COVID-19 Vibes)
    Version history:    0.1 - March 18 - Initial commit
                        0.2 - March 18 - Added FAST mode to the I2C comm: help to set the value in less than 100 usec
                        0.3 - May 14   - Included new control over LEDs connected to the Dev Boards that I designed. 
                                         Now LEDs can be instantiated through new overloaded constructors
    License: MIT                

*/

#include "TPL0102.h"

// Constructors

TPL0102::TPL0102() {

 _debug = false;
 _ledsDefined = false;
 _selectedChannel = 0;    // By default Pot A is selected

}

TPL0102::TPL0102(bool DEBUG) {

  _debug = DEBUG;
  _ledsDefined = false;

  _selectedChannel = 0;    // By default Pot A is selected

}

TPL0102::TPL0102(uint8_t ledA, uint8_t ledB) {

  _debug = false;
  _ledsDefined = true;

  _selectedChannel = 0;    // By default Pot A is selected

  _pinLedA = ledA;
  _pinLedB = ledB;

  pinMode(_pinLedA, OUTPUT);
  pinMode(_pinLedB, OUTPUT);

  _boardLEDs[0] = _pinLedA;
  _boardLEDs[1] = _pinLedB;

  _prevLEDsState[0] = LOW;
  _prevLEDsState[1] = LOW;

  toggleLED(_selectedChannel);

}

TPL0102::TPL0102(uint8_t ledA, uint8_t ledB, bool DEBUG) {

  _debug = DEBUG;
  _ledsDefined = true;

  _selectedChannel = 0;    // By default Pot A is selected

  _pinLedA = ledA;
  _pinLedB = ledB;

  pinMode(_pinLedA, OUTPUT);
  pinMode(_pinLedB, OUTPUT);

  _boardLEDs[0] = _pinLedA;
  _boardLEDs[1] = _pinLedB;

  _prevLEDsState[0] = LOW;
  _prevLEDsState[1] = LOW;

  toggleLED(_selectedChannel);

}

// Methods

void TPL0102::begin(uint16_t addr, uint32_t speed) {
  
  Wire.begin();
  Wire.setClock(speed);

  address = addr;
  // I need to assign the previous value!
  readRegistersStatus();
  
  _tapPointer[0] = _initialState[0];
  _tapPointer[1] = _initialState[1];

  if(_debug){

  Serial.println(F("Initial values:"));
  
  for(int i = 0; i < 2; i++){
    Serial.print (POT_LABELS[i]);
    Serial.println(_tapPointer[i]);
    }
  }
}

float TPL0102:: wiper(uint8_t ch) {

  _selectedChannel = ch;

  return (_tapPointer[ch]) / TAP_NUMBER;

}

/* Switchs ON/OFF the device*/
void TPL0102::switchPot(uint8_t ch, uint8_t st){

  _selectedChannel = ch;

  char ACR_VALUE = 0;
  uint8_t SHDN_INSTR = 0;

  Wire.beginTransmission(address);
  Wire.write(ACR);
  Wire.endTransmission(false);   // --> Thanks to https://forum.arduino.cc/index.php?topic=385377.0
  Wire.requestFrom(address, 1, false);

  while (Wire.available())   // slave may send less than requested
  {
    ACR_VALUE = Wire.receive();    // receive a byte as character

  }

  Wire.endTransmission();

  switch (st) {

    case HIGH:    // Pot is active

      SHDN_INSTR = ACR_VALUE | SHUTDOWN_MASK;

      break;

    case LOW:   // Pot is inactive

      SHDN_INSTR = ACR_VALUE ^ SHUTDOWN_MASK;

      break;

      default:

      SHDN_INSTR =  ACR_VALUE  | 0x00; // Same state

      break;
  }

  Wire.beginTransmission(address);
  Wire.write(ACR);
  Wire.send(SHDN_INSTR);          // sends potentiometer value byte
  Wire.endTransmission(true);     // stop transmitting

}
// Routine for increasing the pot value
void TPL0102::inc(uint8_t ch) {    // return wiper count!

  _selectedChannel = ch;

  if ((_tapPointer[ch] < TAP_NUMBER)) {


    unsigned long _startIncTime = micros();

    _tapPointer[ch]++;

    if (_tapPointer[ch] >= TAP_NUMBER)
      _tapPointer[ch] = TAP_NUMBER;

    dataWrite(ch, _tapPointer[ch]);

    if(_debug){

      Serial.print(F("Current step "));
      Serial.print (POT_LABELS[ch]);
      Serial.println(_tapPointer[ch]);

    }

  
    _incDelay = micros() - _startIncTime;

  }
}

// Routine for decreasing the value
void TPL0102::dec(uint8_t ch) {

  _selectedChannel = ch;

  if ((_tapPointer[ch] > 0)) {

    unsigned long _startDecTime = micros();

    _tapPointer[ch]--;

     if (_tapPointer[ch] <= 0)
      _tapPointer[ch] = 0;

    dataWrite(ch, _tapPointer[ch]);

    if(_debug){

      Serial.print(F("Current step "));  
      Serial.print(POT_LABELS[ch]);
      Serial.println(_tapPointer[ch]);
    }

    _decDelay = micros() - _startDecTime;

  }
}

// Writing data to the user registers
void TPL0102::dataWrite(uint8_t ch, uint8_t val){

  _selectedChannel = ch;

  uint8_t wiperPointer = WRA;

  switch(ch){

    case 0:
      wiperPointer = WRA;
    break;

    case 1:
      wiperPointer = WRB;
    break;
  }

    Wire.beginTransmission(address);
    Wire.write(wiperPointer);
    Wire.send(val);    // sends potentiometer value byte
    Wire.endTransmission(true);     // stop transmitting

}

// Returns how long it took to increase the value
unsigned long TPL0102::incMicros() {

  return _incDelay;

}

// Returns how long it took to decrease the value
unsigned long TPL0102::decMicros() {

  return _decDelay;

}

// Returns how long it took to set the value
unsigned long TPL0102::setMicros() {

  return _setDelay;

}

// Keeps a record of the current tap being addressed
uint8_t TPL0102::taps(uint8_t ch) {

  _selectedChannel = ch;

  return _tapPointer[ch];   // value within [1-64] that points to the taps between resistors [0,63]
}

// Set a desired resistance --> EXTREMELY APPROXIMATE AND THEORETICAL. USE WITH CARE!
uint8_t TPL0102::setValue(uint8_t ch, float desiredR) {

  float distance;
  int tapTarget;

  unsigned long _startSetTime = micros();

  tapTarget = round((desiredR * TAP_NUMBER) / NOMINAL_RESISTANCE);
  distance = abs(_tapPointer[ch] - tapTarget);

  if (_debug){

  Serial.print(F("Distance to target: "));
  Serial.println(distance);

  Serial.print(F("Target tap: "));
  Serial.println(tapTarget);

  }

   if (tapTarget != _tapPointer[ch]) {

    dataWrite(ch, tapTarget);
    _tapPointer[ch] = tapTarget;

  } else {
    // Leave everything where it is
  }

  _setDelay = micros() - _startSetTime;

  return tapTarget;

}

// Select a specific channel and return the value that was selected.
// Dumb-ish but useful(ish)
uint8_t TPL0102::setChannel(uint8_t ch){

  _selectedChannel = ch;

   if(_ledsDefined){
      toggleLED(_selectedChannel);
    }

  return _selectedChannel;

}

// Turn the pot all the way down
void TPL0102::zeroWiper(uint8_t ch) {

  _selectedChannel = ch;

  _tapPointer[ch] = 0;
  dataWrite(ch, _tapPointer[ch]);

}

// Turn the pot all the way up
void TPL0102::maxWiper(uint8_t ch) {

  _selectedChannel = ch;

    _tapPointer[ch] = TAP_NUMBER;
  dataWrite(ch, _tapPointer[ch]);

}

// Get the theoretical current resistance value
float TPL0102::readValue(uint8_t ch) {

  _selectedChannel = ch;

  return (_tapPointer[ch] / TAP_NUMBER) * (NOMINAL_RESISTANCE);

}

// Check the values from the system registers
void TPL0102::readRegistersStatus() {

  for (int pos = 0; pos < 3; pos++) {

    Wire.beginTransmission(address);
    Wire.write(_regs[pos]);
    Wire.endTransmission(false);   // --> Thanks to https://forum.arduino.cc/index.php?topic=385377.0
    Wire.requestFrom(address, 1, false);

    while (Wire.available())   // slave may send less than requested
    {
      char I2CResponse = Wire.receive();    // receive a byte as character

      _initialState[pos] = (uint8_t)I2CResponse;

      if (_debug) {

        Serial.println(F(" ******************** "));
        
        Serial.print(REGISTER_LABELS[pos]);
        Serial.print(I2CResponse, HEX);
        Serial.print(F(" (HEX)"));

        Serial.print(I2CResponse, DEC);
        Serial.print(F(" (DEC)"));
        Serial.println(F(" "));
        Serial.println(F(" ******************** "));
      }
    }

    Wire.endTransmission();

  }

}

// Check the values from the user registers
void TPL0102::readDummyRegStatus() {

  for (int dummyPos = GENERAL_PURPOSE_START; dummyPos <= GENERAL_PURPOSE_END; dummyPos++) {

    Wire.beginTransmission(address);
    Wire.write(dummyPos);
    Wire.endTransmission(false);   // --> Thanks to https://forum.arduino.cc/index.php?topic=385377.0
    Wire.requestFrom(address, 1, false);

    while (Wire.available())   // slave may send less than requested
    {

      char I2CResponse = Wire.receive();    // receive a byte as character

      if (_debug == true) {
        Serial.print(F("Dummy ["));
        Serial.print(dummyPos, HEX);
        Serial.print(F("]: "));

        Serial.print(I2CResponse, HEX);
        Serial.print(F(" (HEX)"));

        Serial.print(I2CResponse, DEC);
        Serial.print(F(" (DEC)"));
        Serial.println(F(" "));
      }
    }

    Wire.endTransmission();
  }
}

// Switch ON/OFF the LEDs attached to the board or
// close to the pots being used. 

void TPL0102::toggleLED(uint8_t ch){

  switch(ch){

    case 0:

            digitalWrite(_boardLEDs[0], HIGH);
            digitalWrite(_boardLEDs[1], LOW);

            _prevLEDsState[0] = HIGH;
            _prevLEDsState[1] = LOW;

  break;

  case 1:

            digitalWrite(_boardLEDs[1], HIGH);
            digitalWrite(_boardLEDs[0], LOW);

            _prevLEDsState[1] = HIGH;
            _prevLEDsState[0] = LOW;

  break;
}
}


