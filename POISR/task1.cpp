#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>

using namespace std;
int g_slider_position = 0;
int g_run = 1, g_dontset = 0; // начинаем в режиме покадрового просмотра
cv::VideoCapture g_cap;
cv::VideoCapture g_gray;
cv::VideoCapture g_canny;

void onTrackbarSlide(int pos, void*) {
	g_cap.set(cv::CAP_PROP_POS_FRAMES, pos);
	if (!g_dontset)
		g_run = 1;
	g_dontset = 0;
}

int main(int argc, char** argv) {
	cv::namedWindow("source", cv::WINDOW_NORMAL);
	cv::namedWindow("Canny", cv::WINDOW_NORMAL);
	g_cap.open("C:/Users/USER/Videos/video.mp4");
	int frames = (int)g_cap.get(cv::CAP_PROP_FRAME_COUNT);
	int tmpw = (int)g_cap.get(cv::CAP_PROP_FRAME_WIDTH);
	int tmph = (int)g_cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	cout << "¬ виде " << frames << " кадров размером("
		<< tmpw << ", " << tmph << ")." << endl;
	cv::createTrackbar("Position", "source", &g_slider_position, frames,
		onTrackbarSlide);
	cv::Mat frame;
	for (;;) {
		if (g_run != 0) {
			g_cap >> frame; if (frame.empty()) break;
			int current_pos = (int)g_cap.get(cv::CAP_PROP_POS_FRAMES);
			g_dontset = 1;
			cv::setTrackbarPos("Position", "source", current_pos);
			cv::imshow("source", frame);
			cv::resizeWindow("source", 500, 500);
			cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
			cv::Canny(frame, frame, 10, 100, 3, true);
			cv::imshow("Canny", frame);
			cv::resizeWindow("Canny", 500, 500);
			g_run -= 1;
		}
		char c = (char)cv::waitKey(10);
		if (c == 's') // покадровый режим
		{
			g_run = 1; cout << "ѕокадровый режим, run = " << g_run << endl;
		}
		if (c == 'r') // непрерывный режим
		{
			g_run = -1; cout << "Ќепрерывный режим, run = " << g_run << endl;
		}
		if (c == 27)
			break;
	}
	return(0);
}
