#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
#include <cmath>
#include <random>

using namespace std;




static void get_frame(const string in_path, const string out_segm_path, const string out_canny_path) {
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
    cv::VideoWriter writer_segm;
    writer_segm.open(out_segm_path, cv::VideoWriter::fourcc('H', '2', '6', '4'), fps, size);

    cv::VideoWriter writer_canny;
    writer_canny.open(out_canny_path, cv::VideoWriter::fourcc('H', '2', '6', '4'), fps, size);

    namedWindow("Src", cv::WINDOW_NORMAL);
    cv::Mat frame, frame_gray, filt, frame_Canny, result;
    int frame_count = 0;

    while (true)
    {
        cap >> frame; if (frame.empty()) break;
        frame_count++;
        cv::pyrMeanShiftFiltering(frame, filt, 20, 40, 2);
        writer_segm << filt;

        cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
        cv::Canny(frame_gray, frame_Canny, 50, 150, 3, true);


        result = filt.clone();
        for (int y = 0; y < frame_Canny.rows; ++y) {
            for (int x = 0; x < frame_Canny.cols; ++x) {
                if (frame_Canny.at<uchar>(y, x) > 0) {
                    cv::circle(result, cv::Point(x, y), 1, cv::Scalar(0, 0, 255), -1);
                }
            }
        }

        writer_canny << result;

        imshow("Src", result);

        if (cv::waitKey(30) == 27)  break;// 'esc'
    }
}





int main(int argc, char** argv) {
    const string in_path = "task2_5/video.mp4";
    const string out_segm_path = "task2_4/segm.mp4";
    const string out_canny_path = "task2_4/segm_with_canny.mp4";

    get_frame(in_path, out_segm_path, out_canny_path);


}
