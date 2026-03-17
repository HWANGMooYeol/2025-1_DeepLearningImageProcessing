#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
	/*  read src  */
	// Add code here
	String HGU_logo = "../../Image/HGU_logo.jpg";
	Mat src = imread(HGU_logo);

	/* Flip src image */
	// Add code here and show image
	Mat FlipImg;
	flip(src, FlipImg, 1);

	/*  Crop(Region of Interest)  original image */
	// Add code here and show image


	/*  Show source(src) and destination(dst)  */
	// Add code here
	imshow("fliped_Img", FlipImg);

	waitKey(0);
}