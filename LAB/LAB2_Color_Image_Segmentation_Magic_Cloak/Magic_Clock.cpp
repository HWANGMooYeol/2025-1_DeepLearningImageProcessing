#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int frame_cnt;
int frame = 0;
int hmin = 28, hmax = 110, smin = 4, smax = 161, vmin = 115, vmax = 250;
Mat frame1, frame2, video, hsv, dst, mask, mask_inv, background, current, result, bg;

int main(int ac, char** av) {

	VideoCapture cap("LAB2_2.mp4");
	VideoCapture bg1("bg.mp4");

	frame_cnt = cap.get(CAP_PROP_FRAME_COUNT);

	if (!cap.isOpened())
	{
		printf("Can't open the camera");
		return -1;
	}

	bg1 >> frame1;		// 1st frame
	imshow("img1", frame1);
	//Size size = frame1.size();

	//bg = imread("BackGround.jpg");
	bg = frame1;
	//resize(bg, bg, Size(size.width,size.height));
	//imshow("background", bg);


	while (frame < frame_cnt-1)
	{
		cap >> video;
		frame += 1;

		cvtColor(video, hsv, COLOR_BGR2HSV);
		blur(hsv, hsv, Size(3, 3));
		inRange(hsv, Scalar(hmin, smin, vmin), Scalar(hmax, smax, vmax), dst);
		morphologyEx(dst, dst, MORPH_OPEN, getStructuringElement(MORPH_RECT, Size(3, 3)));
		//imshow("InRange", dst);
		vector<vector<Point>> contours;

		findContours(dst, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		if (!contours.empty())
		{
			double maxArea = 0;
			int maxIdx = 0;
			for (int i = 0; i < contours.size(); i++) {
				double area = contourArea(contours[i]);
				if (area > maxArea) {
					maxArea = area;
					maxIdx = i;
				}
			}
			mask = Mat::zeros(video.size(), CV_8UC3);
			drawContours(mask, contours, maxIdx, Scalar(255, 255, 255), FILLED);
			dilate(mask, mask, Mat(), Point(-1, -1), 5);
			imshow("mask", mask);
			bitwise_not(mask, mask_inv);

			background = bg & mask;
			//imshow("bg", background);
			current = video & mask_inv;
			//imshow("current", current);
			result = current + background;
			imshow("result", result);
		}


		if (video.empty())
		{
			printf("empty image");
			return 0;
		}

		imshow("camera img", video);

		if (waitKey(25) == 27)
			break;
	}


	return 0;
}

//
#include <iostream>
#include "opencv2/opencv.hpp"

int main()
{
	cv::Mat src = cv::imread("../contour.bmp", cv::IMREAD_GRAYSCALE);
	if (src.empty())  return 1;

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	int mode = cv::RETR_TREE;
	int method = cv::CHAIN_APPROX_SIMPLE;

	cv::findContours(src, contours, hierarchy, mode, method);

	cv::Mat src_color;
	cv::cvtColor(src, src_color, cv::COLOR_GRAY2BGR);

	cv::Rect rect = cv::boundingRect(contours[0]);
	cv::rectangle(src_color, rect, cv::Scalar(0, 0, 255), 2);

	return 0;
}

