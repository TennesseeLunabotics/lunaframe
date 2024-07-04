#include "../include/stream.hpp"

using namespace cv;
using namespace std;

VideoStream::VideoStream(int deviceID, int apiID) {
    isStreamOn = false;
    //killStream = false;
}

void VideoStream::StartStream() {
    std::vector<int> param(2);
    param[0] = cv::IMWRITE_JPEG_QUALITY;
    param[1] = 20;  // default(95) 0-100

    cv::VideoCapture cap = cv::VideoCapture(cv::CAP_V4L2);

    if (!cap.isOpened()) {
        printf("Failed to open\n");

        return;
    }

    printf("Starting the server\n");
    comm.StartServer(VIDEOPORT);

    while (!comm.HasClient()) {
        comm.AwaitClient();
    }

    // these are the specific things to make it work on the NANO
    // only change resolution and fps
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 320);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 240);
    cap.set(cv::CAP_PROP_FPS, 10);

    for (;;) {
        // wait for a new frame from camera and store it into 'frame'
        cap.read(frame);

        // check if we succeeded
        if (frame.empty()) {
            cerr << "ERROR! blank frame grabbed\n";
            break;
        }

        imencode(".jpg", frame, img, param);

        // make sure to die if needed
         if(*(this->killStream)) {
            //comm.HandleDisconnect();
            printf("PAUSING STREAM\n");
            while (*(this->killStream)) {
                // don't actually kill stream
                // variable name is a lie
                usleep(500000);

                if (*(this->kys)) {
                    // literally die like rn
                    // kill thread like a boss
                    printf("CAMERA THREAD DYING\n");
                    comm.HandleDisconnect();
                    return;
                }
            }
            printf("RESUME STREAM\n");
            //return;
            //break;
        }

        // okay we actually may have to die
        if (*(this->kys)) {
            // literally die like rn
            // kill thread like a boss
            printf("CAMERA THREAD DYING\n");
            comm.HandleDisconnect();
            return;
        }

        // tell the client how big the image is
        // then send it
        // 
        // (images can be of variable size and client needs to know when to display image)
        size_t tmp = img.size();
        if (comm.SendMessage((char*)&tmp, sizeof(tmp))) {
            break;
        }
        if (comm.SendImage(img)){
            break;
        }
    }

    //comm.HandleDisconnect();
}
