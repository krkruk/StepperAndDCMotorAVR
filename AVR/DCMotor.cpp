/*
 * DCMotor.cpp
 *
 *  Created on: Dec 28, 2015
 *      Author: root
 */

#include "DCMotor.h"
void DCMotor::configurePWM()
{
	//pin PD5 - OC0B
	//pin PD6 - OC0A
	TCCR0A |= 1<<COM0A0 | 1<<COM0A1;	//PD6 PWM in non-inverted mode
	TCCR0A |= 1<<COM0B0 | 1<<COM0B1;	//PD5 PWM in non-inverted mode

	//fast pwm mode
	TCCR0A |= 1<<WGM01 | 1<<WGM00;


	//prescaler 1024
	TCCR0B |= 1<<CS02 | 1<<CS00;
	DDRD |= 1<<PD5;
	DDRD |= 1<<PD6;
}

DCMotor::DCMotor(const Pin &leftRelay, const Pin &rightRelay, volatile uint8_t *pwmReg)
	: leftRelay(leftRelay), rightRelay(rightRelay), pwm(pwmReg)
{
	stop();
}

DCMotor::DCMotor(Pin&& leftRelay, Pin&& rightRelay, volatile uint8_t *pwmReg)
	: leftRelay(leftRelay), rightRelay(rightRelay), pwm(pwmReg)
{
	stop();
}

void DCMotor::swapDirections()
{
	auto temp = leftRelay;
	leftRelay = rightRelay;
	rightRelay = temp;
}

void DCMotor::rotateClockwise()
{
	direction = Clockwise;
	auto tempPWM = getPWM();
	setPWM(PWM_OFF);
		leftRelay.unset();
		rightRelay.set();
	setPWM(tempPWM);
}

void DCMotor::rotateCounterclockwise()
{
	direction = Counterclockwise;
	auto tempPWM = getPWM();
		setPWM(PWM_OFF);
	leftRelay.set();
	rightRelay.unset();
	setPWM(tempPWM);
}

void DCMotor::stop(uint8_t state)
{
	setPWM(PWM_OFF);
	if(state) {
		leftRelay.set();
		rightRelay.set();
	}
	else {
		leftRelay.unset();
		rightRelay.unset();
	}
}

void DCMotor::setPWM(uint8_t pwm)
{
	(*this->pwm) = pwm;
}


