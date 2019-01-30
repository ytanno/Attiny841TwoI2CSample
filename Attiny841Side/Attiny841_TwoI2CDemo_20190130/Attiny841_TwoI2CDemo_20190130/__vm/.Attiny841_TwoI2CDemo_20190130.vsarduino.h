/* 
	Editor: http://www.visualmicro.com
			visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
			the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
			all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
			note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: ATtiny441/841 (Optiboot), Platform=avr, Package=ATTinyCore
*/

#define ARDUINO 106012
#define ARDUINO_MAIN
#define __AVR__
#define F_CPU {build.f_cpu}
#define ARDUINO 106012
#define ARDUINO_AVR_ATTINYX41
#define ARDUINO_ARCH_AVR
//
//
void request();
float readPressure();
float readTemp();
void setModeBarometer();
void setOversampleRate(byte sampleRate);
void toggleOneShot(void);
void enableEventFlags();
byte IIC_Read(byte regAddr);
void IIC_Write(byte regAddr, byte value);

#include "arduino.h"
#include "Attiny841_TwoI2CDemo_20190130.ino"
