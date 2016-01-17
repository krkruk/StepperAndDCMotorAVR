/*
 * MainApplication.h
 *
 *  Created on: Oct 17, 2015
 *      Author: root
 */

#ifndef MAINAPPLICATION_H_
#define MAINAPPLICATION_H_

#include "Pin.h"
#include "StepperMotor.h"
#include "DCMotor.h"
#include "Uart.h"
#include "jsmn.h"
#define STEPPER_1_POSITION "s1"
#define STEPPER_2_POSITION "s2"
#define STEPPER_1_RESET "sr1"
#define STEPPER_2_RESET "sr2"
#define STEPPER_1_MODE "sm1"
#define STEPPER_2_MODE "sm2"

#define MOTOR_1_PWM "p1"
#define MOTOR_2_PWM "p2"
#define MOTOR_1_DIRECTION "d1"
#define MOTOR_2_DIRECTION "d2"
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

class MainApplication {
	StepperMotor stepperLeft;
	StepperMotor stepperRight;
	DCMotor motorLeft;
	DCMotor motorRight;
	Pin		builtinLed;

//	int32_t s1 {0};
//	int32_t s2 {0};
//	int8_t sm1 {0};
//	int8_t sm2 {0};
//	int8_t p1 {0};
//	int8_t p2 {0};
//	int8_t d1 {0};
//	int8_t d2 {0};
	static constexpr int SIZE = 100;
	char inputBuffer[SIZE];

	void exec1000Ms();
	void exec100Ms();
	void exec50Ms();

	void readToBuffer(const char *jsonString, jsmntok_t *t, int *i);

	void parseStepper1Position(const char *jsonString, jsmntok_t *t, int *i);
	void parseStepper2Position(const char *jsonString, jsmntok_t *t, int *i);

	void parseStepper1Reset(const char *jsonString, jsmntok_t *t, int *i);
	void parseStepper2Reset(const char *jsonString, jsmntok_t *t, int *i);

	void parseStepper1Mode(const char *jsonString, jsmntok_t *t, int *i);
	void parseStepper2Mode(const char *jsonString, jsmntok_t *t, int *i);

	void parseStepper1TimeInterval(const char *jsonString, jsmntok_t *t, int *i);
	void parseStepper2TimeInterval(const char *jsonString, jsmntok_t *t, int *i);

	void parseMotor1PWM(const char *jsonString, jsmntok_t *t, int *i);
	void parseMotor2PWM(const char *jsonString, jsmntok_t *t, int *i);

	void parseMotor1Direction(const char *jsonString, jsmntok_t *t, int *i);
	void parseMotor2Direction(const char *jsonString, jsmntok_t *t, int *i);
public:
	void parseUART();
public:
	MainApplication();

	uint8_t exec();
};

template<typename T>
inline T convertToInteger(char *data)
{
	return atol(data);
}

#endif /* MAINAPPLICATION_H_ */
