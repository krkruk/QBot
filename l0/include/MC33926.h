#ifndef MC33926_H_
#define MC33926_H_
#include <Arduino.h>
#include <math.h>
#include "Motor.h"

namespace drv {

/**
 * Class that checks the state of the driver.
 */
template<uint8_t PIN_D2, uint8_t PIN_SF>
class MC33926_Enable
{
public:
    MC33926_Enable()
    {
        pinMode(PIN_D2, OUTPUT);
        pinMode(PIN_SF, INPUT);
    }

    /**
     * Checks the state of the driver
     * @return true if an error occured
     */
    bool error() const
    {
        return digitalRead(PIN_SF) == 0;
    }

    /**
     * Once an error occurs it may be a good thing
     * to restart the driver. D2 pin must be connected
     * to one of the GPIOs
     */
    void restart()
    {
        digitalWrite(PIN_D2, LOW);
        delay(200);
        digitalWrite(PIN_D2, HIGH);
    }
};

/**
 * Null object of _STATE_PROBER
 */
class MC33926_Null
{
public:
    /**
     * By default no errors :)
     */
    bool error() const
    {
        return false;
    }

    /**
     * Empty
     */
    void restart()
    {
    }
};


template
<
    uint8_t PIN_DIR,
	uint8_t PIN_FB,
	class _STATE_PROBER = MC33926_Null
>
class MC33926 : public Motor<MC33926<PIN_DIR, PIN_FB, _STATE_PROBER>>
{
public:
    _STATE_PROBER STATE = _STATE_PROBER();
	MC33926()
	{
		pinMode(PIN_FB, INPUT);
		pinMode(PIN_DIR, OUTPUT);
		DDRD |= (1<<PD6);
		TCCR0A = (1<<COM0A1) | (1<<WGM01) | (1<<WGM00);
		TCCR0B = (1<<CS21);
		this->setSpeed(0);
	}

	void setPwm(uint8_t pwm)
	{
	    OCR0A = pwm;
	}

	uint8_t getPwm() const
	{
	    return OCR0A;
	}

	void setDirection(uint8_t dir)
	{
	    digitalWrite(PIN_DIR, dir);
	}

	uint8_t getDirection() const
	{
	    return digitalRead(PIN_DIR);
	}

	uint16_t getMilliamps() const
	{
		return analogRead(PIN_FB) * 9;
	}

	int16_t getSpeed() const
	{
	    const int8_t multiplier = getDirection() ? 1 : -1;
	    return multiplier * getPwm();
	}

	uint8_t computeDirection(int16_t speed)
	{
	    return speed >= 0;
	}
};

} /* namespace drv */


#endif /* MC33926_H_ */
