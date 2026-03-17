#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int hmin1 = 40, hmax1 = 85, smin1 = 50, smax1 = 255, vmin1 = 50, vmax1 = 255;
int hmin2 = 45, hmax2 = 145, smin2 = 26, smax2 = 255, vmin2 = 19, vmax2 = 253;

Scalar lower1(hmin1, smin1, vmin1);
Scalar upper1(hmax1, smax1, vmax1);
Scalar lower2(hmin2, smin2, vmin2);
Scalar upper2(hmax2, smax2, vmax2);

void expandContour(vector<Point>& contour, double scaleFactor) {

    Moments mu = moments(contour);
    Point center(mu.m10 / mu.m00, mu.m01 / mu.m00);
    for (size_t i = 0; i < contour.size(); i++) {
        contour[i] = center + scaleFactor * (contour[i] - center);
    }
}

int main() {
    string videoPath = "../../video/LAB_MagicCloak_Sample.mp4";
    VideoCapture cap(videoPath);

    Mat frame, background;  //동영상 저장 및 첫 프레임 저장을 위한 background 함수
    Mat image_disp, blur_disp, hsv_disp, laplacian_dst, mask1, mask2, inrange_disp;
    Mat result_laplcaian, inrange_processed;
    Mat kernel1 = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
    Mat kernel2 = getStructuringElement(MORPH_ELLIPSE, Size(7, 7));
    Mat mask, mask3c;
    Mat grad_x, grad_y, sobel_edge, result_sobel;

    vector<vector<Point>> contours, largeContours;
    vector<Vec4i> hierarchy;

    int kernel_size = 3;
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16U;

    while (true) {
        cap >> frame;
        if (frame.empty()) {
            cout << "End of video" << endl;
            break;
        }
        resize(frame, frame, Size(), 0.5, 0.5);       //이미지 resize

        if (background.empty())
            frame.copyTo(background);                 //첫 프레임 저장 변수

        frame.copyTo(image_disp);                     //실시간 프레임 저장 변수
        imshow("src", image_disp);

        GaussianBlur(image_disp, blur_disp, Size(5, 5), 0);
        cvtColor(image_disp, hsv_disp, COLOR_BGR2HSV); //HSV convert
        //imshow("hsv", hsv_disp);

        //Laplacian Filter
        //Laplacian(hsv_disp, laplacian_dst, ddepth, kernel_size, scale, delta, BORDER_DEFAULT); //edge
        //hsv_disp.convertTo(hsv_disp, CV_16U);
        //result_laplcaian = hsv_disp - laplacian_dst;
        //result_laplcaian.convertTo(result_laplcaian, CV_8U);
        //imshow("Laplacian", result_laplcaian);

        //Sobel filter
        Sobel(hsv_disp, grad_x, ddepth, 1, 0, kernel_size, scale, delta, BORDER_DEFAULT);
        Sobel(hsv_disp, grad_y, ddepth, 0, 1, kernel_size, scale, delta, BORDER_DEFAULT);
        addWeighted(grad_x, 0.5, grad_y, 0.5, 0, sobel_edge);

        hsv_disp.convertTo(hsv_disp, CV_16U);
        sobel_edge.convertTo(sobel_edge, CV_16U);
        result_sobel = hsv_disp - sobel_edge;
        result_sobel.convertTo(result_sobel, CV_8U);

        //imshow("Sobel", result_sobel);

        //InRange
        inRange(result_sobel, lower1, upper1, mask1);
        inRange(result_sobel, lower2, upper2, mask2);
        bitwise_or(mask1, mask2, inrange_disp);

        //Morphology
        morphologyEx(inrange_disp, inrange_processed, MORPH_OPEN, kernel1);

        //Contour
        findContours(inrange_processed, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        largeContours.clear();

        for (int i = 0; i < contours.size(); i++) {
            if (contourArea(contours[i]) > 3000) { //Detect only contours larger than a certain size

                expandContour(contours[i], 1.2);
                largeContours.push_back(contours[i]);
            }
        }

        //draw mask(white color)
        mask = Mat::zeros(frame.size(), CV_8UC1);

        for (size_t i = 0; i < largeContours.size(); i++) {
            drawContours(mask, largeContours, (int)i, Scalar(255), FILLED);
        }
        dilate(mask, mask, kernel2);

        //imshow("mask", mask);

        Mat background_pt, frame_pt;
        bitwise_and(background, background, background_pt, mask);
        //imshow("background_pt", background_pt);
        bitwise_and(frame, frame, frame_pt, ~mask);
        //imshow("frame_pt", frame_pt);

        Mat resultsFrame;
        add(background_pt, frame_pt, resultsFrame);

        //imshow("mask", mask);
        imshow("Video with Background", resultsFrame);
        //imshow("Source", image_disp);

        if (waitKey(10) == 27) break;  // ESC to exit
    }

    cap.release();
    destroyAllWindows();
    return 0;
}