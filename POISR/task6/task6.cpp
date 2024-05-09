#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;
int g_slider_position = 0;
int g_run = 1, g_dontset = 0;
cv::VideoCapture g_cap;
vector<int> corners_number;

void onTrackbarSlide(int pos, void*) {
	g_cap.set(cv::CAP_PROP_POS_FRAMES, pos);
	if (!g_dontset)
		g_run = 1;
	g_dontset = 0;
}


double source_video() {
	cv::namedWindow("source", cv::WINDOW_NORMAL);
	//cv::namedWindow("Canny", cv::WINDOW_NORMAL);

	double total_time = 0;
	int count = 0;
	g_cap.open("C:/Users/USER/Videos/video2.mp4");
	//g_cap.open(0);
	double fps = g_cap.get(cv::CAP_PROP_FPS);
	cv::Size size(
		(int)g_cap.get(cv::CAP_PROP_FRAME_WIDTH),
		(int)g_cap.get(cv::CAP_PROP_FRAME_HEIGHT)
	);
	cv::VideoWriter writer;
	writer.open("task6/video_task6_circles_1.mp4", cv::VideoWriter::fourcc('H', '2', '6', '4'), fps, size);
	int frames = (int)g_cap.get(cv::CAP_PROP_FRAME_COUNT);
	int tmpw = (int)g_cap.get(cv::CAP_PROP_FRAME_WIDTH);
	int tmph = (int)g_cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	//cout << "In the form of " << frames << " frames in size("
		//<< tmpw << ", " << tmph << ")." << endl;
	cv::createTrackbar("Position", "source", &g_slider_position, frames,
		onTrackbarSlide);
	cv::Mat frame;
	cv::Mat frame_gray;
	cv::Mat frame_Canny;

	cv::Mat frame_roi;
	cv::Mat frame_roi_prev;

	cv::Mat frame_gray_prev;
	vector<vector<cv::Point>> edges;

	//параметры goodFeaturesToTrack
	vector<cv::Point2f> corners;
	vector<cv::Point2f> corners_prev;


	//cv::Mat corners;
	int maxCorners = 1000;
	double qualityLevel = 0.05;
	double minDistance = 3;
	int blockSize = 3;

	//начальные приближения для поиска roi, prev_point_up не используется
	int prev_point_up = 0;
	int prev_point_down = 600;

	vector<uchar> status;
	vector<float> err;
	cv::TermCriteria criteria = cv::TermCriteria((cv::TermCriteria::COUNT)+(cv::TermCriteria::EPS), 10, 0.3);

	vector<vector<cv::Point2f>> trajects(maxCorners);


	bool flag = 1;
	for (;;) {
		if (g_run != 0) {
			count++;
			g_cap >> frame; if (frame.empty()) break;

			int current_pos = (int)g_cap.get(cv::CAP_PROP_POS_FRAMES);
			g_dontset = 1;
			cv::setTrackbarPos("Position", "source", current_pos);


			cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
			cv::Canny(frame_gray, frame_Canny, 130, 230, 3, true);

			std::cout << "framefaesf" << frame.size() << std::endl;
			//roi params
			int X0 = 0;
			int Y0 = 0;
			int X = 0;
			int Y = 0;

			cv::Point c1(0, prev_point_down), c2(1600, prev_point_down);
			cv::Point c3(0, prev_point_up), c4(1600, prev_point_up);
			cv::Point c5(0, prev_point_up), c6(0, prev_point_down);
			cv::Point c7(1599, prev_point_up), c8(1599, prev_point_down);

			int thickness = 2;
			cv::line(frame, c1, c2, cv::Scalar(0, 255, 0),
				thickness, cv::LINE_8);
			cv::line(frame, c3, c4, cv::Scalar(0, 255, 0),
				thickness, cv::LINE_8);
			cv::line(frame, c5, c6, cv::Scalar(0, 255, 0),
				thickness, cv::LINE_8);
			cv::line(frame, c7, c8, cv::Scalar(0, 255, 0),
				thickness, cv::LINE_8);

			cv::Rect roi(0, prev_point_up, 1599, prev_point_down - prev_point_up);//x0, y0, width, height

			cv::equalizeHist(frame_gray, frame_gray);
			frame_roi = frame_gray(roi);

			if (flag) {
				frame_gray_prev = frame_gray.clone();
				cv::goodFeaturesToTrack(frame_roi, corners_prev, maxCorners, qualityLevel, minDistance, cv::noArray(), blockSize, true);
				int radius = 10;
				int thickness_circle = 5;
				for (size_t i = 0; i < corners_prev.size(); i++)
				{
					corners_prev[i].y += prev_point_up;
					trajects[i].push_back(corners_prev[i]);
					cv::circle(frame, corners_prev[i], radius, cv::Scalar(0, 255, 255), thickness_circle);
				}
				//trajects.push_back(corners_prev);
				corners_number.push_back(corners_prev.size());
				std::cout << "corners = " << corners_prev.size() << std::endl;
				flag = false;
			}
			else {
				vector<cv::Point2f> corners_good;
				cv::calcOpticalFlowPyrLK(frame_gray_prev, frame_gray, corners_prev, corners, status, err, cv::Size(21, 21), 3, criteria);
				//trajects.push_back(corners);

				//select and vizual
				int radius = 10;
				int thickness_circle = 5;
				auto iter = trajects.cbegin();
				auto iter_end = trajects.cend();

				vector<vector<cv::Point2f>> trajects_temp;
				//trajects_temp = trajects;
				for (uint i = 0; i < status.size(); i++)
				{
					if (status[i] && (corners[i].x < 1599 && corners[i].x > 0 && corners[i].y > prev_point_up && corners[i].y < prev_point_down)) {
						corners_good.push_back(corners[i]);

						trajects[i].push_back(corners[i]);
						trajects_temp.push_back(trajects[i]);

						cv::circle(frame, corners[i], radius, cv::Scalar(0, 255, 255), thickness_circle);

						/*for (int j = 1; j < trajects[i].size(); j++) {
							cv::line(frame, trajects[i][j-1], trajects[i][j], cv::Scalar(255, 255, 0), 3);
						}*/
					}



				}
				//trajects.clear();
				trajects = trajects_temp;

				//upd
				frame_gray_prev = frame_gray.clone();
				corners_prev = corners_good;

				std::cout << "corners = " << corners.size() << std::endl;

				corners_number.push_back(corners.size());
			}

			cv::imshow("source", frame);

			writer << frame;
			g_run -= 1;
			
		}
		char c = (char)cv::waitKey(10);
		if (c == 's')
		{
			g_run = 1; cout << "Frame-by-frame mode, run = " << g_run << endl;
		}
		if (c == 'r')
		{
			g_run = -1; cout << "Continuos mode, run = " << g_run << endl;
		}
		if (c == 27)
			return total_time / count;
	}
	//g_cap.release();
	return total_time / count;
}

int main(int argc, char** argv) {
	source_video();

	std::ofstream out;
	out.open("task6/task6_1.txt");
	if (out.is_open())
	{
		for (int i = 0; i < corners_number.size(); i++)
		{
			out << i << " " << corners_number[i] << std::endl;
		}

	}
	out.close();
	std::cout << "File has been written" << std::endl;
}