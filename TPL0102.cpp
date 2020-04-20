
/*
    Microchip 64 taps Single Digital Potentiometer
    Simple two-wire UP/DOWN interface
    Author: Daniel Melendrez
    Date: March 2020 (COVID-19 Vibes)
    Version: 0.1

*/

#include "TPL0102.h"

 

// Constructors

TPL0102::TPL0102() {

 _debug = false;

}

TPL0102::TPL0102(bool DEBUG) {

  _debug = DEBUG;

}

// Methods

void TPL0102::begin(uint16_t addr) {
  
  Wire.begin();

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

  return (_tapPointer[ch]) / TAP_NUMBER;

}

/* Switchs ON/OFF the device*/

void TPL0102::switchPot(uint8_t ch, uint8_t st){

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
  Wire.send(SHDN_INSTR);    // sends potentiometer value byte

  Wire.endTransmission(true);     // stop transmitting

}

void TPL0102::inc(uint8_t ch) {    // return wiper count!

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


void TPL0102::dec(uint8_t ch) {

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

void TPL0102::dataWrite(uint8_t ch, uint8_t val){

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

unsigned long TPL0102::incMicros() {

  return _incDelay;

}

unsigned long TPL0102::decMicros() {

  return _decDelay;

}

unsigned long TPL0102::setMicros() {

  return _setDelay;

}

uint8_t TPL0102::taps(uint8_t ch) {

  return _tapPointer[ch];   // value within [1-64] that points to the taps between resistors [0,63]
}

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

void TPL0102::zeroWiper(uint8_t ch) {

  _tapPointer[ch] = 0;
  dataWrite(ch, _tapPointer[ch]);

}

void TPL0102::maxWiper(uint8_t ch) {

    _tapPointer[ch] = TAP_NUMBER;
  dataWrite(ch, _tapPointer[ch]);

}

float TPL0102::readValue(uint8_t ch) {

  return (_tapPointer[ch] / TAP_NUMBER) * (NOMINAL_RESISTANCE);

}

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
