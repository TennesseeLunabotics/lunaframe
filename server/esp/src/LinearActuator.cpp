#include "Arduino.h"
#include "LinearActuator.h"

#define LEDC_TIMER_16_BIT  16
#define LEDC_BASE_FREQ     10000

/* Initialize linear actuator and pins*/
LinearActuator::LinearActuator(int pwm, int dir, int pot)
{
    pwm_pin = pwm;
    dir_pin = dir;
    pot_pin = pot;

    pinMode(pwm_pin, OUTPUT);
    pinMode(dir_pin, OUTPUT);
    pinMode(pot_pin, INPUT);
}

/* Drive linear actuator */
void LinearActuator::drive(int val)
{
    val <<= 1;
    // analog must come before the digital write
    // (Makes sure the linear actuators stop before flipping their direction)
    if(val < 0) {
        digitalWrite(dir_pin, LOW);
        analogWrite(pwm_pin, abs(val));
    } else {
        digitalWrite(dir_pin, HIGH);
        analogWrite(pwm_pin, val);
    }
}