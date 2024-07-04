#include "../include/comm.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

#define LUNAADDR "ENTER IP / HOSTNAME HERE"
// #define LUNAADDR "lunabot" // For headless setup

int DisplayVideo(bool *videoOn) {
  tcp_stream stream;

  stream.StartStream(LUNAADDR, "5241");

  cv::Mat frame;

  std::vector<uchar> buf(1500000);
  int bytes_read;

  size_t img_size;
  int tmp;
  int total_received;
  int num_fds = 2;
  fd_set r_set, w_set, e_set;
  //TIMEVAL tv = {.tv_sec = 0, .tv_usec = 100000L};
  //FD_ZERO(&r_set), FD_ZERO(&w_set), FD_ZERO(&e_set);
  
  cv::namedWindow("Video from server", cv::WINDOW_GUI_EXPANDED);
  while (true) {
    if (!(*videoOn)) {
      // sleep till that junk back
      while(!(*videoOn)){
        Sleep(500);
      }

      //stream.CloseStream();
      //cv::destroyWindow("Video from server");
      //return 0;
    }

    //if (select(num_fds, &r_set, &w_set, &e_set, &tv) == 0) continue;
    // read
    tmp = stream.recieve((char*)&img_size, sizeof(img_size));
    if (tmp < 0) {
      printf("Failed to receive image from remote. Killing video thread.\n");
      stream.CloseStream();
      cv::destroyWindow("Video from server");
      return -1;
    }

    total_received = 0;
    while (total_received < img_size) {
      tmp = stream.recieve((char*)buf.data() + total_received, img_size - total_received);
      if (tmp < 0) {
        printf("Failed to receive image from remote. Killing video thread.\n");
        stream.CloseStream();
        cv::destroyWindow("Video from server");
        return -1;
      }

      total_received += tmp;
    }

    bytes_read = total_received;

    if (bytes_read < 0 || bytes_read == 0) {
      printf("What the actual hell %d\n", bytes_read);
      return 0;
    }

    frame = cv::imdecode(cv::Mat(1, bytes_read, CV_8UC1, buf.data()),
                         cv::IMREAD_UNCHANGED);

    if (!(frame.empty())) {
      cv::imshow("Video from server", frame);
      cv::waitKey(1);
    }
  }
  return -1;
}

bool ButtonPressEvent(Controller control, Controller previousControl, short button)
{
    return ((control.state.Gamepad.wButtons & button) > 0) && ((previousControl.state.Gamepad.wButtons & button) == 0);
}

int main(int argc, char** argv) {
  WSADATA data;
  Controller control;
  Controller previousControl;
  const char* ip;
  tcp_stream stream;
  //tcp_stream videostream;
  std::thread videothread;
  bool first_time = true;

  if (argc == 2) {
    ip = argv[1];
  } else {
    ip = LUNAADDR;
  }

  // startup the controller and connect to the nano
  WSAStartup(MAKEWORD(2, 2), &data);
  DWORD dwResult = -1;
  while (dwResult != ERROR_SUCCESS) {
    dwResult = XInputGetState(0, &control.state);
    printf("No controller connected. Will check again in .5 seconds.\n");
    Sleep(500);
    //printf("dwResult %d\n", dwResult);
  }

  printf("Connecting to server...\n");
  stream.StartStream(ip, "5240");
  //videostream.StartStream(ip, "5241");


  //videothread = std::thread(DisplayVideo, std::ref(videostream));

  bool videoOn = false;

  char tmp;
  bool first_run = true;

  // loop until back button is pressed on the controller
  while (true) {
    Sleep(50);
    dwResult = XInputGetState(0, &control.state);

    stream.send(control.state.Gamepad);

    if (ButtonPressEvent(control, previousControl, XINPUT_GAMEPAD_DPAD_UP)) {
      printf("yooo\n");
    }
    if (ButtonPressEvent(control, previousControl, XINPUT_GAMEPAD_BACK)) {
      printf("EXITING\n");
      break;
    }

    // send the controller data to the nano
    // video
    if (ButtonPressEvent(control, previousControl, XINPUT_GAMEPAD_DPAD_DOWN)) {
      if (!videoOn) {
        printf("Trying to start stream\n");
        //printf("Stream started\n");
        
        //if (first_time) {
          //videostream.SetNoBlocking();
          //videostream_alive = true;
          videoOn = true;
          if (first_run) {
            videothread = std::thread(DisplayVideo, &videoOn);
            first_run = false;
          }
          //videothread.detach();
          //first_time = false; // :(
        //}
        printf("before thread run\n");
        printf("running display video\n");
      } else {
        printf("Pressed DPAD DOWN while video is alive\n");
        //videostream_alive = false;
        printf("Waiting for stuff\n");
        videoOn = false;

        //videothread.join();
        //printf("STUFF DONE :)\n");
        //videostream.CloseStream();
      }
    }

    //stream.send(control.state.Gamepad);

    memcpy_s(&previousControl, sizeof(previousControl), &control, sizeof(control));
    //previousControl = control;
  }
  printf("shutting down\n");
  exit(1);
  //videoOn = false;
  //videothread.join();
  return 0;
}
