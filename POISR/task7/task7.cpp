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
		g_run = -1;
	g_dontset = 0;
}


double source_video() {
	cv::namedWindow("source", cv::WINDOW_NORMAL);
	//cv::namedWindow("Canny", cv::WINDOW_NORMAL);

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
	writer.open("task7/video_task7_video1.mp4", cv::VideoWriter::fourcc('H', '2', '6', '4'), fps, size);
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
	

	//roi params
	int X0 = 200;
	int Y0 = 270;
	int width = 1200;
	int height = 500;

	cv::Point c1(X0, Y0), c2(X0 + width, Y0); //up line
	cv::Point c3(X0, Y0 + height), c4(X0 + width, Y0 + height);//down line
	cv::Point c5(X0, Y0), c6(X0, Y0 + height); // left line
	cv::Point c7(X0 + width, Y0), c8(X0 + width, Y0 + height); // right line

	//SIFT
	cv::Ptr<cv::SIFT> siftPtr = cv::SIFT::create();
	cv::Mat descriptors;
	cv::Mat descriptors_prev;
	std::vector<cv::KeyPoint> keypoints;
	std::vector<cv::KeyPoint> keypoints_match;
	std::vector< std::vector<cv::DMatch> > keyp_match;
	cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);

	g_cap >> frame;
	siftPtr->detectAndCompute(frame, cv::noArray(), keypoints, descriptors_prev);
	cv::drawKeypoints(frame, keypoints, frame, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DEFAULT);

	for (;;) {
		if (g_run != 0) {
			count++;
			g_cap >> frame; if (frame.empty()) break;



			int current_pos = (int)g_cap.get(cv::CAP_PROP_POS_FRAMES);
			g_dontset = 1;
			cv::setTrackbarPos("Position", "source", current_pos);


			siftPtr->detectAndCompute(frame, cv::noArray(), keypoints, descriptors);

			matcher->knnMatch(descriptors, descriptors_prev, keyp_match, 2);

			for (size_t i = 0; i < keyp_match.size(); i++) {
				//keypoints_match.push_back(keyp_match[i][0]);
			}
			

			
			//siftPtr->detectAndCompute(frame, cv::noArray(), keypoints, descriptors);
			cv::drawKeypoints(frame, keypoints, frame, cv::Scalar(0,0,255),cv::DrawMatchesFlags::DEFAULT);


			

			cv::imshow("source", frame);

			//writer << frame;
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

	/*
	std::ofstream out;
	
	out.open("task7/taskasdasd.txt");
	if (out.is_open())
	{
		for (int i = 0; i < corners_number.size(); i++)
		{
			out << i << " " << corners_number[i] << std::endl;
		}

	}
	out.close();
	std::cout << "File has been written" << std::endl;*/
}