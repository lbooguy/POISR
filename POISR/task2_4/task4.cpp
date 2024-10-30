#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;

static void get_frame(string in_path, string out_path) {
    cv::VideoCapture cap;
    cap.open(in_path);
    cout << "Start. \n";

    if (!cap.isOpened())
    {
        cout << "Cannot open the video file. \n";
    }

    double fps = cap.get(cv::CAP_PROP_FPS);
    cv::Size size(
        (int)cap.get(cv::CAP_PROP_FRAME_WIDTH),
        (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT)
    );

    cv::VideoWriter writer;
    //writer.open(out_path, cv::VideoWriter::fourcc('H', '2', '6', '4'), fps, size);

    namedWindow("Hough", cv::WINDOW_NORMAL);
    cv::Mat frame, gray, frame_Canny;
    int frame_count = 0;

    while (true)
    {
        cap >> frame; if (frame.empty()) break;
        frame_count++;
        if (frame_count == 190) {
            imwrite(out_path, frame);
            break;
        }
        imshow("Hough", frame);
        //writer << frame;

        if (cv::waitKey(30) == 27)  break;// 'esc'
    }
}



int main(int argc, char** argv) {
    get_frame("task2_4/Calibration_02.mp4", "task2_4/190.png");
   }
