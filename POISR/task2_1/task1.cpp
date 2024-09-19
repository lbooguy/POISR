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

bool check_keyp(float distance, std::vector< std::vector<cv::DMatch>>& last_keyp){

	for (const auto& match : last_keyp) {
		
		if ((match[0].distance < 0.7 * match[1].distance) and match[0].distance == distance) {
			//std::cout << match[0].distance << " " << distance << std::endl;
			return true;
		}
	}
	std::cout << "FALSE" << std::endl;
	return false;
}
void draw_tracks(cv::Mat& frame,
				 std::vector<std::vector<cv::KeyPoint>>& keypoints_prev_trajectory,
				 std::vector<std::vector<cv::KeyPoint>>& keypoints_trajectory,
				 std::vector < std::vector< std::vector<cv::DMatch>>> keyp_match_trajectory) {
	int number_last = keyp_match_trajectory.size() - 1;
	for (int i = 0; i < keypoints_prev_trajectory.size(); i++) {
		for (const auto& match : keyp_match_trajectory[i]) {
			if ((match[0].distance < 0.7 * match[1].distance) ){//and check_keyp(match[0].distance, keyp_match_trajectory[number_last])) {
				cv::Point2f pt1 = keypoints_prev_trajectory[i][match[0].queryIdx].pt;
				cv::Point2f pt2 = keypoints_trajectory[i][match[0].trainIdx].pt;
				cv::line(frame, pt1, pt2, cv::Scalar(0, 255, 0), 2);
			}
		}
	}
}

//void draw_tracks(cv::Mat& frame,
//				std::vector<std::vector<cv::KeyPoint>>& keypoints_prev_trajectory,
//				std::vector<std::vector<cv::KeyPoint>>& keypoints_trajectory,
//				std::vector < std::vector< std::vector<cv::DMatch>>>& keyp_match_trajectory,
//				std::vector< cv::Mat>& descriptors_trajectory,
//				std::vector< cv::Mat>& descriptors_prev_trajectory) {
//	int number_last = keyp_match_trajectory.size() - 1;
//	for (const auto& match : keyp_match_trajectory[number_last]) {
//		if ((match[0].distance < 0.7 * match[1].distance)) {//and check_keyp(match[0].distance, keyp_match_trajectory[number_last])) {
//			cv::Point2f pt1 = keypoints_prev_trajectory[number_last][match[0].queryIdx].pt;
//			cv::Point2f pt2 = keypoints_trajectory[number_last][match[0].trainIdx].pt;
//			cv::line(frame, pt1, pt2, cv::Scalar(0, 255, 0), 2);
//		}
//	}
//	cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE_HAMMING);
//	
//	for (int i = keypoints_prev_trajectory.size() - 2; i >= 0; i--) {
//		std::vector< std::vector<cv::DMatch> > keyp_match;
//		matcher->knnMatch(descriptors_prev_trajectory[i], descriptors_trajectory[number_last], keyp_match, 2);
//		for (int k = 0; k < keyp_match.size(); k++) {
//			for (int j = 0; j < keyp_match_trajectory[i].size(); j++) {
//				if (keyp_match[k][0].queryIdx == keyp_match_trajectory[i][j][0].queryIdx) {
//							cv::Point2f pt1 = keypoints_prev_trajectory[i][keyp_match_trajectory[i][j][0].queryIdx].pt;
//							cv::Point2f pt2 = keypoints_trajectory[i][keyp_match_trajectory[i][j][0].trainIdx].pt;
//							cv::line(frame, pt1, pt2, cv::Scalar(0, 255, 0), 2);
//				}
//			}
//		}
//	}
//}

float distance_points(float x0, float y0, float x1,float y1) {
	std::cout << x0 << " " << y0 << " " << x1<<" " << y1 << std::endl;
	float square = pow(x0 - x1, 2) + pow(y0 - y1, 2);
	return pow(square, 0.5);

}
double source_video() {
	cv::namedWindow("source", cv::WINDOW_NORMAL);
	cv::namedWindow("UL", cv::WINDOW_NORMAL);
	cv::namedWindow("DL", cv::WINDOW_NORMAL);
	cv::namedWindow("UR", cv::WINDOW_NORMAL);
	cv::namedWindow("DR", cv::WINDOW_NORMAL);

	double total_time = 0;
	int count = 0;
	g_cap.open("C:/Users/USER/Videos/Movement.mp4");
	//g_cap.open(0);
	double fps = g_cap.get(cv::CAP_PROP_FPS);
	cv::Size size(
		(int)g_cap.get(cv::CAP_PROP_FRAME_WIDTH),
		(int)g_cap.get(cv::CAP_PROP_FRAME_HEIGHT)
	);
	cv::VideoWriter writer;
	writer.open("task2_1/video.mp4", cv::VideoWriter::fourcc('H', '2', '6', '4'), fps, size);
	int frames = (int)g_cap.get(cv::CAP_PROP_FRAME_COUNT);
	int tmpw = (int)g_cap.get(cv::CAP_PROP_FRAME_WIDTH);
	int tmph = (int)g_cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	//cout << "In the form of " << frames << " frames in size("
		//<< tmpw << ", " << tmph << ")." << endl;
	cv::createTrackbar("Position", "source", &g_slider_position, frames, onTrackbarSlide);
	cv::Mat frame;



	std::ofstream out;

	out.open("task2_1/1.5.txt");
	if (out.is_open())
	{
		out << "N, det_UL, match_UL, det_DL, match_DL, det_UR, match_UR, det_DR, match_DR" << std::endl;
	}

	//roi params
	//int X0 = 200;
	//int Y0 = 270;
	//int width = 1200;
	//int height = 500;

	//cv::Point c1(X0, Y0), c2(X0 + width, Y0); //up line
	//cv::Point c3(X0, Y0 + height), c4(X0 + width, Y0 + height);//down line
	//cv::Point c5(X0, Y0), c6(X0, Y0 + height); // left line
	//cv::Point c7(X0 + width, Y0), c8(X0 + width, Y0 + height); // right line

	//ORB
	cv::Mat descriptors;
	cv::Mat descriptors_prev;
	std::vector<cv::KeyPoint> keypoints;
	std::vector<cv::KeyPoint> keypoints_prev;
	std::vector<cv::KeyPoint> keypoints_match;
	//std::vector< std::vector<cv::DMatch> > keyp_match;
	int nfeatures = 250;
	float scaleFactor = 1.2f;
	int nlevels = 8;
	int edgeThreshold = 31;
	int firstLevel = 0;
	int WTA_K = 2;
	int scoreTyoe = 0;
	int patchSize = 31;
	int fastThreshold = 20;
	cv::Ptr<cv::ORB> orbPtr = cv::ORB::create(nfeatures);
	//cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
	//cv::Ptr <cv::FlannBasedMatcher> matcher = cv::FlannBasedMatcher::create();
	//cv::Ptr<cv::FlannBasedMatcher> matcher = cv::FlannBasedMatcher::create();
	cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE_HAMMING);
	std::vector<std::vector<cv::KeyPoint>> keypoints_prev_trajectory;
	std::vector<std::vector<cv::KeyPoint>> keypoints_trajectory;
	std::vector < std::vector< std::vector<cv::DMatch>>> keyp_match_trajectory;
	std::vector< cv::Mat> descriptors_trajectory;
	std::vector< cv::Mat> descriptors_prev_trajectory;
	g_cap >> frame;

	//TASK 4,5
	int WIDTH = frame.size().width;
	int HEIGHT = frame.size().height;
	cv::Rect UL_roi(0, 0 , WIDTH / 2, HEIGHT/2);
	cv::Rect DL_roi(0, HEIGHT / 2, WIDTH / 2, HEIGHT / 2);
	cv::Rect UR_roi(WIDTH / 2, 0, WIDTH / 2, HEIGHT / 2);
	cv::Rect DR_roi(WIDTH / 2, HEIGHT / 2, WIDTH / 2, HEIGHT / 2);

	cv::Mat UL = frame(UL_roi);
	cv::Mat DL = frame(DL_roi);
	cv::Mat UR = frame(UR_roi);
	cv::Mat DR = frame(DR_roi);


	cv::Mat descriptors_UL;
	cv::Mat descriptors_UL_prev;
	std::vector<cv::KeyPoint> keypoints_UL;
	std::vector<cv::KeyPoint> keypoints_UL_prev;
	
	cv::Mat descriptors_DL;
	cv::Mat descriptors_DL_prev;
	std::vector<cv::KeyPoint> keypoints_DL;

	cv::Mat descriptors_UR;
	cv::Mat descriptors_UR_prev;
	std::vector<cv::KeyPoint> keypoints_UR;

	cv::Mat descriptors_DR;
	cv::Mat descriptors_DR_prev;
	std::vector<cv::KeyPoint> keypoints_DR;

	orbPtr->detectAndCompute(UL, cv::noArray(), keypoints_UL_prev, descriptors_UL_prev);
	orbPtr->detectAndCompute(DL, cv::noArray(), keypoints_DL, descriptors_DL_prev);
	orbPtr->detectAndCompute(UR, cv::noArray(), keypoints_UR, descriptors_UR_prev);
	orbPtr->detectAndCompute(DR, cv::noArray(), keypoints_DR, descriptors_DR_prev);

	//cv::drawKeypoints(frame, keypoints_prev, frame, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	//cv::imshow("source", frame);
	
	cv::drawKeypoints(UL, keypoints_UL, UL, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DEFAULT);
	cv::drawKeypoints(DL, keypoints_DL, DL, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DEFAULT);
	cv::drawKeypoints(UR, keypoints_UR, UR, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DEFAULT);
	cv::drawKeypoints(DR, keypoints_DR, DR, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DEFAULT);

	cv::imshow("UL", UL);
	cv::imshow("DL", DL);
	cv::imshow("UR", UR);
	cv::imshow("DR", DR);
	//writer << frame;

	count++;
	orbPtr->detectAndCompute(frame, cv::noArray(), keypoints_prev, descriptors_prev);
	const std::vector<std::vector<int>> coordinates_point_gt = { {1132, 1459},{1132,1471},{1134,1482}, {1133, 1491}, {1127,1506}, {1126, 1526}, {1125,1541}, {1123, 1558},
{1121,1577}, {1120, 1596}, {1119, 1615}, {1115, 1629}, {1112, 1650}, {1110, 1670}, {1110, 1687}, {1103, 1696}, {1099, 1712}, {1090, 1731}, {1084,1745}, {1082, 1758}, {1078, 1772},
	{1079, 1794}, {1077, 1811}, {1088, 1826}, {1094, 1843}, {1102, 1860}, {1110, 1882}, {1117, 1903}, {1124, 1930}, {1134, 1960} };

	//std::vector<cv::KeyPoint> draw_keypoints;
	//int area_size = 10;
	//for (const auto& point : keypoints_prev) {
	//	if ( abs(point.pt.x - coordinates_point_gt[0][0]) < area_size  and abs(point.pt.y - coordinates_point_gt[0][1]) < area_size) {//pow(point.pt.x, 2) + pow(point.pt.y, 2) < radius * radius) {
	//		draw_keypoints.push_back(point);
	//	}
	//}
	//cv::drawKeypoints(frame, draw_keypoints, frame, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DEFAULT);
	//
	/*out << count - 1 << " ";
	out << distance_points(draw_keypoints[0].pt.x, draw_keypoints[0].pt.y, coordinates_point_gt[0][0], coordinates_point_gt[0][1]) << std::endl;
	imwrite("C:/Users/USER/source/repos/POISR/POISR/task2_1/frames_with_point/" + to_string(count) + ".jpg", frame);*/
	for (;;) {
		if (g_run != 0) {
			//keypoints_prev_trajectory.push_back(keypoints_prev);
			//descriptors_prev_trajectory.push_back(descriptors_prev);
			count++;
			g_cap >> frame; if (frame.empty()) break;
			int current_pos = (int)g_cap.get(cv::CAP_PROP_POS_FRAMES);
			g_dontset = 1;
			cv::setTrackbarPos("Position", "source", current_pos);

			cv::Mat UL = frame(UL_roi);
			cv::Mat DL = frame(DL_roi);
			cv::Mat UR = frame(UR_roi);
			cv::Mat DR = frame(DR_roi);

			orbPtr->detectAndCompute(UL, cv::noArray(), keypoints_UL, descriptors_UL);
			orbPtr->detectAndCompute(DL, cv::noArray(), keypoints_DL, descriptors_DL);
			orbPtr->detectAndCompute(UR, cv::noArray(), keypoints_UR, descriptors_UR);
			orbPtr->detectAndCompute(DR, cv::noArray(), keypoints_DR, descriptors_DR);

			std::vector< std::vector<cv::DMatch> > keyp_match_UL;
			std::vector< std::vector<cv::DMatch> > keyp_match_DL;
			std::vector< std::vector<cv::DMatch> > keyp_match_UR;
			std::vector< std::vector<cv::DMatch> > keyp_match_DR;

			matcher->knnMatch(descriptors_UL_prev, descriptors_UL, keyp_match_UL, 2);
			matcher->knnMatch(descriptors_DL_prev, descriptors_DL, keyp_match_DL, 2);
			matcher->knnMatch(descriptors_UR_prev, descriptors_UR, keyp_match_UR, 2);
			matcher->knnMatch(descriptors_DR_prev, descriptors_DR, keyp_match_DR, 2);
			//out << "N, det_UL, match_UL, det_DL, match_DL, det_UR, match_UR, det_DR, match_DR" << std::endl;
			int count_UL = 0;
			for (const auto& match : keyp_match_UL) {
				if (match[0].distance < 0.7 * match[1].distance) {

					count_UL++;
				}
			}

			int count_DL = 0;
			for (const auto& match : keyp_match_DL) {
				if (match[0].distance < 0.7 * match[1].distance) {

					count_DL++;
				}
			}
			int count_UR = 0;
			for (const auto& match : keyp_match_UR) {
				if (match[0].distance < 0.7 * match[1].distance) {

					count_UR++;
				}
			}
			int count_DR = 0;
			for (const auto& match : keyp_match_DR) {
				if (match[0].distance < 0.7 * match[1].distance) {

					count_DR++;
				}
			}
			out << count - 1<<" " << keypoints_UL.size() << " " << count_UL << " " <<
									 keypoints_DL.size() << " " << count_DL << " " <<
									 keypoints_UR.size() << " " << count_UR << " " <<
									 keypoints_DR.size() << " " << count_DR << std::endl;
			descriptors_UL_prev = descriptors_UL.clone();
			descriptors_DL_prev = descriptors_DL.clone();
			descriptors_UR_prev = descriptors_UR.clone();
			descriptors_DR_prev = descriptors_DR.clone();

			/*cv::drawKeypoints(UL, keypoints_UL, UL, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DEFAULT);
			cv::drawKeypoints(DL, keypoints_DL, DL, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DEFAULT);
			cv::drawKeypoints(UR, keypoints_UR, UR, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DEFAULT);
			cv::drawKeypoints(DR, keypoints_DR, DR, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DEFAULT);*/

			/*for (const auto& match : keyp_match_UL) {
				if (match[0].distance < 0.7 * match[1].distance) {

					cv::Point2f pt1 = keypoints_UL_prev[match[0].queryIdx].pt;
					cv::Point2f pt2 = keypoints_UL[match[0].trainIdx].pt;
					cv::line(UL, pt1, pt2, cv::Scalar(0, 255, 0), 2);
				}
			}*/
			keypoints_UL_prev = keypoints_UL;
			/*cv::imshow("UL", UL);
			cv::imshow("DL", DL);
			cv::imshow("UR", UR);
			cv::imshow("DR", DR);*/

			//TASK 1,2,3
			/*
			auto begin_TOTAL = std::chrono::steady_clock::now();

			auto begin_dAC = std::chrono::steady_clock::now();
			orbPtr->detectAndCompute(frame, cv::noArray(), keypoints, descriptors);
			auto end_dAC = std::chrono::steady_clock::now();
			auto detectAndCompute_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_dAC - begin_dAC);
			keypoints_trajectory.push_back(keypoints);*/
			
			/*std::vector<cv::KeyPoint> draw_keypoints;
			for (const auto& point : keypoints) {
				if (abs(point.pt.x - coordinates_point_gt[count - 1][0]) < area_size and abs(point.pt.y - coordinates_point_gt[count - 1][1]) < area_size) {
					draw_keypoints.push_back(point);
				}
			}*/
			//out << count - 1 << " ";
			//out << distance_points(draw_keypoints[0].pt.x, draw_keypoints[0].pt.y, coordinates_point_gt[count - 1][0], coordinates_point_gt[count - 1][1]) << std::endl;
			//cv::drawKeypoints(frame, draw_keypoints, frame, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DEFAULT);
			////cv::drawKeypoints(frame, keypoints, frame, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DEFAULT);

			/*std::vector< std::vector<cv::DMatch> > keyp_match;
			//auto begin_kM = std::chrono::steady_clock::now();
			matcher->knnMatch(descriptors_prev, descriptors, keyp_match, 2);
			auto end_kM = std::chrono::steady_clock::now();
			keyp_match_trajectory.push_back(keyp_match);
			descriptors_trajectory.push_back(descriptors);
			auto knnMatch_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_kM - begin_kM);

			auto end_TOTAL = std::chrono::steady_clock::now();
			auto TOTAL_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_TOTAL - begin_TOTAL);*/

			/*for (const auto& match : keyp_match) {
				if (match[0].distance < 0.7 * match[1].distance) {

					cv::Point2f pt1 = keypoints_prev[match[0].queryIdx].pt;
					cv::Point2f pt2 = keypoints[match[0].trainIdx].pt;
					cv::line(frame, pt1, pt2, cv::Scalar(0, 255, 0), 2);
				}
			}*/
		    /*draw_tracks(frame,
			keypoints_prev_trajectory,
			keypoints_trajectory,
			keyp_match_trajectory);*/

			/*draw_tracks(frame,
				keypoints_prev_trajectory,
				keypoints_trajectory,
				keyp_match_trajectory,
				descriptors_trajectory,
				descriptors_prev_trajectory);*/

			/*descriptors_prev = descriptors.clone();
			keypoints_prev = keypoints;
			cv::imshow("source", frame);*/
			/*out << count - 1 << " ";
			out << TOTAL_time.count() << " ";
			out << detectAndCompute_time.count() << " ";
			out << knnMatch_time.count() << std::endl;*/

			//writer << frame;
			//if (count<=30) { imwrite("C:/Users/USER/source/repos/POISR/POISR/task2_1/frames_with_point/" + to_string(count) + ".jpg", frame); }
			
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

	g_cap.release();
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