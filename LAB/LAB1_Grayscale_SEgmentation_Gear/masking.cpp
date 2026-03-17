#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main()
{
    Mat src, src_gray, imthres, mask, result;
    src = imread("../../Image/Gear4.jpg");

    if (src.empty()) {
        cerr << "Error: Image not found!" << endl;
        return -1;
    }

    cvtColor(src, src_gray, COLOR_BGR2GRAY);

    threshold(src_gray, imthres, 127, 255, THRESH_BINARY);

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(imthres, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    int largest_idx = -1;
    double max_area = 0;
    for (size_t i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i]);
        if (area > max_area) {
            max_area = area;
            largest_idx = i;
        }
    }

    if (largest_idx == -1) {
        cerr << "No valid contours found!" << endl;
        return -1;
    }

    vector<Point> approx;
    double epsilon = 0.02 * arcLength(contours[largest_idx], true);
    approxPolyDP(contours[largest_idx], approx, epsilon, true);

    Point2f center;
    float radius;
    minEnclosingCircle(approx, center, radius);

    mask = Mat::zeros(src.size(), CV_8UC1);
    circle(mask, center, static_cast<int>(radius * 0.805), Scalar(255), -1);

    result = src.clone();
    result.setTo(Scalar(0, 0, 0), mask);

    vector<vector<Point>> toothContours;
    findContours(imthres - mask, toothContours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    for (size_t i = 0; i < toothContours.size(); i++) {
        double tooth_area = contourArea(toothContours[i]);
        Moments m = moments(toothContours[i]);

        Scalar color = (tooth_area <= 1000 || tooth_area >= 1500) ? Scalar(0, 0, 255) : Scalar(0, 255, 0);

        drawContours(result, toothContours, (int)i, color, 1);

        if (m.m00 != 0) {
            int cx = int(m.m10 / m.m00);
            int cy = int(m.m01 / m.m00);
            putText(result, to_string(int(tooth_area)), Point(cx, cy), FONT_HERSHEY_SIMPLEX, 0.5, color, 1);

            if (tooth_area <= 1000 || tooth_area >= 1500) {
                circle(src, Point(cx, cy), 40, Scalar(0, 255, 255), 2, LINE_AA);
            }
        }
    }

    imshow("Original", src);
    imshow("Result", result);
    waitKey(0);
    return 0;
}
