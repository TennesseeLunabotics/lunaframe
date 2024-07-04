#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <windows.h>
#include <Xinput.h>

/* This class contains the XINPUT
 * parameters for a controller
 */
class Controller{
    public:
    /* Zeros all the memory and sets the base vibration values */
    Controller();
    XINPUT_STATE state;
    XINPUT_VIBRATION base, vib;
};
#endif