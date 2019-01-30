//reference  https://s2jp.com/2013/01/tiny10-i2c-master/

#ifndef _S2_TINY841_SOFTI2C_H_
#define _S2_TINY841_SOFTI2C_H_

#include <avr/io.h>
#include <util/delay.h>


#define CLKWaitTime 3 // confirmed Arduino Communication 512
#define I2C_SDA_PIN           PORTA3 //IF pwm use 3, PORTA2
#define I2C_SCL_PIN           PORTA0 // PA5 can run .

#define I2C_SDA_PIN_BV        _BV(I2C_SDA_PIN)
#define I2C_SCL_PIN_BV        _BV(I2C_SCL_PIN)

#define _I2C_SDA_HIGH        (PORTA |= I2C_SDA_PIN_BV)
#define _I2C_SDA_LOW        (PORTA &= ~I2C_SDA_PIN_BV)

#define _I2C_SCL_HIGH        (PORTA |= I2C_SCL_PIN_BV)
#define _I2C_SCL_LOW        (PORTA &= ~I2C_SCL_PIN_BV)


void I2C_clkWait()
{
	/*
#if F_CPU == 1000000
	__asm__ __volatile__("nop");
#elif F_CPU == 1200000
	__asm__ __volatile__("nop");
#elif F_CPU == 2000000
	__asm__ __volatile__("nop");
	__asm__ __volatile__("nop");
	__asm__ __volatile__("nop");
	__asm__ __volatile__("nop");
	__asm__ __volatile__("nop");
#elif F_CPU == 2400000
	__asm__ __volatile__("nop");
	__asm__ __volatile__("nop");
	__asm__ __volatile__("nop");
	__asm__ __volatile__("nop");
#elif F_CPU == 4000000
	_delay_us(2);
#elif F_CPU == 4800000
	_delay_us(2);
#else
	_delay_us(4);
#endif
*/
	_delay_us(CLKWaitTime);
}


void I2C_clkWait2()
{
#if F_CPU == 1000000
#elif F_CPU == 1200000
#elif F_CPU == 2000000
#elif F_CPU == 2400000
#elif F_CPU == 4000000
	_delay_us(2);
#elif F_CPU == 4800000
	_delay_us(2);
#else
	_delay_us(3);
#endif
}

inline static void I2C_init()
{

	//pullup active
	//http://www.atmel.com/Images/Atmel-8495-8-bit-AVR-Microcontrollers-ATtiny441-ATtiny841_Datasheet.pdf
	//57p
	/*
    PUEA = 0b00101000;//40;  //00101000  PA3 And PA5 ON (start is PA0)	
	PUEA  |= I2C_SDA_PIN_BV;
	PUEA  |= I2C_SCL_PIN_BV;
	*/

	DDRA |= I2C_SDA_PIN_BV;
	DDRA |= I2C_SCL_PIN_BV;
	_I2C_SCL_HIGH;
	_I2C_SDA_HIGH;
}

inline static void I2C_start()
{
	_I2C_SDA_HIGH;
	I2C_clkWait();
	_I2C_SCL_HIGH;

	I2C_clkWait();
	_I2C_SDA_LOW;
	I2C_clkWait();
	_I2C_SCL_LOW;
}

inline static void I2C_stop()
{
	_I2C_SDA_LOW;
	I2C_clkWait();
	_I2C_SCL_HIGH;
	I2C_clkWait();
	_I2C_SDA_HIGH;
	I2C_clkWait();
	//I2C_clkWait();
}

uint8_t I2C_write(uint8_t data)
{
	uint8_t temp;
	for (temp = 0x80; temp; temp >>= 1)
	{
		if (data & temp)        _I2C_SDA_HIGH;
		else 
		{
			_I2C_SDA_LOW;
		}
		I2C_clkWait();
		//I2C_clkWait2();
		_I2C_SCL_HIGH;
		I2C_clkWait();
		_I2C_SCL_LOW;
		
	}

	DDRA &= ~I2C_SDA_PIN_BV;

	I2C_clkWait();
	temp = PINA;
	_I2C_SCL_HIGH;
	I2C_clkWait();

	_I2C_SCL_LOW;

	DDRA |= I2C_SDA_PIN_BV;

	if (temp & I2C_SDA_PIN_BV)
	{
		return 1;
	}
	return 0;
}

uint8_t I2C_read(uint8_t ACK)
{
	uint8_t data = 0;
	uint8_t temp;

	DDRA &= ~I2C_SDA_PIN_BV;

	for (temp = 0x80; temp; temp >>= 1)
	{
		I2C_clkWait();
		if (PINA & I2C_SDA_PIN_BV)        data |= temp;
		_I2C_SCL_HIGH;

		I2C_clkWait();
		_I2C_SCL_LOW;
	}

	DDRA |= I2C_SDA_PIN_BV;

	if (ACK)    _I2C_SDA_LOW;
	else    _I2C_SDA_HIGH;

	I2C_clkWait();
	_I2C_SCL_HIGH;
	I2C_clkWait();
	_I2C_SCL_LOW;


	//I2C_clkWait();
	return data;
}

#endif