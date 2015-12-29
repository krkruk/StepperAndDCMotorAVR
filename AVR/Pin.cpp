/*
 * Pin.cpp
 *
 *  Created on: Sep 30, 2015
 *      Author: root
 */

#include "Pin.h"

volatile uint8_t tickSecondFlag = 0;
volatile uint8_t tick100MsFlag = 0;
volatile uint8_t tick50MsFlag = 0;
volatile uint64_t __timer1_millis = 0;
static uint8_t __tick50Counter = 0;

Pin::Pin(volatile uint8_t &ddr,  volatile uint8_t &port, uint8_t pin)
	: _port(&port), _pin(pin)
{
	ddr |= 1<<pin;
	this->unset();
}

void Pin::set()
{
	*this->_port |= (1 << this->_pin);
}

void Pin::unset()
{
	*this->_port &= ~(1 << this->_pin);
}

void Pin::toggle()
{
	*this->_port ^= (1 << this->_pin);
}

Pin::Pin(Pin&& pin)
{
	_pin = pin._pin;
	_port = pin._port;
}

Pin::Pin(const Pin& pin)
{
	_pin = pin._pin;
	_port = pin._port;
}

Pin& Pin::operator =(const Pin& p)
{
	_pin = p._pin;
	_port = p._port;
	return *this;
}
uint8_t isElapsed1000Ms()
{
	if(tickSecondFlag)
	{
		tickSecondFlag = 0;
		return 1;
	}
	return 0;
}
uint8_t isElapsed100Ms()
{
	if(tick100MsFlag)
	{
		tick100MsFlag = 0;
		return 1;
	}
	return 0;
}
uint8_t isElapsed50Ms()
{
	if(tick50MsFlag)
	{
		tick50MsFlag = 0;
		return 1;
	}
	return 0;
}
uint64_t millis()
{
	return __timer1_millis;
}
void millis_init()
{
	TCCR1B |= 1<<WGM12;
	TCCR1B |= 1<<CS11 | 1<<CS10;
	OCR1A = 124;	//1000Hz
	TIMSK1 |= 1<<OCIE1A;
	sei();
}

ISR(TIMER1_COMPA_vect)
{
	__timer1_millis++;
	if(__timer1_millis % 50 == 0)
	{
		tick50MsFlag = 1;
		++__tick50Counter;

		if(!(__tick50Counter & 1<<0))
			tick100MsFlag = 1;

		if(__tick50Counter == 20)
		{
			tickSecondFlag = 1;
			__tick50Counter = 0;
		}
	}
}
