#include <opencv2/opencv.hpp> 
#include <iostream> 

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	// Read the image file 
	Mat image = imread("C:/Users/USER/Pictures/Camera Roll/WIN_20231112_17_52_12_Pro.jpg");
	// Check for failure 
	if (image.empty())
	{
		cout << "Image Not Found!!!" << endl;
		cin.get(); //wait for any key press 
		return -1;
	}
	// Show our image inside a window. 
	imshow("Image Window Name here", image);

	// Wait for any keystroke in the window 
	waitKey(0);
	return 0;
}
//asdasdasd