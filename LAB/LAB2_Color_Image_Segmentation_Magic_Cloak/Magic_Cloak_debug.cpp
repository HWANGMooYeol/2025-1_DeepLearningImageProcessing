#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int frame_cnt;
int frame = 0;
int hmin = 1, hmax = 179, smin = 113, smax = 255, vmin = 140, vmax = 255;
Mat frame1,frame2, video, hsv, dst, mask,mask_inv, background,current,result;

int main(int ac, char** av) {

	VideoCapture cap("LAB_MagicCloak_Sample1.mp4");

	frame_cnt = cap.get(CAP_PROP_FRAME_COUNT);

	if (!cap.isOpened())
	{
		printf("Can't open the camera");
		return -1;
	}

	cap >> frame1;		// 1st frame

	imshow("img1", frame1);

	while (1)
	{
		cap >> video;
		frame += 1;

		if (frame == 350) {
			cap >> frame2;
			imshow("midframe", frame2);
			cvtColor(frame2, hsv, COLOR_BGR2HSV);
			blur(hsv, hsv, Size(3, 3));
			inRange(hsv, Scalar(hmin, smin, vmin), Scalar(hmax, smax, vmax), dst);
			morphologyEx(dst, dst, MORPH_OPEN, getStructuringElement(MORPH_RECT, Size(3, 3)));
			imshow("InRange", dst);
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
				drawContours(mask, contours, maxIdx, Scalar(255,255,255), FILLED);
				dilate(mask, mask, Mat(), Point(-1, -1), 5);
				imshow("mask", mask);
				bitwise_not(mask, mask_inv);
			}
			
			background = frame1 & mask;
			imshow("bg", background);
			current = frame2 & mask_inv;
			imshow("current", current);
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