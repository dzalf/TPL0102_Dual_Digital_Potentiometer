# TPL0102_Dual_Digital_Potentiometer
Arduino library for the Texas Instruments TPL0102 Two 256-Taps Digital Potentiometers With Non-Volatile Memory


Hiya!

I am thrilled to share my version of the library for controlling the [TPL0102](http://www.ti.com/lit/ds/slis134c/slis134c.pdf) from Texas Instruments. The library was tested with a 100kOhms device.

It is a compact 14-pin dual 256 taps I2C potentiometer with non-volatile registers. Pretty handy for digitally controlled amplifiers, PID controllers and in the words of the manufacturer: suitable for Adjustable Power Supplies, Precision Calibration of Set Point Thresholds, Sensor Trimming and Calibration, Mechanical, Potentiometer Replacement etc.

Please read and play around with the example codes to get a good grasp of the usability of the library.

I am aware that there is still a lot to be polished in the code but I will do my best within the following days.

**UPDATE**
14-May-2020

Just updated this to the latest version: 0.3. 
~~The main intention is to use this library with the breakout/Development Boards soon to be available on [Tindie](https://www.tindie.com/stores/ivorycircuits/)~~. In advance, I can say that there is a tiny mistake on these boards where I accidentally swapped the positions of LED_CHA and LED_CHB. Not a big deal, as it can be solved by software. I will make sure that the next version does not have this issue.


30-June-20

Updated to version 0.4. 
New methods and overloaded constructors are available. It is possible to override the nominal resistance by setting up the pot with the measured value. 
Finished testing this library on the prototypes of the PCBs to be sold soon :wink:


Stay safe

## UPDATE: Feb 2024. 

Unfortunately, the Tindie shop had to be closed. 
Thank you to all of you who purchased something from it. 
We appreciate your support.
P&D


Cheers :beer:

dzalf
