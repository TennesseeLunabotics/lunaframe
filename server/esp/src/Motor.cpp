#include "Arduino.h"
#include "Motor.h"


/* Set channels and pins */
Motor::Motor(int pin)
{
  servo.attach(pin, 500, 2500);
}

/* Run Motor */
void Motor::Spin(int speed)
{
  servo.writeMicroseconds(speed);
}

