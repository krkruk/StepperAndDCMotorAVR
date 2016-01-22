/*
 * MainApplication.cpp
 *
 *  Created on: Oct 17, 2015
 *      Author: root
 */

#include "MainApplication.h"
MainApplication app;


MainApplication::MainApplication() :
	stepperLeft(
			Pin(DDRB, PORTB, PB1),
			Pin(DDRB, PORTB, PB2),
			Pin(DDRB, PORTB, PB3),
			Pin(DDRB, PORTB, PB4)),
	stepperRight(
			Pin(DDRC, PORTC, PC0),
			Pin(DDRC, PORTC, PC1),
			Pin(DDRC, PORTC, PC2),
			Pin(DDRC, PORTC, PC3)),
	motorLeft(
			Pin(DDRD, PORTD, PD2),
			Pin(DDRD, PORTD, PD3),
			&LEFT_PWM),
	motorRight(
			Pin(DDRC, PORTC, PC4),
			Pin(DDRC, PORTC, PC5),
			&RIGHT_PWM),
	builtinLed(
			Pin(DDRB, PORTB, PB5))
{
	wdt_disable();
	builtinLed.set();
	DCMotor::configurePWM();
	millis_init();
	stepperRight.setTimeInterval(10);
	setUartWatchdog();
}
void MainApplication::exec1000Ms()
{
	static int i = 0;
	if(isElapsed1000Ms())
	{
		builtinLed.toggle();
		if(++i % 10 == 0)
			exec100Ms();
	}
}
void MainApplication::exec100Ms()
{
	if(isElapsed100Ms())
	{
		/*
		 * s1 - stepper 1, position in steps
		 * s2 - stepper 2, position in steps
		 * sr1 - stepper 1 reset position [0..1],
		 * sr2 - stepper 2 reset position [0..1],
		 * sm1 - stepper 1, mode: 0 - full step, 1 - half step
		 * sm2 - stepper 2, mode: 0 - full step, 1 - half step
		 * st1 - stepper1, time interval in ms
		 * st2 - stepper2, time interval in ms
		 * p1 - dc motor 1, pwm [0..100]
		 * d1 - dc motor 1, direction [0..1]; 0 - clockwise, 1, counterclockwise
		 * p2 - dc motor 2, pwm [0..100]
		 * d2 - dc motor 2, direction [0..1]
		 * *1 - left
		 * *2 - right
		 */
		auto s1 = stepperLeft.currentStepperPosition();
		auto s2 = stepperRight.currentStepperPosition();
//		auto sm1 = stepperLeft.stepsAvailable() / 4 - 1;
//		auto sm2 = stepperRight.stepsAvailable() / 4 - 1;
//		auto st1 = stepperLeft.getTimeInterval();
//		auto st2 = stepperRight.getTimeInterval();
		uint8_t p1 = motorLeft.getPWM();
		uint8_t p2 = motorRight.getPWM();
		auto d1 = motorLeft.getDirection();
		auto d2 = motorRight.getDirection();

		constexpr uint8_t BUFFER_SIZE = 200;
		char buffer[BUFFER_SIZE];
		memset(buffer, '\0', BUFFER_SIZE);

		sprintf(buffer, "{\"s1\": %ld, \"s2\": %ld, \"p1\": %d, \"p2\": %d, \"d1\": %d, \"d2\": %d}\r\n",
				s1, s2, p1, p2, d1, d2);
		uart.send(buffer);
	}
}
void MainApplication::exec50Ms()
{
	if(isElapsed50Ms())
	{
		parseUART();
	}
}
void MainApplication::readToBuffer(const char* jsonString, jsmntok_t* t,
		int* i)
{
	memset(inputBuffer, '\0', SIZE);
	unsigned int len = t[(*i)+1].end-t[(*i)+1].start;
	const char *string = &jsonString[ t[(*i)+1].start ];
	strncpy(inputBuffer, string, len);
	++(*i);
}

void MainApplication::parseStepper1Position(const char* jsonString,
		jsmntok_t* t, int* i)
{
	if (jsoneq(jsonString, &t[*i], "s1"))
	{
		readToBuffer(jsonString, t, i);
		auto s1 = atol(inputBuffer);
		stepperLeft.step(s1);
	}
}

void MainApplication::parseStepper2Position(const char* jsonString,
		jsmntok_t* t, int* i)
{
	if (jsoneq(jsonString, &t[*i], "s2"))
	{
		readToBuffer(jsonString, t, i);
		auto s2 = atol(inputBuffer);
		stepperRight.step(s2);
	}
}

void MainApplication::parseStepper1Reset(const char* jsonString, jsmntok_t* t,
		int* i)
{
	if (jsoneq(jsonString, &t[*i], "sr1"))
	{
		readToBuffer(jsonString, t, i);
//		auto sr1 = atoi(inputBuffer);
	}
}

void MainApplication::parseStepper2Reset(const char* jsonString, jsmntok_t* t,
		int* i)
{
	if (jsoneq(jsonString, &t[*i], "sr2"))
	{
		readToBuffer(jsonString, t, i);
//		auto sr2 = atoi(inputBuffer);
	}
}

void MainApplication::parseStepper1Mode(const char* jsonString, jsmntok_t* t,
		int* i)
{
	if (jsoneq(jsonString, &t[*i], "sm1"))
	{
		readToBuffer(jsonString, t, i);
		auto sm1 = atoi(inputBuffer);
		switch(sm1)
		{
		case 0: stepperLeft.setAlgorithm(new StepperMotorFullStep); break;
		case 1: stepperLeft.setAlgorithm(new StepperMotorHalfStep); break;
		default: uart.send("SM1 parsing error"); break;
		}
	}
}

void MainApplication::parseStepper2Mode(const char* jsonString, jsmntok_t* t,
		int* i)
{
	if (jsoneq(jsonString, &t[*i], "sm2"))
	{
		readToBuffer(jsonString, t, i);
		auto sm2 = atoi(inputBuffer);
		switch(sm2)
		{
		case 0: stepperRight.setAlgorithm(new StepperMotorFullStep); break;
		case 1: stepperRight.setAlgorithm(new StepperMotorHalfStep); break;
		default: uart.send("SM2 parsing error"); break;
		}
	}
}

void MainApplication::parseStepper1TimeInterval(const char* jsonString,
		jsmntok_t* t, int* i)
{
	if (jsoneq(jsonString, &t[*i], "st1"))
	{
		readToBuffer(jsonString, t, i);
		auto st1 = atol(inputBuffer);
		stepperLeft.setTimeInterval(st1);
	}
}

void MainApplication::parseStepper2TimeInterval(const char* jsonString,
		jsmntok_t* t, int* i)
{
	if (jsoneq(jsonString, &t[*i], "st2"))
	{
		readToBuffer(jsonString, t, i);
		auto st2 = atol(inputBuffer);
		stepperRight.setTimeInterval(st2);
	}
}

void MainApplication::parseMotor1PWM(const char* jsonString, jsmntok_t* t,
		int* i)
{
	if (jsoneq(jsonString, &t[*i], "p1"))
	{
		readToBuffer(jsonString, t, i);
		auto p1 = atoi(inputBuffer);
		if(p1 == 0)
		{
			motorLeft.stop();
			return;
		}
		motorLeft.setPWM(p1);
	}
}

void MainApplication::parseMotor2PWM(const char* jsonString, jsmntok_t* t,
		int* i)
{
	if (jsoneq(jsonString, &t[*i], "p2"))
	{
		readToBuffer(jsonString, t, i);
		auto p2 = atoi(inputBuffer);
		if(p2 == 0)
		{
			motorRight.stop();
			return;
		}
		motorRight.setPWM(p2);
	}
}

void MainApplication::parseMotor1Direction(const char* jsonString, jsmntok_t* t,
		int* i)
{
	if (jsoneq(jsonString, &t[*i], "d1"))
	{
		readToBuffer(jsonString, t, i);
		auto d1 = atoi(inputBuffer);
		switch(d1)
		{
		case DCMotor::Clockwise: motorLeft.rotateClockwise(); break;
		case DCMotor::Counterclockwise: motorLeft.rotateCounterclockwise(); break;
		default: motorLeft.stop(); break;
		}
	}
}

void MainApplication::parseMotor2Direction(const char* jsonString, jsmntok_t* t,
		int* i)
{
	if (jsoneq(jsonString, &t[*i], "d2"))
	{
		readToBuffer(jsonString, t, i);
		auto d2 = atoi(inputBuffer);
		switch(d2)
		{
		case DCMotor::Clockwise: motorRight.rotateClockwise(); break;
		case DCMotor::Counterclockwise: motorRight.rotateCounterclockwise(); break;
		default: motorRight.stop(); break;
		}
	}
}

void MainApplication::parseUART()
{
	constexpr uint8_t JSON_VALUE_BUFFER_SIZE = 100;
	constexpr uint8_t TOKEN_SIZE = 30;

	char jsonString[JSON_VALUE_BUFFER_SIZE];
	memset(jsonString, '\0', JSON_VALUE_BUFFER_SIZE);

	jsmn_parser p;
	jsmntok_t t[TOKEN_SIZE];

	if(uart.isReadyReadLine())
	{
		jsmn_init(&p);
		uart.readLine(jsonString);
		int r = jsmn_parse(&p, jsonString, strlen(jsonString), t, sizeof(t)/sizeof(t[0]));

		if (r < 0) {
			uart.send("Error 1"); return;
		}

		if (r < 1 || t[0].type != JSMN_OBJECT) {
			uart.send("Error 2"); return;
		}

		for (int i = 1; i < r; i++) {
			parseStepper1Position(jsonString, t, &i);
			parseStepper2Position(jsonString, t, &i);
			parseStepper1Reset(jsonString, t, &i);
			parseStepper2Reset(jsonString, t, &i);
			parseStepper1Mode(jsonString, t, &i);
			parseStepper2Mode(jsonString, t, &i);
			parseStepper1TimeInterval(jsonString, t, &i);
			parseStepper2TimeInterval(jsonString, t, &i);
			parseMotor1PWM(jsonString, t, &i);
			parseMotor2PWM(jsonString, t, &i);
			parseMotor1Direction(jsonString, t, &i);
			parseMotor2Direction(jsonString, t, &i);
		}
	}
}

void MainApplication::watchdogRoutine()
{
	motorLeft.stop();
	motorRight.stop();
}

uint8_t MainApplication::exec()
{
	while(1)
	{
		exec1000Ms();
		exec100Ms();
		exec50Ms();

		stepperLeft.stepUntil();
		stepperRight.stepUntil();
	}
	return 1;
}

ISR(WDT_vect)
{
	app.watchdogRoutine();
}
