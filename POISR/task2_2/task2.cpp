#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;
using namespace cv;

double source_video() {
    VideoCapture cap;
    cap.open("task2_2/Calibration_01.mp4");

    if (!cap.isOpened())
    {
        cout << "Cannot open the video file. \n";
        return -1;
    }

    double fps = cap.get(cv::CAP_PROP_FPS);

    namedWindow("Calibration", cv::WINDOW_NORMAL);
    Mat frame;
    while(true)
    {
        
        cap >> frame; if (frame.empty()) break;

        imshow("Calibration", frame);

        if (waitKey(30) == 27)  break;// 'esc'
    }
}

int main(int argc, char** argv) {
	source_video();
}
