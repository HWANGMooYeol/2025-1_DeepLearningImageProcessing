/*------------------------------------------------------/
* Image Proccessing with Deep Learning - DLIP
*
* MIDTERM 2025
*
* Date: 2025-04-18
*
* Name: HWANG MooYeol
*
* ID: 22000814
*
------------------------------------------------------*/



#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main()
{
	/******** Declaration of variables ********/
	Mat src, mask, dst, hsv, blur1, blur2 , bound;
	Mat result_laplcaian;
	int hmin = 3, hmax = 16, smin = 78, smax = 115, vmin = 125, vmax = 202;
	int kernel_size = 3;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16U;
	// Declare Other Variables	
	// YOUR CODE GOES HERE
	// YOUR CODE GOES HERE


	/******** Read src image ********/
	src = imread("Test1_image_2025.jpg");
	// Load image check
	if (src.empty())
	{
		cout << "File Read Failed : src is empty" << endl;
		waitKey(0);
	}

	dst = src;
	bound = src;

	mask = Mat::zeros(src.size(), CV_8UC3);

	// Display source image
	namedWindow("src", 1);
	imshow("src", src);

	

	/******** Color segmentation about cloak			 ********/
	// HINT:  Use HSV color space to find inRange values
	// 
	// YOUR CODE GOES HERE
	// YOUR CODE GOES HERE
	blur(src, blur1, Size(9, 9));
	GaussianBlur(src, blur2, Size(5, 5), 0);
	cvtColor(blur2, hsv, COLOR_BGR2HSV);
	
	/*
	//Laplacian Filter
	Laplacian(hsv, dst, ddepth, kernel_size, scale, delta, BORDER_DEFAULT); //edge
	hsv.convertTo(hsv, CV_16U);
	result_laplcaian = hsv - dst;
	result_laplcaian.convertTo(result_laplcaian, CV_8U);
	//imshow("Laplacian", result_laplcaian);
	inRange(result_laplcaian, Scalar(hmin, smin, vmin), Scalar(hmax, smax, vmax), dst);
	*/

	inRange(hsv, Scalar(hmin, smin, vmin), Scalar(hmax, smax, vmax), dst);


	/******** Post-processing(e.g. Morphology...) ********/
	// YOUR CODE GOES HERE
	// YOUR CODE GOES HERE
	morphologyEx(dst, dst, MORPH_CLOSE, getStructuringElement(MORPH_RECT, Size(11,11)));
	//morphologyEx(dst, dst, MORPH_OPEN, getStructuringElement(MORPH_RECT, Size(3, 3)));



	// Display skin-color-segmented output
	// YOUR CODE GOES HERE
	// YOUR CODE GOES HERE
	imshow("Segment skin-colored area", dst);




	/******** Create and Display Mask for Faces ********/
	// HINT: Find contours and select only face masks
	// YOUR CODE GOES HERE
	// YOUR CODE GOES HERE
	
	vector<vector<Point>> contours;
	int face_num = 0;
	findContours(dst, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	if (!contours.empty())
	{
		for (int i = 0; i < contours.size(); i++) {
			//drawContours(src, contours, (int)i, Scalar(0, 255, 0), 1);
			double area = contourArea(contours[i]);
			Moments m = moments(contours[i]);
			Rect rect = cv::boundingRect(contours[i]);
			/*
			if (m.m00 != 0) {
				int cx = int(m.m10 / m.m00);
				int cy = int(m.m01 / m.m00);
				string area_text = to_string(int(area));
				int baseline = 0;
				Size text_size = getTextSize(area_text, FONT_HERSHEY_SIMPLEX, 0.5, 1.5, &baseline);

				putText(src, area_text, Point(cx,cy),FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1.5);
			}
			*/


			if (area >= 843 ) {
				//drawContours(mask, contours, i, Scalar(255, 255, 255), FILLED);
				face_num = face_num + 1;
				rectangle(mask, rect, cv::Scalar(255, 255, 255), FILLED);
				rectangle(bound, rect, cv::Scalar(255, 255, 255), 2);
				
			}
		}
			
	}
	
	// Print how many faces found
	// YOUR CODE GOES HERE
	// YOUR CODE GOES HERE
	printf("----------------------------------------------------------------------------------------------\n");
	printf(" faces found: %d",face_num);
	printf("----------------------------------------------------------------------------------------------\n");



	// Display mask images (binary)
	// YOUR CODE GOES HERE
	// YOUR CODE GOES HERE
	vector<vector<Point>> contours1;
	imshow("mask images", mask);



	// Display mask boundary on source image
	// YOUR CODE GOES HERE
	// YOUR CODE GOES HERE
	imshow(" mask boundary on source image", bound);



	/******** Blurring Masked Area   ********/
	// HINT:  Use bitwise logic,  bitwise_and() etc .. 
	// HINT:  You also can combine: (1) blurred image(inside the mask) with (2) not-blurred image(outside the mask)
	// 
	// YOUR CODE GOES HERE
	// YOUR CODE GOES HERE
	src = src & ~mask;
	blur1 = blur1 & mask;
	dst = src + blur1;

	

	/******** Display the final output   ********/
	//Display output image
	namedWindow("dst", 1);
	imshow("dst", dst);



	int key = waitKey(0);
	if (key == 27) {
		cout << "ESC key is pressed by user\n";
		return 0;
	}

	return 0;
}

