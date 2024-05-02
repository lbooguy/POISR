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
	writer.open("video_Canny_source.mp4", cv::VideoWriter::fourcc('H', '2', '6', '4'), fps, size);
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
			cv::drawContours(frame, edges, -1, (157, 155, 0), 3);
			cv::imshow("source", frame);
			
			
			
			cv::imshow("Canny", frame_Canny);

		
			//std::cout << "f  =" << frame_gray.at<uchar>(cv::Point(0, 0)) << std::endl;
			f.push_back(frame_gray);
			
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
	std::cout <<"Source video: Average time processing of frame  =" <<source_video()<<"ms" << std::endl;


	/*
	for (int row = 0; row < f[0].rows; ++row) {
		uchar* p = f[0].ptr(row); //pointer p points to the first place of each row
		for (int col = 0; col < f[0].cols; ++col) {
			*p++;
			if (p[row] != 0) {
			}

		}
	}*/
	std::ofstream out;  
	out.open("pix.txt");
	
	//std::cout << "f  =" << f[0].data << std::endl;
	for (int i = 0; i < f[0].rows; i++) {
		for (int j = 0; j < f[0].cols; j++) {
			if (f[0].at<uchar>(i, j) == 0) {
				if (out.is_open())
				{
					out << int(f[0].at<uchar>(i, j)) <<'|' << i << ',' << j << std::endl;
				}
			}
		}
	}

}