#include <termios.h>
#include <sys/fcntl.h>
#include <thread>

#include "comm.hpp"
#include "stream.hpp"
#include "ansicodes.hpp"

/* Handles serial connection */
struct SerialCOM {
  SerialCOM() : fd_(-1) {}
  ~SerialCOM();

  /* open file and setup serial */
  bool open(char const *devname);

  int fd_; // handle on the file
};

typedef struct {
    short frontleft;
    short frontright;
    short backleft;
    short backright;
}DRIVEMOTORS;

typedef struct {
    signed char val; 
}LINEAR_ACTUATOR_PAIR;

class NANO{
  public:
    NANO();

    /* Sends a string message over UART to the ESP32 */
    void UART_SendMessage(const void *message, int size);

    /* Send all motors to the ESP32 */
    void SendMotors();

    void SetMotorsToZeroAndSend();

    /* Parses gamepad input */
    bool ParseGamepad(GAMEPAD &g);

    /* Scale motor pwm values to frequency required */
    int InterpolateMotorValues(int);

    /* ButtonPressEvent */
    void UpdatePreviousGamepad();
    bool ButtonPressEvent(short button);

    /* Control functions */
    void Tank(int, int, bool);
    void Arcade(int, int, bool);
    bool AllValsZero();

    void StartVideoStream();

    void WaitForVideoStreamToExit();

    COM server;
    GAMEPAD gamepad;
    GAMEPAD previousGamepad;
    SerialCOM uart;
    DRIVEMOTORS drivemotors;
    bool beltOn = false;
    short beltFrequency = 1500;
    bool stopVideo = false;
    bool videoOn = false;
    bool videokys = false;
    bool firstVideoHit = true;
    int FD; // easier access to uart.fd_
    std::thread video_t;
};
