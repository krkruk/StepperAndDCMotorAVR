/*
 * StepperMotor.h
 *
 *  Created on: Sep 30, 2015
 *      Author: root
 */

#ifndef STEPPERMOTOR_H_
#define STEPPERMOTOR_H_
#include "Pin.h"
#include "new.hpp"
class StepperMotor;

class StepperMotorAlgorithmAbstract
{
	friend StepperMotor;
	const uint8_t totalSteps;
	virtual void setPins(Pin *pin1, Pin *pin2, Pin *pin3, Pin *pin4) = 0;
	StepperMotorAlgorithmAbstract(const StepperMotorAlgorithmAbstract&) = delete;
protected:
	Pin *pin1 {nullptr};
	Pin *pin2 {nullptr};
	Pin *pin3 {nullptr};
	Pin *pin4 {nullptr};
public:
	StepperMotorAlgorithmAbstract(uint8_t totalSteps)
		: totalSteps(totalSteps)
	{}
	StepperMotorAlgorithmAbstract(Pin *pin1, Pin *pin2, Pin *pin3, Pin *pin4,
			uint8_t totalSteps)
		: totalSteps(totalSteps), pin1(pin1), pin2(pin2), pin3(pin3), pin4(pin4)
	{
		/*
		 * The algorithm requires 4 pin outputs in order to move the stepper motor
		 * Additionally totalSteps variable is required. The variable stands
		 * for the maximal limit of available steps per single motor rotation.
		 */
	}
	virtual ~StepperMotorAlgorithmAbstract(){}

	int8_t stepsAvailable() const;	//Returns number of steps available in a particular algorithm
	int8_t normalizeStep(const int8_t &nextStep);	//Normalize the step counter. If the step counter exceeds maximal value, the variable is set to the nearest value in a cyclic mode.

	virtual void step(uint8_t step) = 0;	//Execute a step.
};

class StepperMotorFullStep : public StepperMotorAlgorithmAbstract
{
	void setPins(Pin *pin1, Pin *pin2, Pin *pin3, Pin *pin4);
	StepperMotorFullStep(const StepperMotorFullStep&) = delete;
public:
	StepperMotorFullStep();
	StepperMotorFullStep(Pin *pin1, Pin *pin2, Pin *pin3, Pin *pin4);
	virtual ~StepperMotorFullStep(){}

	void step(uint8_t step);
};

class StepperMotorHalfStep : public StepperMotorAlgorithmAbstract
{
	void setPins(Pin *pin1, Pin *pin2, Pin *pin3, Pin *pin4);
	StepperMotorHalfStep(const StepperMotorHalfStep&) = delete;
public:
	StepperMotorHalfStep();
	StepperMotorHalfStep(Pin *pin1, Pin *pin2, Pin *pin3, Pin *pin4);
	virtual ~StepperMotorHalfStep(){}

	void step(uint8_t step);
};

class StepperMotor
{
	enum class DIRECTION { CLOCKWISE, COUNTERCLOCKWISE };

	Pin pin1;
	Pin pin2;
	Pin pin3;
	Pin pin4;

	StepperMotorAlgorithmAbstract *algorithm {nullptr};

	uint64_t lastStepTime;
	uint8_t stopFlag {1};
	int_fast8_t currentMiddleStep {0};
	int_fast32_t currentPosition {0};
	int_fast32_t desiredPosition {0};
	DIRECTION desiredDirection {DIRECTION::CLOCKWISE};

	uint8_t timeInterval {4};


	void applyStep(const DIRECTION &dirn);
	int_fast8_t middleStep(const DIRECTION &dirn);
public:
	StepperMotor(Pin &&pin1, Pin &&pin2, Pin &&pin3, Pin &&pin4);
	~StepperMotor();

	void rotateClockwise();
	void rotateCounterclockwise();
	void rotateStart();
	void rotateStop();

	void step(int16_t steps);
	void stepUntil();
	void stepStop();
	uint8_t isPositionReached() const;
	int_fast32_t currentStepperPosition() const;

	uint8_t getTimeInterval() const;
	void setTimeInterval(uint8_t timeInterval);

	void setAlgorithm(StepperMotorAlgorithmAbstract *algorithmSelect);
	int8_t stepsAvailable() const { return algorithm->stepsAvailable(); }

};

#endif /* STEPPERMOTOR_H_ */
