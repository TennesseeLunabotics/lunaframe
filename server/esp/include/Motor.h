#pragma once
#define LEDC_TIMER_16_BIT  16
#define LEDC_BASE_FREQ     10000

#include "ESP32Servo.h"

// class Motor {
//   public:
//     Motor(int);
//     void spin(int);

//   private:
//     int pwmpin;
// };

class Motor {
  public:
    Motor(int);    /* Constructor */
    void Spin(int);          /* Function to spin Motor */
    
  private:
    int pwmpin;         /* Pin for forward output */
    Servo servo;
};

