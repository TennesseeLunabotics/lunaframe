#include "Controller.h"

Controller::Controller() {
        lx = 0;
        ly = 0;     
        rx = 0; 
        ry = 0;    
        A = 0;
        B = 0;
        X = 0;
        Y = 0;     
        Up = 0;
        Down = 0;
        Left = 0;
        Right = 0; 
        LB = 0;
        RB = 0;        
        LT = 0;
        RT = 0;                 
        start = 0;
        options = 0;     
}

void Controller::Update(short buttons, short x1, short y1, short x2, short y2) {
        lx = x1;
        ly = y1;
        rx = x2;
        ry = y2;
        A = buttons & 0x1000;
        B = buttons & 0x2000;
        X = buttons & 0x4000;
        Y = buttons & 0x8000;
        Up = buttons & 0x0001;
        Down = buttons & 0x0002;
        Left = buttons & 0x0004;
        Right = buttons & 0x0008;
        LB = buttons & 0x0100;
        RB = buttons & 0x0200;
        LT = buttons & 0x0400;
        RT = buttons & 0x0800;
        start = buttons & 0x0010;
        options = buttons & 0x0020;
}

