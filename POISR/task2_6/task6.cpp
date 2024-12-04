#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
#include <cmath>
#include <random>

using namespace std;


struct Params {
    double pyrScale; // отношение масштабов уровней пирамиды (< 1.0)
    int levels; // число уровней пирамиды
    int winsize; // размер окна на этапе предварительного сглаживания
    int iterations; // число итераций на каждом уровне пирамиды
    int polyN; // размер окрестности для аппроксимации полиномом
    double polySigma; // стандартное отклонение ядра Гаусса для вычисления
    int flags; // флаги режимов вычислений
};

Params params1{ 0.5, 3, 15, 3, 5, 1.2, 0 };
Params params2{ 0.5, 1, 15, 3, 5, 1.2, 0 };
Params params3{ 0.5, 1, 15, 1, 5, 1.2, 0 };

namespace cv {
void flowToColor(const cv::Mat& flow, cv::Mat& color) {
    // visualization
    cv::Mat flow_parts[2];
    split(flow, flow_parts);
    cv::Mat magnitude, angle, magn_norm;
    cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);
    normalize(magnitude, magn_norm, 0.0f, 1.0f, cv::NORM_MINMAX);
    angle *= ((1.f / 360.f) * (180.f / 255.f));
    //build hsv image
    cv::Mat _hsv[3], hsv, hsv8;
    _hsv[0] = angle;
    _hsv[1] = cv::Mat::ones(angle.size(), CV_32F);
    _hsv[2] = magn_norm;
    merge(_hsv, 3, hsv);
    hsv.convertTo(hsv8, CV_8U, 255.0);
    cvtColor(hsv8, color, cv::COLOR_HSV2BGR);
}

void drawOpticalFlow(const Mat& flow, Mat& output, int step = 20) {
    output = Mat::zeros(flow.size(), CV_8UC3);
    for (int y = 0; y < flow.rows; y += step) {
        for (int x = 0; x < flow.cols; x += step) {
            const Point2f& vec = flow.at<Point2f>(y, x);
            Point2f endpoint(x + vec.x, y + vec.y);
            line(output, Point(x, y), endpoint, Scalar(0, 255, 0), 1);
            circle(output, endpoint, 2, Scalar(0, 0, 255), -1);
        }
    }
}
}// namespace cv
static void get_frame(const string in_path, const string out_vector_path, const string out_color_path, const string out_txt_path, Params& params) {
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
    cv::VideoWriter writer_vector;
    writer_vector.open(out_vector_path, cv::VideoWriter::fourcc('H', '2', '6', '4'), fps, size);

    cv::VideoWriter writer_color;
    writer_color.open(out_color_path, cv::VideoWriter::fourcc('H', '2', '6', '4'), fps, size);

    ofstream timingFile(out_txt_path);

    namedWindow("Src", cv::WINDOW_NORMAL);
    cv::Mat frame, frame_prev, flow, flow_color, flow_vectors;
    int frame_count = 0;
    cap >> frame_prev;
    cv::cvtColor(frame_prev, frame_prev, cv::COLOR_BGR2GRAY);
    while (true)
    {
        cap >> frame; if (frame.empty()) break;
        frame_count++;
        
        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);

        auto start = chrono::high_resolution_clock::now();
        cv::calcOpticalFlowFarneback(frame_prev, frame, flow, params.pyrScale,
            params.levels, params.winsize, params.iterations, params.polyN,
            params.polySigma, params.flags);
        auto end = chrono::high_resolution_clock::now();
        double elapsedTime = chrono::duration_cast<chrono::milliseconds>(end - start).count();

        timingFile << frame_count << "\t" << elapsedTime << endl;
        flowToColor(flow, flow_color);
        writer_color << flow_color;
        imshow("Src", flow_color);

        drawOpticalFlow(flow, flow_vectors);
        writer_vector << flow_vectors;
        //imshow("Src", flow_vectors);
        cout << "frame number = " << frame_count << endl;
        frame_prev = frame.clone();

        if (cv::waitKey(30) == 27)  break;// 'esc'
    }
}



int main(int argc, char** argv) {
    string in_path = "task2_6/road.mp4";

    string out_vector_path = "task2_6/vector_road_params1.mp4";
    string out_color_path = "task2_6/color_road_params1.mp4";
    string out_txt_path = "task2_6/road_params1.txt";
    get_frame(in_path, out_vector_path, out_color_path, out_txt_path, params1);

    out_vector_path = "task2_6/vector_road_params2.mp4";
    out_color_path = "task2_6/color_road_params2.mp4";
    out_txt_path = "task2_6/road_params2.txt";
    get_frame(in_path, out_vector_path, out_color_path, out_txt_path, params2);

    out_vector_path = "task2_6/vector_road_params3.mp4";
    out_color_path = "task2_6/color_road_params3.mp4";
    out_txt_path = "task2_6/road_params3.txt";
    get_frame(in_path, out_vector_path, out_color_path, out_txt_path, params3);



    in_path = "task2_6/surface.mp4";

    out_vector_path = "task2_6/vector_surface_params1.mp4";
    out_color_path = "task2_6/color_surface_params1.mp4";
    out_txt_path = "task2_6/surface_params1.txt";
    get_frame(in_path, out_vector_path, out_color_path, out_txt_path, params1);

    out_vector_path = "task2_6/vector_surface_params2.mp4";
    out_color_path = "task2_6/color_surface_params2.mp4";
    out_txt_path = "task2_6/surface_params2.txt";
    get_frame(in_path, out_vector_path, out_color_path, out_txt_path, params2);

    out_vector_path = "task2_6/vector_surface_params3.mp4";
    out_color_path = "task2_6/color_surface_params3.mp4";
    out_txt_path = "task2_6/surface_params3.txt";
    get_frame(in_path, out_vector_path, out_color_path, out_txt_path, params3);

}