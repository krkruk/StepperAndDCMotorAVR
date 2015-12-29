/*
 * DCMotor.h
 *
 *  Created on: Dec 28, 2015
 *      Author: root
 */

#ifndef DCMOTOR_H_
#define DCMOTOR_H_
#include "Pin.h"
#include <avr/interrupt.h>
#define LEFT_PWM OCR0A
#define RIGHT_PWM OCR0B
#define PWM_OFF 0

class DCMotor
{
	Pin leftRelay;
	Pin rightRelay;
	volatile uint8_t *pwm {nullptr};
	uint8_t direction {Clockwise};
public:
	DCMotor(const Pin &leftRelay, const Pin &rightRelay, volatile uint8_t *pwmReg);
	DCMotor(Pin &&leftRelay, Pin &&rightRelay, volatile uint8_t *pwmReg);
	enum DIRECTION { Clockwise, Counterclockwise };

	void swapDirections();

	void rotateClockwise();
	void rotateCounterclockwise();
	void stop(uint8_t state = 1);

	void setPWM(uint8_t pwm);
	uint8_t getPWM() const { return *pwm; }

	uint8_t getDirection() const { return direction; }

	static void configurePWM();
};

#endif /* DCMOTOR_H_ */
