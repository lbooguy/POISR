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

static ostream& operator<< (ostream& out, const std::vector<cv::Point2f>& arr) {
    out << endl;
    for (int i = 0; i < arr.size(); i++) {
        if (i % 7 == 0) out << "\n";
        out << arr[i] << '\t';
    }
    out << endl;
    return out;
}

static cv::Point3f operator*(const cv::Mat& a, const cv::Point3f& b)
{
    double number1 = a.at<double>(0, 0) * b.x + a.at<double>(0, 1) * b.y + a.at<double>(0, 2) * b.z;
    double number2 = a.at<double>(1, 0) * b.x + a.at<double>(1, 1) * b.y + a.at<double>(1, 2) * b.z;
    //double number3 = a.at<double>(2, 0) * b.x + a.at<double>(2, 1) * b.y + a.at<double>(2, 2) * b.z;
    double number3 = 0; //?
    return cv::Point3f(number1, number2, number3);
}

static cv::Point3f operator*(const std::vector<vector<double>>& a, const cv::Point3f& b)
{
    double number1 = a[0][0] * b.x + a[0][1] * b.y + a[0][2] * b.z;
    double number2 = a[1][0] * b.x + a[1][1] * b.y + a[1][2] * b.z;
    //double number3 = a.at<double>(2, 0) * b.x + a.at<double>(2, 1) * b.y + a.at<double>(2, 2) * b.z;
    double number3 = 0; //?
    return cv::Point3f(number1, number2, number3);
}
static cv::Point2f operator-(const cv::Point3f& first_operand, const cv::Point3f& second_operand)
{
    return cv::Point2f(first_operand.x - second_operand.x, first_operand.y - second_operand.y);
}

template<typename T> void print(const T& out) {
    std::cout << out << std::endl;
}

static std::vector<cv::Point2f> get_object_points(size_t obj_size, size_t chessboard_width, size_t chessboard_height) {
    std::vector<cv::Point2f> tmp;
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 7; ++j) {
            tmp.push_back(cv::Point2f(j * 5 * 10, i * 5 * 10));
        }
    }
    return tmp;
}

static void get_frame(const string in_path, const string out_path) {
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

    namedWindow("Src", cv::WINDOW_NORMAL);
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
        imshow("Src", frame);

        if (cv::waitKey(30) == 27)  break;// 'esc'
    }
}

static std::vector<cv::Point2f> get_sorted_line(std::vector<cv::Point2f>& corners, int idx_begin, int idx_end) {
    std::vector<cv::Point2f> line;
    for (int i = idx_begin; i < idx_end; i++) {
        line.push_back(corners[i]);
    }
    sort(line.begin(), line.end(), [](const cv::Point& a, const cv::Point& b) { return (a.x < b.x) ? true : (a.x > b.x) ? false : (a.y < b.y); });
    return line;
}

static void sort_corners(std::vector<cv::Point2f>& corners) {
    std::vector<cv::Point2f> new_corners;

    if (corners[0].y < corners[7].y) {
        for (int i = 0; i < 7; i++) {
            cout << i << endl;
            std::vector<cv::Point2f> tmp = get_sorted_line(corners, i * 7, (i + 1) * 7);
            for (const auto& point : tmp) {
                new_corners.push_back(point);
            }
        }
    }
    else if (corners[0].y > corners[7].y) {
        for (int i = 6; i > -1; i--) {
            cout << i << endl;
            std::vector<cv::Point2f> tmp = get_sorted_line(corners, i * 7, (i + 1) * 7);
            for (const auto& point : tmp) {
                new_corners.push_back(point);
            }
        }
    }
    corners = new_corners;


}

static void get_projective_transform(string chess_image_path, cv::Mat& projective_transform, string projective_transform_txt, bool cornersIsSorted = false) {

    cv::Mat image = cv::imread(chess_image_path);
    cv::Mat gray;
    cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    std::vector<cv::Point2f> corners;
    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.001);

    auto obj_points = get_object_points(5, 8, 8);
    bool chess_find_flag =
        cv::findChessboardCorners(gray,
            cv::Size(7, 7),
            corners,
            cv::CALIB_CB_ADAPTIVE_THRESH +
            cv::CALIB_CB_NORMALIZE_IMAGE +
            cv::CALIB_CB_FAST_CHECK);


    cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1), criteria);
    if (cornersIsSorted == true) {
        sort_corners(corners);
    }
    cv::drawChessboardCorners(image, cv::Size(7, 7), corners, chess_find_flag);

    imwrite("task2_4/corners.png", image);

    std::cout << corners << std::endl;
    std::cout << obj_points << std::endl;

    cv::Mat transform = cv::findHomography(corners, obj_points, 0);
    projective_transform = transform;

    std::ofstream transform_txt;
    transform_txt.open(projective_transform_txt);
    transform_txt << transform << endl;
    transform_txt.close();
}

static float distance(cv::Point3f& point_1, cv::Point3f& point_2) {
    return pow(pow(point_1.x - point_2.x, 2) + pow(point_1.y - point_2.y, 2), 0.5);// + pow(point_1.z - point_2.z, 2)
}

static float vector_lenght(cv::Point3f& vector) {
    return pow(pow(vector.x, 2) + pow(vector.y, 2), 0.5);// + pow(point_1.z - point_2.z, 2)
}
static float vector_lenght(const cv::Point2f& vector) {
    return pow(pow(vector.x, 2) + pow(vector.y, 2), 0.5);// + pow(point_1.z - point_2.z, 2)
}
static void check_transform(cv::Mat& projective_transform, cv::Point2f point1, cv::Point2f point2) {
    cv::Point3f point_1 = projective_transform * cv::Point3f(point1.x, point1.y, 1);
    cv::Point3f point_2 = projective_transform * cv::Point3f(point2.x, point2.y, 1);

    std::cout << "Transform:\n" << projective_transform << std::endl;
    cout << endl;
    cout << "Point 1 = " << point_1.x << ",\t" << point_1.y << ",\t" << point_1.z << endl;
    cout << "Point 2 = " << point_2.x << ",\t" << point_2.y << ",\t" << point_2.z << endl;
    float dist = distance(point_1, point_2);//pow( pow(point_1.x - point_2.x, 2) + pow(point_1.y - point_2.y, 2), 0.5);
    print("Distance = " + std::to_string(dist));

}

static void get_query_inds(std::set <int>& query_inds, std::vector < std::vector< std::vector<cv::DMatch>>>& keyp_match_trajectory, int index) {
    query_inds.clear();
    for (const auto& match : keyp_match_trajectory[index]) {
        if (match[0].distance < 0.7 * match[1].distance) {
            query_inds.insert(match[0].queryIdx);
        }
    }
}

static void draw_line(cv::Mat& frame, std::vector<cv::KeyPoint>& keypoints_prev, std::vector<cv::KeyPoint>& keypoints, int query_idx, int train_idx) {
    cv::Point2f pt1 = keypoints_prev[query_idx].pt;
    cv::Point2f pt2 = keypoints[train_idx].pt;
    cv::line(frame, pt1, pt2, cv::Scalar(0, 255, 0), 2);
}

static void draw_lines(cv::Mat& frame, std::vector<std::vector<cv::KeyPoint>>& keypoints_trajectory, std::vector < std::vector< std::vector<cv::DMatch>>>& keyp_match_trajectory) {
    std::set <int> query_inds;
    std::set <int> new_query_inds;
    get_query_inds(query_inds, keyp_match_trajectory, keyp_match_trajectory.size() - 1);

    for (int i = keyp_match_trajectory.size() - 2; i >= 0; i--) {
        //get_query_inds(query_inds, keyp_match_trajectory, i + 1);
        for (const auto& match : keyp_match_trajectory[i]) {
            if ((match[0].distance < 0.7 * match[1].distance) and (query_inds.find(match[0].trainIdx) != query_inds.end())) {
                new_query_inds.insert(match[0].queryIdx);
                draw_line(frame, keypoints_trajectory[i], keypoints_trajectory[i + 1], match[0].queryIdx, match[0].trainIdx);
            }
        }
        query_inds = new_query_inds;
        new_query_inds.clear();
        //get_query_inds(query_inds, keyp_match_trajectory, keyp_match_trajectory.size() - 1);
    }
}

static cv::Point2d averaging_vectors(std::vector<cv::Point3f>& vectors) {
    cv::Point2d average_vector(0, 0);
    int count = 0;
    for (size_t i = 0; i < vectors.size(); i++) {
        average_vector.x += vectors[i].x;
        average_vector.y += vectors[i].y;
        count++;
  
    }
    average_vector.x /= count;
    average_vector.y /= count;
    return average_vector;
}

static int get_inliers_number(cv::Point2f& model_vector, std::vector<float>& offsets, std::vector<cv::Point2f>& offset_vectors, std::vector<cv::Point2f>& best_inliers, float angle_thresgold, float lenght_threshold) {
    float model_vector_angle = atan2(model_vector.y, model_vector.x) * 180 / CV_PI;
    float model_vector_lenght = pow(model_vector.y * model_vector.y + model_vector.x * model_vector.x, 0.5);
    best_inliers.clear();
    int inliers_count = 0;
    for (size_t i = 0; i < offsets.size(); i++) {
        float maybe_inlier_angle = atan2(offset_vectors[i].y, offset_vectors[i].x) * 180 / CV_PI;
        float maybe_inlier_lenght = offsets[i];
        if (abs(maybe_inlier_angle - model_vector_angle) < angle_thresgold && abs(maybe_inlier_lenght - model_vector_lenght) < lenght_threshold) {
            best_inliers.push_back(offset_vectors[i]);
            inliers_count++;
        }
    }
    return inliers_count;
}
namespace cv{
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
}
static void MatToVector(cv::Mat& mat, std::vector<cv::Point2f>& vector) {
    for (int i = 0; i < mat.rows; i++) {
        for (int j = 0; j < mat.cols; j++) {
            
            const cv::Point2f point = mat.at<cv::Point2f>(i, j);
            if (point.x != 0 and point.y != 0 and vector_lenght(point) > 0.05) {
                vector.push_back(point);
            }
        }
    }

}
static void apply_transform(string movement_video_path, std::vector<vector<double>>& transform, string stats_txt_path, Params& params) {
    cv::VideoCapture cap;
    cap.open(movement_video_path);

    if (!cap.isOpened())
    {
        std::cout << "Cannot open the video file. \n";
    }

    double fps = cap.get(cv::CAP_PROP_FPS);
    cv::Size size(
        (int)cap.get(cv::CAP_PROP_FRAME_WIDTH),
        (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT)
    );

    namedWindow("Movement", cv::WINDOW_NORMAL);

    int frame_count = 0;
   
    float prev_average_vector_angle = 0;
    std::ofstream stats;
    stats.open(stats_txt_path);

    cv::Mat frame, frame_prev, flow_mat;
    std::vector<cv::Point2f> flow;

    cv::Point2f trajectory(0, 0);

    while (true)
    {   
        if (frame_count == 0) {
            cap >> frame;
            cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
            frame_prev = frame.clone();
        }
        frame_count++;
        cap >> frame; if (frame.empty()) break;
        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);

        auto begin_TOTAL = std::chrono::steady_clock::now();

        cv::calcOpticalFlowFarneback(frame_prev, frame, flow_mat, params.pyrScale,
            params.levels, params.winsize, params.iterations, params.polyN,
            params.polySigma, params.flags);
        frame_prev = frame.clone();

        MatToVector(flow_mat, flow);
        //drawOpticalFlow(flow_mat, frame);
        //imshow("Movement", frame);
        std::vector<cv::Point3f> metric_flow;
        for (const auto& fl : flow) {       
            cv::Point3f metric_fl = transform * cv::Point3f(fl.x, fl.y, 1);
            metric_flow.push_back(metric_fl);
        }
        
        cv::Point2d average_vector = averaging_vectors(metric_flow);
        trajectory.x += average_vector.x;
        trajectory.y += average_vector.y;
        auto end_TOTAL = std::chrono::steady_clock::now();
        auto TOTAL_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_TOTAL - begin_TOTAL);

        float average_vector_angle = atan2(average_vector.y, average_vector.x) * 180 / CV_PI;
        if (frame_count == 1) {
            prev_average_vector_angle = atan2(average_vector.y, average_vector.x) * 180 / CV_PI;
        }


        if (frame_count > 2) {
            float average_vector_len = pow(average_vector.x * average_vector.x + average_vector.y * average_vector.y, 0.5);
            float average_vector_angle = atan2(average_vector.y, average_vector.x) * 180 / CV_PI;

            stats << frame_count << " " <<
                average_vector_len << " " <<
                average_vector_angle - prev_average_vector_angle << " " <<
                trajectory.x << " " <<
                trajectory.y << " " <<
                TOTAL_time.count() << endl;

            prev_average_vector_angle = average_vector_angle;
        }

        //frame_prev = frame.clone();
        if (cv::waitKey(30) == 27)  break;// 'esc'
    }
    stats.close();
}

int main(int argc, char** argv) {

    const string stats_txt_path = "task2_7/stats.txt";
    const string movement_video_path = "task2_7/Movement_01.mp4";

    std::vector<vector<double>> projective_transform = { { -1.100642098326805, 0.06525565846120168, 1137.186308938773},
                                                         {-0.003354742110148203, -2.253588178675616, 3545.491365823719 },
                                                         {1.884591680671522e-05, -0.001283004573461808, 1 }};
    apply_transform(movement_video_path, projective_transform, stats_txt_path, params1);

}
