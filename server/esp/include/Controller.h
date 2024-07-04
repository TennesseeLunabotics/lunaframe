#pragma once
class Controller {
    public:
        Controller(); 
        void Update(short, short, short, short, short);
        int lx, ly;                   /* Left stick axes (-1 <= a <= 1) */
        int rx, ry;                   /* Right stick axes (-1 <= a <= 1) */
        int A, B, X, Y;                 /* Letter buttons (0/1) */
        int Up, Down, Left, Right;      /* Directional buttons (0/1) */
        int LB, RB;                     /* Left and right buttons (0/1) */
        int LT, RT;                     /* Left and right triggers (0/1) */
        int start, options;             /* Start and Options button (0/1) */
};