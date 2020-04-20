
/*
    Texas Instruments TPL0102 256 taps Dual Digital Potentiometer
    I2C interface
    Author: Daniel Melendrez
    Date: March 2020 (COVID-19 Vibes)
    Version: 0.1

*/

#ifndef TPL0102_h
#define TPL0102_h

#include "Arduino.h"
#include <Wire.h>
#include <avr/pgmspace.h>

#define TAP_NUMBER 255.0           // Total taps, 255 resistors with non-volatile memory. Wiper values are from 0x00 to 0x3F
#define DEFAULT_TAP_COUNT 128.0     // Half way resistance
#define NOMINAL_RESISTANCE 97270    // 100000 theoretical --> Real value measured using maxPot method
#define WIPER_RESISTANCE 39.5   // 75 typical (According to datasheet)
#define FACTORY_WIPER_POSITION 128.0  //0x80
// Registers
#define IVRA 0x00 // Initial Value Register por Potentiomenter A: Stores wiper positiom for Pot A
#define IVRB 0x01 // Initial Value Register por Potentiomenter B: Stores wiper positiom for Pot B
#define WRA 0x00  // Wiper Resistance Register for Potentiometer A: IVRA loads value to WRA to determine wiper position
#define WRB 0x01  // Wiper Resistance Register for Potentiometer B: IVRB loads value to WRB to determine wiper position
#define ACR 0x10  // Access control register: Volatile register to control register access, determine shut-down mode, and read non-volatile write operations
// B7: VOL[0/1] (W/R) --> 0: Non-volatile registers acessible; 1: Volatile registers accesible
// B6: ~SHDN[0/1](W/R) --> 0: Shutdown enabled (both pots); 1: Shutdown disabled
// B5: WIP[0/1](R) --> Non-volatile operation not in progress; 1: Non-volatile operation in progress (not possible to write to WR or ACR while WIP = 1)
// B4-B0: 0
#define SHUTDOWN_MASK 0x40 // Turn off bit 6
#define VOLATILE_REG_ACCESSIBLE 0xC0
#define NON_VOLATILE_REG_ACCESSIBLE 0x40   // Bitmask: 0x80  (|= (OR to set) or ^= (XOR to remove))
#define GENERAL_PURPOSE_START 2//0x02
#define GENERAL_PURPOSE_END   14 //0x0E

#define CHA 0
#define CHB 1


class TPL0102 {

  public:

    // Constructors:
    TPL0102(void);
    TPL0102(bool dbg);

    //Constants

     // constants

   /* Declared here thanks to the GREAT help from my buddies on: https://stackoverflow.com/questions/11072244/c-multiple-definitions-of-a-variable*/
    const char LBL_POT_A[8] PROGMEM = "POT A: ";
    const char LBL_POT_B[8] PROGMEM = "POT B: ";

    const char LBL_REG_IVRA[7] PROGMEM = "IVRA: ";
    const char LBL_REG_IVRB[7] PROGMEM = "IVRB: ";
    const char LBL_REG_ACR[7] PROGMEM = "ACR_: ";

    const char *const POT_LABELS[2] PROGMEM = {LBL_POT_A,LBL_POT_B};
    const char *const REGISTER_LABELS[3] PROGMEM = {LBL_REG_IVRA, LBL_REG_IVRB, LBL_REG_ACR};

    const float nominal = NOMINAL_RESISTANCE;

    // Deconstructor
    //~TPL0102(void);

    // Variables
    
    uint16_t address;
    // Methods:
    void begin(uint16_t addr);
    uint8_t taps(uint8_t chan);
    float wiper(uint8_t chan);
    void inc(uint8_t chan);
    void dec(uint8_t chan);
    void zeroWiper(uint8_t chan);
    void maxWiper(uint8_t chan);
    float readValue(uint8_t chan);
    uint8_t setValue(uint8_t chan, float val);
    unsigned long incMicros(void);
    unsigned long decMicros(void);
    unsigned long setMicros(void);
    void switchPot(uint8_t chan, uint8_t state);

  private:

    // Chars
    
    uint8_t _regs[3] = {IVRA, IVRB, ACR};
    uint8_t _tapPointer[2];
    uint8_t _initialState[3]; // Holds the initial values from Registers: IVRA([0]), IVRB([1]) and ACR([2]).
    unsigned long _incDelay;
    unsigned long _decDelay;
    unsigned long _setDelay;
    bool _debug;
   

    void dataWrite(uint8_t ch, uint8_t val);
    void readRegistersStatus(void);
    void readDummyRegStatus(void);

};

#endif
