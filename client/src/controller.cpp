#include "../include/controller.hpp"

Controller::Controller(){
  ZeroMemory(&vib, sizeof(XINPUT_VIBRATION));
  ZeroMemory(&base, sizeof(XINPUT_VIBRATION));
  ZeroMemory(&state, sizeof(XINPUT_STATE));

  vib.wLeftMotorSpeed = 30000;
  vib.wRightMotorSpeed = 30000;
  base.wLeftMotorSpeed = 0;
  base.wRightMotorSpeed = 0;
}