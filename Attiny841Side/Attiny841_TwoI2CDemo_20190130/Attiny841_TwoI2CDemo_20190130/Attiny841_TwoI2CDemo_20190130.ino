/*
 * Attiny841_TwoI2CDemo_20190130.ino
 *
 * Created: 1/30/2019 12:19:33 PM
 * Author: yt
 */ 

//fuse change  LOW 0x42 -> 0xE2 for high speed process
//core https://github.com/SpenceKonde/arduino-tiny-841
#include <MySoftI2CM/MySoftI2CM.h>  //I2C Master, yt made it. Please confirm pin setting in MySoftI2CM.h
#include <WireS/WireS.h> //I2C Slave  https://github.com/nadavmatalon/WireS

//########################################### finish #############################################################


#define I2C_NODE_SLAVE_ADDRESS 101 //start 101

//reference https://github.com/sparkfun/SparkFun_MPL3115A2_Breakout_Arduino_Library/blob/master/examples/SparkFunMPL3115A2AdvancedExample/SparkFunMPL3115A2AdvancedExample.ino
#define STATUS     0x00
#define OUT_P_MSB  0x01
#define OUT_P_CSB  0x02
#define OUT_P_LSB  0x03
#define OUT_T_MSB  0x04
#define OUT_T_LSB  0x05
#define DR_STATUS  0x06
#define OUT_P_DELTA_MSB  0x07
#define OUT_P_DELTA_CSB  0x08
#define OUT_P_DELTA_LSB  0x09
#define OUT_T_DELTA_MSB  0x0A
#define OUT_T_DELTA_LSB  0x0B
#define WHO_AM_I   0x0C
#define F_STATUS   0x0D
#define F_DATA     0x0E
#define F_SETUP    0x0F
#define TIME_DLY   0x10
#define SYSMOD     0x11
#define INT_SOURCE 0x12
#define PT_DATA_CFG 0x13
#define BAR_IN_MSB 0x14
#define BAR_IN_LSB 0x15
#define P_TGT_MSB  0x16
#define P_TGT_LSB  0x17
#define T_TGT      0x18
#define P_WND_MSB  0x19
#define P_WND_LSB  0x1A
#define T_WND      0x1B
#define P_MIN_MSB  0x1C
#define P_MIN_CSB  0x1D
#define P_MIN_LSB  0x1E
#define T_MIN_MSB  0x1F
#define T_MIN_LSB  0x20
#define P_MAX_MSB  0x21
#define P_MAX_CSB  0x22
#define P_MAX_LSB  0x23
#define T_MAX_MSB  0x24
#define T_MAX_LSB  0x25
#define CTRL_REG1  0x26
#define CTRL_REG2  0x27
#define CTRL_REG3  0x28
#define CTRL_REG4  0x29
#define CTRL_REG5  0x2A
#define OFF_P      0x2B
#define OFF_T      0x2C
#define OFF_H      0x2D

#define MPL3115A2_ADDRESS 0x60 // 7-bit I2C address
//long startTime;
float altitude;	  //Serial.print("Altitude(m):");   // ...
int mask;

union t_tag{
	byte b[4];
	float f;
}pressure1, temperature1;
//Serial.print("Altitude(kPa):"); // from 20 to 110
//Serial.print(" Temp(c):");



void setup()
{
	
	
	I2C_init();
	IIC_Read(WHO_AM_I);
	// Configure the sensor
	//setModeAltimeter(); // Measure altitude above sea level in meters
	setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa

	setOversampleRate(7); //before 7 //Set Oversample to the recommended 128
	enableEventFlags(); // Enable all three pressure and temp event flags

	//altitude = readAltitude();
	pressure1.f = readPressure();
	temperature1.f = readTemp();
	mask = 0x3;

	TinyWireS.begin(I2C_NODE_SLAVE_ADDRESS);
	TinyWireS.onRequest(request);
}

void loop()
{
	if((mask & 0x01)>0){
		pressure1.f = readPressure();	  //Serial.print("Altitude(kPa):"); // from 20 to 110
	}
	if((mask & 0x02)>0){
		temperature1.f = readTemp();	  //Serial.print(" Temp(c):");
	}

	delay(10);
	
}

void request(){
	if((mask & 0x01)>0){
		TinyWireS.write(pressure1.b[0]);// 4 bytes, must read 4 times.
		TinyWireS.write(pressure1.b[1]);// 4 bytes, must read 4 times.
		TinyWireS.write(pressure1.b[2]);// 4 bytes, must read 4 times.
		TinyWireS.write(pressure1.b[3]);// 4 bytes, must read 4 times.
	}
	if((mask & 0x02)>0){
		TinyWireS.write(temperature1.b[0]);// 4 bytes, must read 4 times.
		TinyWireS.write(temperature1.b[1]);// 4 bytes, must read 4 times.
		TinyWireS.write(temperature1.b[2]);// 4 bytes, must read 4 times.
		TinyWireS.write(temperature1.b[3]);// 4 bytes, must read 4 times.
	}
}



//Reads the current pressure in Pa
//Unit must be set in barometric pressure mode
float readPressure()
{
	toggleOneShot(); //Toggle the OST bit causing the sensor to immediately take another reading

	//Wait for PDR bit, indicates we have new pressure data
	int counter = 0;
	while( (IIC_Read(STATUS) & (1<<2)) == 0)
	{
		if(++counter > 100) return(-999); //Error out
		delay(10);
	}

	// Read pressure registers
	

	I2C_start();
	I2C_write(MPL3115A2_ADDRESS << 1 | 0); //set up write
	I2C_clkWait();
	I2C_write(OUT_P_MSB); //write address (ref address)
	I2C_clkWait();
	I2C_start(); //Wire.endTransmission(false);
	I2C_write(MPL3115A2_ADDRESS << 1 | 1); //set up read
	I2C_clkWait();
	byte msb, csb, lsb;
	for(int i = 0; i<3; i++) I2C_clkWait();
	msb = I2C_read(1);
	csb = I2C_read(1);
	lsb = I2C_read(0);
	I2C_stop();
	
	toggleOneShot(); //Toggle the OST bit causing the sensor to immediately take another reading

	// Pressure comes back as a left shifted 20 bit number
	long pressure_whole = (long)msb<<16 | (long)csb<<8 | (long)lsb;
	pressure_whole >>= 6; //Pressure is an 18 bit number with 2 bits of decimal. Get rid of decimal portion.

	lsb &= 0b00110000; //Bits 5/4 represent the fractional component
	lsb >>= 4; //Get it right aligned
	float pressure_decimal = (float)lsb/4.0; //Turn it into fraction

	float pressure = (float)pressure_whole + pressure_decimal;

	return(pressure);
}


float readTemp()
{
	toggleOneShot(); //Toggle the OST bit causing the sensor to immediately take another reading

	//Wait for TDR bit, indicates we have new temp data
	int counter = 0;
	while( (IIC_Read(STATUS) & (1<<1)) == 0)
	{
		if(++counter > 100) return(-999); //Error out
		delay(10);
	}

	I2C_start();
	I2C_write(MPL3115A2_ADDRESS << 1 | 0); //set up write
	I2C_clkWait();
	//I2C_write(OUT_P_MSB); //write address (ref address)
	I2C_write(OUT_T_MSB); //write address (ref address)
	I2C_clkWait();
	I2C_start(); //Wire.endTransmission(false);
	I2C_write(MPL3115A2_ADDRESS << 1 | 1); //set up read
	I2C_clkWait();
	byte msb, lsb;
	for(int i = 0; i<3; i++) I2C_clkWait();
	msb = I2C_read(1);
	lsb = I2C_read(0);
	I2C_stop();
	
	// The least significant bytes l_altitude and l_temp are 4-bit,
	// fractional values, so you must cast the calulation in (float),
	// shift the value over 4 spots to the right and divide by 16 (since
	// there are 16 values in 4-bits).
	float templsb = (lsb>>4)/16.0; //temp, fraction of a degree

	float temperature = (float)(msb + templsb);

	return(temperature);
}

//Sets the mode to Barometer
//CTRL_REG1, ALT bit
void setModeBarometer()
{
	byte tempSetting = IIC_Read(CTRL_REG1); //Read current settings
	tempSetting &= ~(1<<7); //Clear ALT bit
	IIC_Write(CTRL_REG1, tempSetting);
}




//Call with a rate from 0 to 7. See page 33 for table of ratios.
//Sets the over sample rate. Datasheet calls for 128 but you can set it
//from 1 to 128 samples. The higher the oversample rate the greater
//the time between data samples.
void setOversampleRate(byte sampleRate)
{
	if(sampleRate > 7) sampleRate = 7; //OS cannot be larger than 0b.0111
	sampleRate <<= 3; //Align it for the CTRL_REG1 register

	byte tempSetting = IIC_Read(CTRL_REG1); //Read current settings
	tempSetting &= 0b11000111; //Clear out old OS bits
	tempSetting |= sampleRate; //Mask in new OS bits
	IIC_Write(CTRL_REG1, tempSetting);
}

//Clears then sets the OST bit which causes the sensor to immediately take another reading
//Needed to sample faster than 1Hz
void toggleOneShot(void)
{
	byte tempSetting = IIC_Read(CTRL_REG1); //Read current settings
	tempSetting &= ~(1<<1); //Clear OST bit
	IIC_Write(CTRL_REG1, tempSetting);

	tempSetting = IIC_Read(CTRL_REG1); //Read current settings to be safe
	tempSetting |= (1<<1); //Set OST bit
	IIC_Write(CTRL_REG1, tempSetting);
}

//Enables the pressure and temp measurement event flags so that we can
//test against them. This is recommended in datasheet during setup.
void enableEventFlags()
{
	IIC_Write(PT_DATA_CFG, 0x07); // Enable all three pressure and temp event flags
}


byte IIC_Read(byte regAddr)
{
	I2C_start();
	I2C_write(MPL3115A2_ADDRESS << 1 | 0); //set up write
	I2C_clkWait();
	I2C_write(regAddr); //write address (ref address)
	I2C_clkWait();
	I2C_start(); //Wire.endTransmission(false);
	I2C_write(MPL3115A2_ADDRESS << 1 | 1); //set up read
	I2C_clkWait();
	byte sr = I2C_read(0);
	I2C_stop();
	return sr;
}

void IIC_Write(byte regAddr, byte value)
{
	I2C_start();
	I2C_write(MPL3115A2_ADDRESS << 1 | 0);
	I2C_clkWait();
	I2C_write(regAddr); //write address (ref address)
	I2C_clkWait();
	I2C_write(value);
	I2C_stop();
}