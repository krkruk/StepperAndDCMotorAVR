/*
 * Pin.h
 *
 *  Created on: Sep 30, 2015
 *      Author: root
 */

#ifndef PIN_H_
#define PIN_H_
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

class Pin {
	volatile uint8_t *_port {0};
	volatile uint8_t _pin {0};
public:
	Pin(volatile uint8_t &ddr, volatile uint8_t &port, uint8_t pin);
	Pin(Pin &&pin);
	Pin(const Pin &pin);

	void set();
	void unset();
	void toggle();

	Pin &operator=(const Pin &p);
};

extern volatile uint8_t tickSecondFlag;
extern volatile uint8_t tick100MsFlag;
extern volatile uint8_t tick50MsFlag;
extern volatile uint64_t __timer1_millis;

uint8_t isElapsed1000Ms();
uint8_t isElapsed100Ms();
uint8_t isElapsed50Ms();

uint64_t millis();
void millis_init();

#endif /* PIN_H_ */
