#include "../include/nano.hpp"

#include "../include/stream.hpp"

/* closes serial if necessary */
SerialCOM::~SerialCOM() {
    if (fd_ != -1)
        ::close(fd_);
}

/* open file and setup serial */
bool SerialCOM::open(char const *devname) {
    if (fd_ != -1)
        ::close(fd_);

    // open file (read & write)
    fd_ = ::open(devname, O_RDWR | O_NOCTTY | O_NDELAY);

    // err check
    if (fd_ < 0) {
        perror(devname);
        return false;
    }

    // setup a termios struct
    struct termios tio;
    ::tcgetattr(fd_, &tio);
    cfmakeraw(&tio);
    tio.c_iflag &= ~(IXON | IXOFF);

    // set baud rate
    cfsetspeed(&tio, B115200);
    tio.c_cc[VTIME] = 1;
    tio.c_cc[VMIN] = 5;

    //  set the serial settings
    int err = ::tcsetattr(fd_, TCSAFLUSH, &tio);

    // err check
    if (err != 0) {
        perror(devname);
        ::close(fd_);
        fd_ = -1;
        return false;
    }
    return true;
}

/* Setup the NANO class */
NANO::NANO() {
    memset(&gamepad, 0, sizeof(GAMEPAD));

    // open the uart
    // (on the Jetson NANO this is the file)
    uart.open("/dev/ttyTHS1");
    FD = uart.fd_;

    // error check
    if (FD == -1) {
        perror("FD:");
    }

    stopVideo = false;

    // stream = VideoStream(1);
    // stream.isStreamOn = false;
}

/* Sends a string message over UART to the ESP32 */
void NANO::UART_SendMessage(const void *message, int size) {
    if (write(FD, message, size) == -1) {
        perror("WRITE:");
    }

    // usleep(10000);
}

/* Tank drive mode, currently assuming that parameters are at proper magnitude */

void NANO::Tank(int y1, int y2, bool bumper) {
    float multiplier = 1;
    // Creates a deadzone: if x and y are within deadzone, set them to 0
    if ((y1 < 0.1 * 32767 && y1 > -0.1 * 32768) && (y2 < 0.1 * 32767 && y2 > -0.1 * 32768)) {
        y1 = 0;
        y2 = 0;
    }

    if (bumper) {
        multiplier = .35;
    }

    drivemotors.frontleft = y1 * multiplier;
    drivemotors.frontright = y2 * multiplier;
    drivemotors.backleft = y1 * multiplier;
    drivemotors.backright = y2 * multiplier;
}

int NANO::InterpolateMotorValues(int in) {
    if (in == 0) {
        return 1500;
    }
    int minOutRange, minInRange, maxOutRange, maxInRange;
    minInRange = 1;
    maxInRange = 255;

    if (in > 0) {
        minOutRange = 1525;
        maxOutRange = 2000;
    } else {
        in = abs(in);

        // flip in place
        int distfrommedian = 128 - in;
        in = 128 + distfrommedian;

        minOutRange = 1000;
        maxOutRange = 1475;
    }

    return minOutRange + (in - minInRange) * (maxOutRange - minOutRange) / (maxInRange - minInRange);
}

/* Arcade drive mode, currently assuming that parameters are at proper magnitudes */
void NANO::Arcade(int x, int y, bool bumper) {
    float multiplier = 1;
    // Creates a deadzone: if x and y are within deadzone, set them to 0
    if ((x < 0.1 * 32767 && x > -0.1 * 32768) && (y < 0.1 * 32767 && y > -0.1 * 32768)) {
        x = 0;
        y = 0;
    }

    if (bumper) {
        multiplier = .35;
    }

    // Map to [-255,255] range
    x = (x * 255) / 32768;
    y = (y * 255) / 32768;

    drivemotors.frontleft = ((y + x) / 2) * multiplier;
    drivemotors.frontright = ((y - x) / 2) * multiplier;
    drivemotors.backleft = ((y + x) / 2) * multiplier;
    drivemotors.backright = ((y - x) / 2) * multiplier;

    drivemotors.frontleft = InterpolateMotorValues(drivemotors.frontleft);
    drivemotors.frontright = InterpolateMotorValues(drivemotors.frontright);
    drivemotors.backleft = InterpolateMotorValues(drivemotors.backleft);
    drivemotors.backright = InterpolateMotorValues(drivemotors.backright);

    drivemotors.backright = 1500 - (drivemotors.backright - 1500);
    drivemotors.frontright = 1500 - (drivemotors.frontright - 1500);
}


/* Parses gamepad input */
bool NANO::ParseGamepad(GAMEPAD &g) {
    /* ONLY USE ONE */
    // Tank(g.sThumbLY, g.sThumbRY, g.wButtons && XINPUT_GAMEPAD_RIGHT_SHOULDER);
    Arcade(g.sThumbLX, g.sThumbLY, g.wButtons && XINPUT_GAMEPAD_RIGHT_SHOULDER);

    //printf("before video stream if %d\n", stream.isStreamOn);
    if (ButtonPressEvent(XINPUT_GAMEPAD_DPAD_DOWN) && videoOn == false) {
        printf("Starting video stream\n");
        if (firstVideoHit) {
            stopVideo = false;
            firstVideoHit = false;
            videoOn = true;
            StartVideoStream();
        } else {
            stopVideo = false;
            videoOn = true;
        }
    } else if (ButtonPressEvent(XINPUT_GAMEPAD_DPAD_DOWN) && videoOn == true) {
        printf("WAITING FOR VIDEO THREAD TO FINISH\n");
        stopVideo = true;
        videoOn = false;
        //WaitForVideoStreamToExit();
    }

    /* Parse rest of controls here*/

    return true;
}

bool NANO::AllValsZero() {
    return (drivemotors.frontleft == 1500 && drivemotors.frontright == 1500 && drivemotors.backleft == 1500 && drivemotors.backright == 1500);
}

void NANO::SendMotors() {
    char allzero = 0x00;
    if (AllValsZero()) {
        allzero = 0xff;
    }

    // send the struct to the ESP32
    // with s to designate start and e to designate end
    printf("fl: %d, fr: %d, bl: %d, br: %d, allzero: %x\n", drivemotors.frontleft, drivemotors.frontright, drivemotors.backleft, drivemotors.backright, allzero);
    UART_SendMessage("s", 1);
    UART_SendMessage(&drivemotors.frontleft, sizeof(drivemotors.frontleft));
    UART_SendMessage(&drivemotors.frontright, sizeof(drivemotors.frontright));
    UART_SendMessage(&drivemotors.backleft, sizeof(drivemotors.backleft));
    UART_SendMessage(&drivemotors.backright, sizeof(drivemotors.backright));
    UART_SendMessage(&allzero, sizeof(allzero));
    UART_SendMessage("e", 1);
}

void NANO::SetMotorsToZeroAndSend() {
    printf("SETTING TO 0\n");
    drivemotors.frontleft = 1500;
    drivemotors.frontright = 1500;
    drivemotors.backleft = 1500;
    drivemotors.backright = 1500;

    SendMotors();
}

void NANO::UpdatePreviousGamepad()
{
    previousGamepad = gamepad;
}

bool NANO::ButtonPressEvent(short button)
{
    return ((gamepad.wButtons & button) > 0) && ((previousGamepad.wButtons & button) == 0);
}

void video_thread(bool *stop, bool *kys) {
    VideoStream stream;
    stream.killStream = stop;
    stream.kys = kys;

    stream.StartStream();
}

void NANO::StartVideoStream()
{
    video_t = std::thread(video_thread, &stopVideo, &videokys);
}

void NANO::WaitForVideoStreamToExit() {
    video_t.join();
}