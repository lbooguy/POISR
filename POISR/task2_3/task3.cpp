#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;

static void hough_lines(string in_path, string out_path, string out_stat_txt_path) {
    cv::VideoCapture cap;
    cap.open(in_path);
    cout << "Start. \n";
    std::ofstream statistic_txt;
    statistic_txt.open(out_stat_txt_path);

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
    writer.open(out_path, cv::VideoWriter::fourcc('H', '2', '6', '4'), fps, size);

    namedWindow("Hough", cv::WINDOW_NORMAL);
    cv::Mat frame, gray, frame_Canny;
    int frame_count = 0;

    while (true)
    {
        cap >> frame; if (frame.empty()) break;
        frame_count++;
        cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::Canny(gray, frame_Canny, 50, 150, 3, true);

        std::vector<cv::Vec2f> lines;

        auto start = chrono::steady_clock::now();
        cv::HoughLines(frame_Canny, lines, 1, CV_PI / 180, 300, 0, 0);
        auto end = chrono::steady_clock::now();
        auto diff = end - start;
        auto time = chrono::duration<double, milli>(diff).count();

        statistic_txt << "Lines number\t" << lines.size() << "\tTime\t" << time << endl;
        for (const auto& line : lines) {       
            float rho = line[0], theta = line[1];
            cv::Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a * rho, y0 = b * rho;
            pt1.x = cvRound(x0 + 2500 * (-b));
            pt1.y = cvRound(y0 + 2500 * (a));
            pt2.x = cvRound(x0 - 2500 * (-b));
            pt2.y = cvRound(y0 - 2500 * (a));
            cv::line(frame, pt1, pt2, cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
        }
        imshow("Hough", frame);
        writer << frame;

        if (cv::waitKey(30) == 27)  break;// 'esc'
    }
}

static void hough_linesP(string in_path, string out_path, string out_stat_txt_path) {
    cv::VideoCapture cap;
    cap.open(in_path);
    cout << "Start. \n";
    std::ofstream statistic_txt;
    statistic_txt.open(out_stat_txt_path);

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
    writer.open(out_path, cv::VideoWriter::fourcc('H', '2', '6', '4'), fps, size);

    namedWindow("Hough", cv::WINDOW_NORMAL);
    cv::Mat frame, gray, frame_Canny;
    int frame_count = 0;

    while (true)
    {
        cap >> frame; if (frame.empty()) break;
        frame_count++;
        cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::Canny(gray, frame_Canny, 50, 150, 3, true);

        std::vector<cv::Vec4i> lines;
        auto start = chrono::steady_clock::now();
        cv::HoughLinesP(frame_Canny, lines, 1, CV_PI / 180, 100, 20, 5);
        auto end = chrono::steady_clock::now();
        auto diff = end - start;
        auto time = chrono::duration<double, milli>(diff).count();

        statistic_txt << "Lines number\t" << lines.size() << "\tTime\t" << time << endl;
        for (const auto& line : lines) {
            cv::line(frame, cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
        }
        imshow("Hough", frame);
        writer << frame;

        if (cv::waitKey(30) == 27)  break;// 'esc'
    }
}

int main(int argc, char** argv) {
    //hough_lines("task2_3/Movement_01.mp4", "task2_3/hough_lines_Movement_01.mp4", "task2_3/hough_lines_stat.txt");
    hough_linesP("task2_3/Movement_01.mp4", "task2_3/hough_linesP_Movement_01.mp4", "task2_3/hough_linesP_stat.txt");
 }
