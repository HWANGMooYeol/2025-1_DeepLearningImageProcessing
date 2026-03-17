#include <opencv2/opencv.hpp>
#include <iostream>
#include <deque>  // 최근 6개 사각형을 저장하기 위한 큐

using namespace cv;
using namespace std;

Point origin;
Rect selection;
bool selectObject = false;
bool trackObject = false;
int hmin = 1, hmax = 179, smin = 30, smax = 255, vmin = 0, vmax = 255;

// 최대 6개 사각형 저장
deque<Rect> recentBoxes;

static void onMouse(int event, int x, int y, int, void* userdata)
{
    Mat* img = (Mat*)userdata;

    if (selectObject)
    {
        selection.x = MIN(x, origin.x);
        selection.y = MIN(y, origin.y);
        selection.width = abs(x - origin.x) + 1;
        selection.height = abs(y - origin.y) + 1;
        selection &= Rect(0, 0, img->cols, img->rows);
    }

    switch (event)
    {
    case EVENT_LBUTTONDOWN:
        selectObject = true;
        origin = Point(x, y);
        break;
    case EVENT_LBUTTONUP:
        selectObject = false;
        if (selection.area())
            trackObject = true;
        break;
    }
}

int main()
{
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "카메라를 열 수 없습니다.\n";
        return -1;
    }

    Mat frame, image_disp, hsv, dst, dst_track;
    namedWindow("Source", 0);
    setMouseCallback("Source", onMouse, &frame);

    createTrackbar("Hmin", "Source", &hmin, 179);
    createTrackbar("Hmax", "Source", &hmax, 179);
    createTrackbar("Smin", "Source", &smin, 255);
    createTrackbar("Smax", "Source", &smax, 255);
    createTrackbar("Vmin", "Source", &vmin, 255);
    createTrackbar("Vmax", "Source", &vmax, 255);

    while (true)
    {
        cap >> frame;
        if (frame.empty()) break;

        frame.copyTo(image_disp);
        dst_track = Mat::zeros(frame.size(), CV_8UC3);

        cvtColor(frame, hsv, COLOR_BGR2HSV);
        blur(hsv, hsv, Size(3, 3));
        inRange(hsv, Scalar(hmin, smin, vmin), Scalar(hmax, smax, vmax), dst);
        morphologyEx(dst, dst, MORPH_CLOSE, getStructuringElement(MORPH_RECT, Size(3, 3)));

        imshow("Source", image_disp);
        imshow("InRange", dst);

        if (trackObject)
        {
            trackObject = false;
            Mat roi_HSV(hsv, selection);
            Scalar means, stddev;
            meanStdDev(roi_HSV, means, stddev);

            hmin = max((int)(means[0] - stddev[0]), 0);
            hmax = min((int)(means[0] + stddev[0]), 179);
            smin = max((int)(means[1] - stddev[1]), 0);
            smax = min((int)(means[1] + stddev[1]), 255);
            vmin = max((int)(means[2] - stddev[2]), 0);
            vmax = min((int)(means[2] + stddev[2]), 255);

            setTrackbarPos("Hmin", "Source", hmin);
            setTrackbarPos("Hmax", "Source", hmax);
            setTrackbarPos("Smin", "Source", smin);
            setTrackbarPos("Smax", "Source", smax);
            setTrackbarPos("Vmin", "Source", vmin);
            setTrackbarPos("Vmax", "Source", vmax);
        }

        if (selectObject && selection.area() > 0)
        {
            Mat roi_RGB(image_disp, selection);
            bitwise_not(roi_RGB, roi_RGB);
        }

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

            Rect box = boundingRect(contours[maxIdx]);

            // 최근 6개 박스만 저장
            if (recentBoxes.size() >= 6)
                recentBoxes.pop_front();
            recentBoxes.push_back(box);

            // 모두 그리기
            for (const Rect& r : recentBoxes)
                rectangle(dst_track, r, Scalar(255, 0, 255), 2);

            drawContours(image_disp, contours, maxIdx, Scalar(0, 0, 255), 2);
            rectangle(image_disp, box, Scalar(255, 0, 255), 2);
        }

        imshow("Contour_Box", image_disp);
        imshow("Contour_Track", dst_track);

        if ((char)waitKey(10) == 27) break;
    }

    cap.release();
    destroyAllWindows();
    return 0;
}
