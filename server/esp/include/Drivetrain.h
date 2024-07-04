#pragma once
#include "Motor.h"
#include <stdio.h>

class Drivetrain {
    public:
        Drivetrain(Motor*, Motor*, Motor*, Motor*);
        void drive(short, short, short, short);

    private:
        Motor *rf;
        Motor *lf;
        Motor *rb;
        Motor *lb;
};