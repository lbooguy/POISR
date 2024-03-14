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

double source_size() {
	cv::namedWindow("source", cv::WINDOW_NORMAL);
	cv::namedWindow("Canny", cv::WINDOW_NORMAL);
	time_t start, end;
	double average_time = 0;
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
	writer.open("video_Canny_source.mp4", cv::VideoWriter::fourcc('H', '2', '6', '4'), fps, size);
	int frames = (int)g_cap.get(cv::CAP_PROP_FRAME_COUNT);
	int tmpw = (int)g_cap.get(cv::CAP_PROP_FRAME_WIDTH);
	int tmph = (int)g_cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	//cout << "In the form of " << frames << " frames in size("
		//<< tmpw << ", " << tmph << ")." << endl;
	cv::createTrackbar("Position", "source", &g_slider_position, frames,
		onTrackbarSlide);
	cv::Mat frame;
	for (;;) {
		if (g_run != 0) {
			count++;
			g_cap >> frame; if (frame.empty()) break;
			int current_pos = (int)g_cap.get(cv::CAP_PROP_POS_FRAMES);
			g_dontset = 1;
			cv::setTrackbarPos("Position", "source", current_pos);
			cv::imshow("source", frame);
			cv::resizeWindow("source", 500, 500);
			auto begin = std::chrono::steady_clock::now();
			cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
			cv::Canny(frame, frame, 99, 100, 3, true);
			cv::imshow("Canny", frame);
			cv::resizeWindow("Canny", 500, 500);
			auto end = std::chrono::steady_clock::now();
			auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
			total_time += elapsed_ms.count();
			//std::cout << "total_time=" << elapsed_ms.count() <<" ms" << std::endl;
			//std::cout << "count=" << count << std::endl;
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
			return total_time/count;
	}
	return total_time / count;
}


int main(int argc, char** argv) {
	std::cout<<"Source size: Average time preparation of frame  =" <<source_size()<<"ms" << std::endl;

}