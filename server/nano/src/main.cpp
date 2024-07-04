#include <string>
//#include <signal.h>
#include <csignal>
#include "../include/nano.hpp"

void Write2Serial(NANO &nano);
void SendClientDataToSerial(NANO &nano);

void signalHandler(int signum) {
    printf("PIPE BROKE\n");
}

int main(int argc, char **argv) {
    std::signal(SIGPIPE, signalHandler);
    //sigaction(SIGPIPE, &(struct sigaction){SIG_IGN}, NULL);
    NANO nano;
    
    // usage check
    if (argc < 2) {
        printf("Usage: ./server [client/server]\n");
        printf("client == Send controller input over serial to ESP32\n");
        printf("server == Send Nano's terminal input over serial to ESP32\n");
        return 1;
    }

    // if client then send the client controller data to the ESP32
    // otherwise spam a message to test ESP32 connection
    if (strcmp(argv[1], "client") == 0) {
        SendClientDataToSerial(nano);
    } else if (strcmp(argv[1], "server") == 0) {
        Write2Serial(nano);
    }

    return 0;
}

/* Does what it says */
void SendClientDataToSerial(NANO &nano) {
    // startup the server
    nano.server.StartServer(PORT);
    //nano.video.StartServer(VIDEOPORT);

    //video_t = std::thread(&VideoStream::start, &stream);
    //video_t.detach();

    // accept clients and send their data to the ESP32 forever
    while (1) {
        usleep(25);
        if (!nano.server.HasClient()) {
            printf("Waiting for client\n");

            // video had been ran
            // so we gotta clean em up
            if (!nano.firstVideoHit) {
                nano.videokys = true;
                printf("Waiting for video stream to exit\n");
                nano.WaitForVideoStreamToExit();
                printf("videostream has exited\n");
                nano.videokys = false;
                nano.firstVideoHit = true;
                nano.videoOn = false;
                nano.stopVideo = false;
            }
            // nano.SetMotorsToZeroAndSend();
            nano.server.AwaitClient();
        }

        printf("Getting controller data\n");
        nano.server.GetControllerData(nano.gamepad, &nano);

        printf("parsing\n");
        if (!nano.ParseGamepad(nano.gamepad)){
            nano.SetMotorsToZeroAndSend();
            continue;
        }

        printf("Sending motor commands\n");
        nano.SendMotors();

        nano.UpdatePreviousGamepad();
    }
}

/* Wries the string to the ESP32 (FOR TESTING) */
void Write2Serial(NANO &nano) {
    char c[5000];
    while (1) {
        // delay for sending (may not be necessary need to test)
        usleep(5000);
        scanf("%s", c);
        nano.UART_SendMessage(c, strlen(c));
        printf("after SendMessage fd=%d\n", nano.FD);
        memset(c, 0, 5000);
    }
}
