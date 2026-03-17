#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int frame_cnt;
int frame = 0;
int hmin = 28, hmax = 110, smin = 4, smax = 161, vmin = 115, vmax = 250;
Mat frame1, frame2, video, hsv, dst, mask, mask_inv, background, current, result, bg;

int main(int ac, char** av) {

    VideoCapture cap("LAB2.mp4");
    VideoCapture bg1("background.mp4");

    frame_cnt = cap.get(CAP_PROP_FRAME_COUNT);

    if (!cap.isOpened())
    {
        printf("Can't open the camera");
        return -1;
    }

    // Get video properties
    int width = cap.get(CAP_PROP_FRAME_WIDTH);
    int height = cap.get(CAP_PROP_FRAME_HEIGHT);
    double fps = cap.get(CAP_PROP_FPS);

    // Define the codec and create VideoWriter object
    VideoWriter writer("output_result.avi", VideoWriter::fourcc('X', 'V', 'I', 'D'), fps, Size(width, height));

    bg1 >> frame1;        // 1st frame
    imshow("img1", frame1);
    bg = frame1;

    while (frame < frame_cnt - 1)
    {
        cap >> video;
        frame += 1;

        if (video.empty())
        {
            printf("empty image");
            break;
        }

        cvtColor(video, hsv, COLOR_BGR2HSV);
        inRange(hsv, Scalar(hmin, smin, vmin), Scalar(hmax, smax, vmax), dst);
        morphologyEx(dst, dst, MORPH_OPEN, getStructuringElement(MORPH_RECT, Size(4, 4)));

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
            bitwise_not(mask, mask_inv);

            background = bg & mask;
            current = video & mask_inv;
            result = current + background;

            imshow("result", result);
            writer.write(result);  // Save result frame
        }

        imshow("camera img", video);

        if (waitKey(25) == 27)
            break;
    }

    cap.release();
    bg1.release();
    writer.release();
    destroyAllWindows();

    return 0;
}
