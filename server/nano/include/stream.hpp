#ifndef Video_Stream_H
#define Video_Stream_H

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include "comm.hpp"
#include <vector>

class VideoStream {
    public:
        VideoStream(int deviceID = 0, int apiID = cv::CAP_ANY);

        void StartStream();

        cv::Mat frame;
        bool isStreamOn;
        bool *killStream = NULL; // set before calling StartStream()
        bool *kys = NULL; // set before calling StartStream() (actually kills the stream)
        std::vector<uchar> img;
        COM comm;
};

#endif
