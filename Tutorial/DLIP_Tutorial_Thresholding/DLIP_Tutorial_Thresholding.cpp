/*------------------------------------------------------/
* Image Proccessing with Deep Learning
* OpenCV : Threshold Demo
* Created: 2021-Spring
------------------------------------------------------*/
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

//* @function main
int main()
{
	Mat src, src_gray, dst, dst_morph;

	src = imread("../../Image/coin.jpg", 0);    // Load an image (DLIP/Image/coin.jpg)

	if (src.empty())					// Load image check
	{
		cout << "File Read Failed : src is empty" << endl;
		waitKey(0);
	}
	// Create a window to display results
	namedWindow("DemoWIndow", WINDOW_AUTOSIZE); //WINDOW_AUTOSIZE(1) :Fixed Window, 0: Unfixed window
	if (!src.empty())imshow("DemoWIndow", src); // Show image


	/* threshold_type
	0: Binary
	1: Binary Inverted
	2: Threshold Truncated
	3: Threshold to Zero
	4: Threshold to Zero Inverted*/
	double maxValue = 255;
	int adaptiveMethod = ADAPTIVE_THRESH_MEAN_C;
	int thresholdType = THRESH_BINARY;
	int blockSize = 3;
	double C = 11;


	adaptiveThreshold(src, dst, maxValue, adaptiveMethod, thresholdType, blockSize, C);

	// Create a window to display results
	namedWindow("ThreshWIndow", WINDOW_AUTOSIZE); //WINDOW_AUTOSIZE(1) :Fixed Window, 0: Unfixed window
	imshow("ThreshWIndow", dst); // Show image

	waitKey(0);//Pause the program
	return 0;
}