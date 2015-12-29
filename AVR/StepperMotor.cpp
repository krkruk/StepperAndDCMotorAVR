/*
 * StepperMotor.cpp
 *
 *  Created on: Sep 30, 2015
 *      Author: root
 */

#include "StepperMotor.h"

//StepperMotorAlgorithmAbstract BEGIN *************************************
int8_t StepperMotorAlgorithmAbstract::stepsAvailable() const
{
	/*
	 * Return number of available steps in a particular algorithm
	 */
	return totalSteps;
}

int8_t StepperMotorAlgorithmAbstract::normalizeStep(const int8_t &nextStep)
{
	/*
	 * Normalize the next position for the stepper motor.
	 * If the value exceeds possible range, the function returns
	 * the nearest proper value in a cyclic manner.
	 * E.g. nextStop = 5 and availableSteps = 4; return 0;
	 */
	if(nextStep >= totalSteps)
		return 0;
	if(nextStep < 0)
		return totalSteps - 1;

	return nextStep;
}
//StepperMotorAlgorithmAbstract END ***************************************

//StepperMotorFullStep BEGIN **********************************************
StepperMotorFullStep::StepperMotorFullStep(Pin *pin1, Pin *pin2, Pin *pin3, Pin *pin4)
	: StepperMotorAlgorithmAbstract(pin1, pin2, pin3, pin4, 4)
{}

StepperMotorFullStep::StepperMotorFullStep()
	: StepperMotorAlgorithmAbstract(4)
{
}

void StepperMotorFullStep::step(uint8_t step)
{
	/*
	 * Execute a step in a full step manner.
	 * Possible values are [0..3]
	 */
	switch(step)
	{
	case 1:
		pin1->unset();
		pin2->set();
		pin3->unset();
		pin4->unset();
		break;
	case 2:
		pin1->unset();
		pin2->unset();
		pin3->set();
		pin4->unset();
		break;
	case 3:
		pin1->unset();
		pin2->unset();
		pin3->unset();
		pin4->set();
		break;
	case 0:
	default:
		pin1->set();
		pin2->unset();
		pin3->unset();
		pin4->unset();
		break;
	}
}
void StepperMotorFullStep::setPins(Pin* pin1, Pin* pin2, Pin* pin3, Pin* pin4)
{
	this->pin1 = pin1;
	this->pin2 = pin2;
	this->pin3 = pin3;
	this->pin4 = pin4;
}
//StepperMotorFullStep END *************************************************

//StepperMotorHalfStep BEGIN ***********************************************
StepperMotorHalfStep::StepperMotorHalfStep(Pin* pin1, Pin* pin2, Pin* pin3,
		Pin* pin4)
	: StepperMotorAlgorithmAbstract(pin1, pin2, pin3, pin4, 8)
{}

StepperMotorHalfStep::StepperMotorHalfStep()
	: StepperMotorAlgorithmAbstract(8)
{
}

void StepperMotorHalfStep::step(uint8_t step)
{
	/*
		 * Execute a step in a half step manner.
		 * Possible values are [0..7]
		 */
	switch(step)
	{
	case 1:
		pin1->set();
		pin2->set();
		pin3->unset();
		pin4->unset();
		break;
	case 2:
		pin1->unset();
		pin2->set();
		pin3->unset();
		pin4->unset();
		break;
	case 3:
		pin1->unset();
		pin2->set();
		pin3->set();
		pin4->unset();
		break;
	case 4:
		pin1->unset();
		pin2->unset();
		pin3->set();
		pin4->unset();
		break;
	case 5:
		pin1->unset();
		pin2->unset();
		pin3->set();
		pin4->set();
		break;
	case 6:
		pin1->unset();
		pin2->unset();
		pin3->unset();
		pin4->set();
		break;
	case 7:
		pin1->set();
		pin2->unset();
		pin3->unset();
		pin4->set();
		break;
	case 0:
	default:
		pin1->set();
		pin2->unset();
		pin3->unset();
		pin4->unset();
		break;
	}
}

void StepperMotorHalfStep::setPins(Pin* pin1, Pin* pin2, Pin* pin3, Pin* pin4)
{
	this->pin1 = pin1;
	this->pin2 = pin2;
	this->pin3 = pin3;
	this->pin4 = pin4;
}
//StepperMotorHalfStep END **************************************************

//StepperMotor BEGIN ********************************************************
StepperMotor::StepperMotor(Pin&& pin1, Pin&& pin2, Pin&& pin3, Pin&& pin4)
	: pin1(pin1), pin2(pin2), pin3(pin3), pin4(pin4),
	  algorithm(new StepperMotorFullStep(&this->pin1, &this->pin2, &this->pin3, &this->pin4)),
	  lastStepTime(millis())
{
	/*
	 *
	 */
}

StepperMotor::~StepperMotor()
{
	delete algorithm;
}

void StepperMotor::applyStep(const DIRECTION &dirn)
{
	/*
	 * Apply a single step in the selected direction
	 */
	currentMiddleStep = middleStep(dirn);
	algorithm->step(currentMiddleStep);
}

int_fast8_t StepperMotor::middleStep(const DIRECTION &dirn)
{
	/*
	 * Compute the next step value for the StepperMotorAlgorithmAbstract::step(*)
	 */
	switch(dirn)
	{
	case DIRECTION::CLOCKWISE:
		currentMiddleStep = algorithm->normalizeStep(++currentMiddleStep);
		++currentPosition;
		break;
	case DIRECTION::COUNTERCLOCKWISE:
		currentMiddleStep = algorithm->normalizeStep(--currentMiddleStep);
		--currentPosition;
		break;
	default:
		currentMiddleStep = 0; break;
	}
	return currentMiddleStep;
}

void StepperMotor::rotateClockwise()
{
	/*
	 * Rotate clockwise.
	 * The revolving starts automatically by default. See
	 * rotateStart() for more details.
	 */
	if(millis() - lastStepTime > timeInterval
			&& stopFlag)
	{
		lastStepTime = millis();
		applyStep(DIRECTION::CLOCKWISE);
	}
}

void StepperMotor::rotateCounterclockwise()
{
	/*
	 * Rotate counterclockwise
	 * The revolving starts automatically by default. See
	 * rotateStart() for more details.
	 */
	if(millis() - lastStepTime > timeInterval
			&& stopFlag)
	{
		lastStepTime = millis();
		applyStep(DIRECTION::COUNTERCLOCKWISE);
	}
}

void StepperMotor::rotateStop()
{
	/*
	 * Stops constant revolving of the stepper shaft.
	 */
	stopFlag = 0;
}

void StepperMotor::rotateStart()
{
	/*
	 * Starts constant revolving of the stepper shaft only if
	 * rotate*() function is invoked.
	 *
	 * The constant rotation movement is set by default.
	 */
	stopFlag = 1;
}


void StepperMotor::step(int16_t steps)
{
	/*
	 * Set number of steps to be executed.
	 * Positive value determines clockwise rotation.
	 * Negative value determines counterclockwise rotation.
	 *
	 * In order to apply these changes stepUntil() function
	 * must be called in the main event loop of the program.
	 */
	desiredDirection = DIRECTION::CLOCKWISE;
	if(steps < 0)
		desiredDirection = DIRECTION::COUNTERCLOCKWISE;

	desiredPosition = currentPosition + steps;
}

void StepperMotor::stepUntil()
{
	/*
	 * Rotate the shaft until the stepper reaches
	 * the position passed by step(*) function.
	 *
	 * The function should be run in the main event loop.
	 */
	if(millis() - lastStepTime > timeInterval
			&& !isPositionReached())
	{
		lastStepTime = millis();
		applyStep(desiredDirection);
	}
}

void StepperMotor::stepStop()
{
	/*
	 * Stop rotation move in determined steps mode.
	 * The function assigns current position as the value passed
	 * in step(*) function was reached.
	 */
	desiredPosition = currentPosition;
}

uint8_t StepperMotor::isPositionReached() const
{
	/*
	 * Get flag whether the stepper reached the
	 * desired position.
	 */
	if(desiredPosition == currentPosition)
		return 1;
	else
		return 0;
}

uint8_t StepperMotor::getTimeInterval() const
{
	/*
	 * Return the time interval in ms the stepper works.
	 */
	return timeInterval;
}

void StepperMotor::setTimeInterval(uint8_t timeInterval)
{
	/*
	 * Set the time span (in ms) the steps are performed.
	 */
	this->timeInterval = timeInterval;
}

int_fast32_t StepperMotor::currentStepperPosition() const
{
	/*
	 * Return current position in number of steps
	 * since the stepper was started or value reseted.
	 *
	 * The value is vulnerable to the change of the algorithm
	 * the stepper works.
	 */
	return currentPosition;
}

void StepperMotor::setAlgorithm(
		StepperMotorAlgorithmAbstract* algorithmSelect)
{
	/*
	 * Set an algorithm, the StepperMotor class works.
	 * By default, the class works in the FullStep mode.
	 */
	delete algorithm;
	algorithm = algorithmSelect;
	algorithm->setPins(
			&this->pin1,
			&this->pin2,
			&this->pin3,
			&this->pin4
			);
}
//StepperMotor END **********************************************************
