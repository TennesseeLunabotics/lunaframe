#include "Drivetrain.h"

/* Save motors under drivetrain */
Drivetrain::Drivetrain(Motor* RF, Motor* LF, Motor* RB, Motor* LB) {
    rf = RF; 
    lf = LF;
    rb = RB;
    lb = LB;
}

/* Drivetrain drive, forwards all values to motors. */
void Drivetrain::drive(short fl, short fr, short bl, short br){
    //with controller logic happening on nano, we don't need Tank and Arcade functions

    lf->Spin((int) fl);
    rf->Spin((int) fr);
    lb->Spin((int) bl);
    rb->Spin((int) br);
}
