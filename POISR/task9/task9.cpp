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
//vector<int> corners_number;

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
	g_cap.open("C:/Users/USER/Videos/video3.mp4");
	//g_cap.open(0);
	double fps = g_cap.get(cv::CAP_PROP_FPS);
	cv::Size size(
		(int)g_cap.get(cv::CAP_PROP_FRAME_WIDTH),
		(int)g_cap.get(cv::CAP_PROP_FRAME_HEIGHT)
	);
	cv::VideoWriter writer;
	writer.open("task9/video_task9_ORB.mp4", cv::VideoWriter::fourcc('H', '2', '6', '4'), fps, size);
	int frames = (int)g_cap.get(cv::CAP_PROP_FRAME_COUNT);
	int tmpw = (int)g_cap.get(cv::CAP_PROP_FRAME_WIDTH);
	int tmph = (int)g_cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	//cout << "In the form of " << frames << " frames in size("
		//<< tmpw << ", " << tmph << ")." < < endl;
	cv::createTrackbar("Position", "source", &g_slider_position, frames,
		onTrackbarSlide);
	cv::Mat frame;
	cv::Mat frame_gray;
	cv::Mat frame_Canny;
	cv::Mat frame_roi;
	cv::Mat frame_roi_prev;
	cv::Mat frame_gray_prev;

	std::ofstream out;

	out.open("task9/task9_ORB.txt");
	//if (out.is_open())
	//{
		//out << "Frame number, Numbers of detected features, Numbers of matched features, detectAndCompute time ms, knnMatch time ms" << std::endl;
	//}

	//roi params
	int X0 = 200;
	int Y0 = 270;
	int width = 1200;
	int height = 500;

	cv::Point c1(X0, Y0), c2(X0 + width, Y0); //up line
	cv::Point c3(X0, Y0 + height), c4(X0 + width, Y0 + height);//down line
	cv::Point c5(X0, Y0), c6(X0, Y0 + height); // left line
	cv::Point c7(X0 + width, Y0), c8(X0 + width, Y0 + height); // right line

	//ORB
	cv::Mat descriptors;
	cv::Mat descriptors_prev;
	std::vector<cv::KeyPoint> keypoints;
	std::vector<cv::KeyPoint> keypoints_prev;
	std::vector<cv::KeyPoint> keypoints_match;
	//std::vector< std::vector<cv::DMatch> > keyp_match;
	int nfeatures = 500;
	float scaleFactor = 1.4f;
	int nlevels = 16;
	int edgeThreshold = 31;
	int firstLevel = 0;
	int WTA_K = 2;
	cv::ORB::ScoreType scoreType = cv::ORB::HARRIS_SCORE;
	int patchSize = 31;
	int fastThreshold = 81;
	cv::Ptr<cv::ORB> orbPtr = cv::ORB::create(nfeatures, scaleFactor, nlevels, edgeThreshold, firstLevel,WTA_K, scoreType,patchSize,fastThreshold);
	//cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
	//cv::Ptr <cv::FlannBasedMatcher> matcher = cv::FlannBasedMatcher::create();
	//cv::Ptr<cv::FlannBasedMatcher> matcher = cv::FlannBasedMatcher::create();
	cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE_HAMMING);
	cv::Mat affineParams;
	std::vector<uchar> inliers;

	


	g_cap >> frame;
	count++;
	orbPtr->detectAndCompute(frame, cv::noArray(), keypoints, descriptors_prev);
	cv::drawKeypoints(frame, keypoints, frame, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	//cv::imshow("source", frame);
	writer << frame;

	keypoints_prev = keypoints;
	double sum_tx = 0;
	double sum_ty = 0;

	for (;;) {
		if (g_run != 0) {
			count++;
			g_cap >> frame; if (frame.empty()) break;

			//out write 1
			out << count - 1 << " ";

			int current_pos = (int)g_cap.get(cv::CAP_PROP_POS_FRAMES);
			g_dontset = 1;
			cv::setTrackbarPos("Position", "source", current_pos);

			auto begin_dAC = std::chrono::steady_clock::now();
			orbPtr->detectAndCompute(frame, cv::noArray(), keypoints, descriptors);
			auto end_dAC = std::chrono::steady_clock::now();
			auto detectAndCompute_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_dAC - begin_dAC);


			std::vector< std::vector<cv::DMatch> > keyp_match;
			auto begin_kM = std::chrono::steady_clock::now();
			matcher->knnMatch(descriptors_prev, descriptors, keyp_match, 2);
			auto end_kM = std::chrono::steady_clock::now();
			auto knnMatch_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_kM - begin_kM);



			std::vector<cv::DMatch>  keyp_match_good;
			double ratio = 0.7;
			for (size_t i = 0; i < keyp_match.size(); i++) {
				if (keyp_match[i][0].distance < ratio * keyp_match[i][1].distance) {
					keyp_match_good.push_back(keyp_match[i][0]);
				}
			}

			

			std::vector<cv::Point2f> points;
			std::vector<cv::Point2f> points_prev;
			std::vector<cv::KeyPoint> traceable_keypoints;
			std::set<int> idx_monitored;
			for (size_t i = 0; i < keyp_match_good.size(); i++) {
				traceable_keypoints.push_back(keypoints[keyp_match_good[i].trainIdx]);
				points_prev.push_back(keypoints_prev[keyp_match_good[i].queryIdx].pt);
				points.push_back(keypoints[keyp_match_good[i].trainIdx].pt);
				idx_monitored.insert(keyp_match_good[i].trainIdx);
			}

			
			affineParams = cv::estimateAffinePartial2D(points_prev, points, inliers);
			//std::cout << "affine" << affineParams << std::endl;

			double theta = atan2(affineParams.at<double>(1, 0), affineParams.at<double>(0, 0));
			double tx = affineParams.at<double>(0, 2);
			double ty = affineParams.at<double>(1, 2);
			sum_tx += tx;
			sum_ty += ty;
			//out write 2
			out << 180 * theta / CV_PI << " ";
			//out write 3
			out << tx << " ";
			//out write 4
			out << ty << " ";
			//out write 5
			out << tx * tx + ty * ty << " ";
			out << sum_tx << " ";
			out << sum_ty << std::endl;
			//out << knnMatch_time.count() << std::endl;

			cv::drawKeypoints(frame, traceable_keypoints, frame, cv::Scalar(0, 255, 0), cv::DrawMatchesFlags::DEFAULT);

			std::vector<cv::KeyPoint> untraceable_keypoints;
			for (int i = 0; i < keypoints.size(); i++) {
				if (!idx_monitored.count(i)) {
					untraceable_keypoints.push_back(keypoints[i]);
				}
			}
			//cv::drawKeypoints(frame, untraceable_keypoints, frame, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DEFAULT);

			points_prev = points;
			keypoints_prev = keypoints;
			descriptors_prev = descriptors.clone();

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
	out.close();
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