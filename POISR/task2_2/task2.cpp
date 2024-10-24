#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;

std::vector<cv::Point3f> get_object_points(size_t obj_size, size_t chessboard_width, size_t chessboard_height) {
    std::vector<cv::Point3f> tmp;
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 7; ++j) {
            tmp.push_back(cv::Point3f(i * 5, j * 5, 0));
            //cout << cv::Point3f(j * 5, i * 5, 0) << endl;

        }
    }
    //sort(tmp.begin(), tmp.end(), [](const cv::Point3f& a, const cv::Point3f& b) { return (a.x < b.x) ? true : (a.x > b.x) ? false : (a.y < b.y); });
    return tmp;
}
ostream& operator<< (ostream& out, const std::vector<cv::Point2f>& arr) {
    out << endl;
    for (int i = 0; i < arr.size(); i++) {
        if (i % 7 == 0) out << "\n";
        out << arr[i] << '\t';
    }
    out << endl;
    return out;
}

void sort_corners(std::vector<cv::Point2f>& corners) {
    std::vector<cv::Point2f> tmp = corners;
    float minimun = INFINITY;
    for (const auto& corner : tmp) {
        if (corner.x < minimun) {
            int oi = 0;
        }
    }
}
void source_video(string in_path, string out_path, string out_corners_txt_path, string out_cparams_txt_path) {
    cv::VideoCapture cap;
    cap.open(in_path);

    if (!cap.isOpened())
    {
        std::cout << "Cannot open the video file. \n";
    }

    double fps = cap.get(cv::CAP_PROP_FPS);
    cv::Size size(
        (int)cap.get(cv::CAP_PROP_FRAME_WIDTH),
        (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT)
    );

    cv::VideoWriter writer;
    //writer.open(out_path, cv::VideoWriter::fourcc('H', '2', '6', '4'), fps, size);

    namedWindow("Calibration", cv::WINDOW_NORMAL);
    cv::Mat frame, gray;
    int frame_count = 0;
    std::vector<cv::Point2f> corners;
    std::vector< std::vector<cv::Point2f> > all_corners;
    std::vector< std::vector<cv::Point3f> > object_points;
    bool chess_find_flag;
    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.001);
    while (true)
    {
        frame_count++;
        cap >> frame; if (frame.empty()) break;
        if (frame_count % 10 == 0) {
            cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
            chess_find_flag = cv::findChessboardCorners(gray, cv::Size(7, 7), corners, cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);
            if (chess_find_flag) {
                //sort(corners.begin(), corners.end(), [](const cv::Point& a, const cv::Point& b) { return (a.x < b.x) ? true : (a.x > b.x) ? false : (a.y < b.y); });
                cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1), criteria);
                object_points.push_back(get_object_points(5, 8, 8));
                all_corners.push_back(corners);
                cv::drawChessboardCorners(gray, cv::Size(7, 7), corners, chess_find_flag);
            }
            imshow("Calibration", gray);
        }

        if (cv::waitKey(30) == 27)  break;// 'esc' // || frame_count == 5
    }



    cv::Mat cameraMatrix, distCoeffs;
    std::vector<cv::Mat> rvecs, tvecs;

    std::cout << "Количество кадров: " << frame_count << endl;
    std::cout << "Запись углов" << endl;
    std::ofstream out_corners;
    out_corners.open(out_corners_txt_path);
    for (const auto& corns : all_corners) {
        out_corners << corns;
    }
    out_corners.close();
    std::cout << "Калибровка" << endl;
    cv::calibrateCamera(
        object_points,
        all_corners,
        gray.size(),
        cameraMatrix,
        distCoeffs,
        rvecs,
        tvecs
    );



    std::ofstream out_params;
    out_params.open(out_cparams_txt_path);

    out_params << "Фокусное расстояние " << cameraMatrix.at<double>(0, 0) << ", " << cameraMatrix.at<double>(1, 1) << endl;
    out_params << "Главные точки " << cameraMatrix.at<double>(0, 2) << ", " << cameraMatrix.at<double>(1, 2) << endl;
    out_params << "Коэффициенты дисторсии " << distCoeffs << endl;
    out_params.close();
    cap.release();
    cv::destroyAllWindows();
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "Russian");
    source_video("task2_2/Calibration_01.mp4", "task2_2/corners_Calibration_01.mp4", "task2_2/corners_Calibration_01.txt", "task2_2/params_Calibration_01.txt");
    source_video("task2_2/Calibration_02.mp4", "task2_2/corners_Calibration_02.mp4", "task2_2/corners_Calibration_02.txt", "task2_2/params_Calibration_02.txt");
}
