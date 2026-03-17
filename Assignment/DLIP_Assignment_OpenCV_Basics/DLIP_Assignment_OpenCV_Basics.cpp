/*------------------------------------------------------------------------------------------\
@ Deep Learning & Image Processing  by Young-Keun Kim - Handong Global University

Author          : HWANG MooYeol
Created         : 01-04-2019
Modified        : 07-03-2025
Language/ver	: C++
Course		: Deep Learning & Image Processing

Description     : "Assignment1"
/------------------------------------------------------------------------------------------*/

#include <iostream>
#include <opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
	/*  read image  */
	String HGU_logo = "../../Image/HGU_logo.jpg";  // class
	Mat src = imread(HGU_logo);  //Mat class
	Mat src_gray = imread("../../Image/HGU_logo.jpg", 0);  // read in grayscale

	Mat src_resized;
	resize(src_gray, src_resized, Size(), 0.5, 0.5, 1);

	// Output #1 //
	Mat output_1 = Mat::zeros(src_gray.size(), src_gray.type());
	bitwise_not(output_1, output_1);


	for (int i = 0; i < src_resized.rows; i++)
	{
		for (int j = 0; j < src_resized.cols; j++)
		{
			output_1.at<uchar>(i, j) = src_resized.at<uchar>(i,j);
		}
	}
	
	// Output #2 //
	Mat output_2 = Mat::zeros(src_gray.size(), src_gray.type());
	bitwise_not(output_2, output_2);

	flip(src_resized, src_resized, -1);

	for (int i = 0 ; i < src_resized.rows; i++)
	{
		for (int j = 0; j < src_resized.cols; j++)
		{
			output_2.at<uchar>(i + (src.rows / 4), j + (src.cols / 4) ) = src_resized.at<uchar>(i, j);
		}
	}

	
	// Output #3 //
	Mat output_3 = Mat::zeros(src_gray.size(), src_gray.type());
	bitwise_not(output_3, output_3);
	int _x = 135;
	int _y = 145;
	int width = 190;
	int height = 201;

	Rect r(_x, _y, width, height);
	Mat roiImg = src_gray(r);

	for (int i = 0; i < roiImg.rows; i++)
	{
		for (int j = 0; j < roiImg.cols; j++)
		{
			output_3.at<uchar>(i+(src.rows-height), j+(src.cols-width)) = roiImg.at<uchar>(i, j);
		}
	}
	

	// print image size //
	printf("size of image is %d X %d px \n", src_gray.cols, src_gray.rows);


	/*  display image  */
	namedWindow("Origianl", WINDOW_AUTOSIZE);
	imshow("Original", src_gray);

	namedWindow("Output #1", WINDOW_AUTOSIZE);
	imshow("Output #1", output_1);
	
	namedWindow("Output #2", WINDOW_AUTOSIZE);
	imshow("Output #2", output_2);

	namedWindow("Output #3", WINDOW_AUTOSIZE);
	imshow("Output #3", output_3);


	waitKey(0);
}
