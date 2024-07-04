#pragma once
class LinearActuator {
    private:
        int pwm_pin;
        int dir_pin;
        int pot_pin;

    public:
        LinearActuator(int, int, int);
        void drive(int);

};