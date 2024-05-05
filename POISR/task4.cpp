#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;
int g_slider_position = 0;
int g_run = 1, g_dontset = 0;
cv::VideoCapture g_cap;

void onTrackbarSlide(int pos, void*) {
	g_cap.set(cv::CAP_PROP_POS_FRAMES, pos);
	if (!g_dontset)
		g_run = 1;
	g_dontset = 0;
}


int find_point(cv::Mat frame_Canny,
	int prev_point,
	int x0, int y0,
	int x1, int y1,
	int mode) {
	if (mode == 1) {
		int step = 10;
		int y_out = 0;
		y0 = prev_point - 10;
		for (int y = y0; y < y1; y = y + step) {
			for (int x = x0; x < x1; x++) {
				auto check = frame_Canny.at<uchar>(y, x);
				if (check != '\0') {
					for (int y_t = y - 10; y_t < y; y_t++) {
						for (int x_t = x0; x_t < x1; x_t++) {
							int check1 = frame_Canny.at<uchar>(y_t, x_t);
							if (check1 != '\0') {
								return y_t;
							}
						}
					}

				}
			}
		}
	}

	if (mode != 1) {
		int step = 10;
		int y_out = 0;
		for (int y = y1; y > y0; y = y - step) {
			for (int x = x0; x < x1; x++) {
				auto check = frame_Canny.at<uchar>(y, x);
				if (check != '\0') {
					for (int y_t = y; y_t > y - 10; y_t--) {
						for (int x_t = x0; x_t < x1; x_t++) {
							int check1 = frame_Canny.at<uchar>(y_t, x_t);
							if (check1 != '\0') {
								return y_t;
							}
						}
					}

				}
			}
		}
	}

}

std::vector<cv::Mat> f;
double source_video() {
	cv::namedWindow("source", cv::WINDOW_NORMAL);
	cv::namedWindow("Canny", cv::WINDOW_NORMAL);
	time_t start, end;
	double total_time = 0;
	int count = 0;
	g_cap.open("C:/Users/USER/Videos/video.mp4");
	//g_cap.open(0);
	double fps = g_cap.get(cv::CAP_PROP_FPS);
	cv::Size size(
		(int)g_cap.get(cv::CAP_PROP_FRAME_WIDTH),
		(int)g_cap.get(cv::CAP_PROP_FRAME_HEIGHT)
	);
	cv::VideoWriter writer;
	writer.open("video_edges.mp4", cv::VideoWriter::fourcc('H', '2', '6', '4'), fps, size);
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
	vector< vector< cv::Point> > edges;
	int prev_point_up = 0;
	int prev_point_down = 600;
	for (;;) {
		if (g_run != 0) {
			count++;
			g_cap >> frame; if (frame.empty()) break;
			int current_pos = (int)g_cap.get(cv::CAP_PROP_POS_FRAMES);
			g_dontset = 1;
			cv::setTrackbarPos("Position", "source", current_pos);
			cv::Point p1(530, 0), p2(530, 1200);
			cv::Point p3(1060, 0), p4(1060, 1200);
			int thickness = 2;
			// Line drawn using 8 connected 
			// Bresenham algorithm 
			/*
			cv::line(frame, p1, p2, cv::Scalar(255, 0, 0),
				thickness, cv::LINE_8);
			cv::line(frame, p3, p4, cv::Scalar(255, 0, 0),
				thickness, cv::LINE_8);
				*/

			cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
			cv::Canny(frame_gray, frame_Canny, 130, 230, 3, true);

			cv::findContours(frame_Canny, edges, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

			cv::drawContours(frame, edges, -1, (0, 0, 255), 3);
			prev_point_down = find_point(frame_Canny,
				prev_point_down,
				530, 600,
				1060, 1200,
				1);
			cv::Point c1(0, prev_point_down), c2(1600, prev_point_down);

			prev_point_up = find_point(frame_Canny,
				prev_point_up,
				530, 0,
				1060, 600,
				0);
			cv::Point c3(0, prev_point_up), c4(1600, prev_point_up);

			cv::line(frame, c1, c2, cv::Scalar(0, 255, 0),
				thickness, cv::LINE_8);
			cv::line(frame, c3, c4, cv::Scalar(0, 255, 0),
				thickness, cv::LINE_8);

			cv::Point c5(0, prev_point_up), c6(0, prev_point_down);
			cv::Point c7(1599, prev_point_up), c8(1599, prev_point_down);
			cv::line(frame, c5, c6, cv::Scalar(0, 255, 0),
				thickness, cv::LINE_8);
			cv::line(frame, c7, c8, cv::Scalar(0, 255, 0),
				thickness, cv::LINE_8);


			cv::imshow("source", frame);
			//cv::imshow("Canny", frame_Canny);
  
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

}