/* ------------------------------------------------------ /
*Image Proccessing with Deep Learning
* Created : 2025.03.21
* Autor : HWANG MooYeol
------------------------------------------------------ */
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main()
{
    vector<String> imageFiles = { "Gear1.jpg", "Gear2.jpg", "Gear3.jpg", "Gear4.jpg" };

    for (size_t idx = 0; idx < imageFiles.size(); idx++) {
        Mat src, src_gray, imthres, mask, result;
        src = imread(imageFiles[idx]);

        cvtColor(src, src_gray, COLOR_BGR2GRAY);
        threshold(src_gray, imthres, 127, 255, THRESH_BINARY);

        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;

        findContours(imthres, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

        //drawContours(src, contours, -1, Scalar(0,255,0), 2);

        Point2f center;
        float radius;
        minEnclosingCircle(contours[0], center, radius);

        //circle(src, center, radius, Scalar(255, 0, 0), 1, 4, 0);
        //circle(src, center, radius*0.805, Scalar(255, 0, 255), 1, 4, 0);

        mask = Mat::zeros(src.size(), CV_8UC1);
        circle(mask, center, radius * 0.805, Scalar(255), -1);

        Mat mask_inv;
        bitwise_not(mask, mask_inv);
        bitwise_and(src, src, result, mask_inv);

        vector<vector<Point>> toothContours;
        findContours(imthres - mask, toothContours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        string Quality_Inspection = "PASS";
        int Defective = 0;
        double avg_area = 0;

        for (size_t i = 0; i < toothContours.size(); i++) {
            double tooth_area = contourArea(toothContours[i]);
            avg_area += tooth_area;
            Moments m = moments(toothContours[i]);

            Scalar color = (tooth_area <= 1000 || tooth_area >= 1500) ? Scalar(0, 0, 255) : Scalar(0, 255, 0);

            drawContours(result, toothContours, (int)i, color, 1);
            double ratio = 0.23;

            if (m.m00 != 0) {
                int cx = int(m.m10 / m.m00);
                int cy = int(m.m01 / m.m00);

                string area_text = to_string(int(tooth_area));
                int baseline = 0;
                Size text_size = getTextSize(area_text, FONT_HERSHEY_SIMPLEX, 0.5, 1.5, &baseline);

                putText(result, area_text, Point((center.x) * ratio + (cx - text_size.width / 2) * (1 - ratio),
                    (center.y) * ratio + (cy + text_size.height / 2) * (1 - ratio)),
                    FONT_HERSHEY_SIMPLEX, 0.5, color, 1.5);

                if (tooth_area <= 1000 || tooth_area >= 1500) {
                    circle(src, Point(cx, cy), 35, Scalar(0, 255, 255), 2, LINE_AA);
                    Quality_Inspection = "FAIL";
                    Defective += 1;
                }
            }
        }

        avg_area = avg_area / toothContours.size();

        printf("----------------------------------------------------------------------------------------------\n");
        printf(" Image: %s\n Teeth number: %zu\n Avg, Teeth Area: %f\n Defective Teeth: %d\n Diameter: %f\n Quality Inspection: %s\n",
            imageFiles[idx].c_str(), toothContours.size(), avg_area, Defective, radius * 0.805 * 2, Quality_Inspection.c_str());
        printf("----------------------------------------------------------------------------------------------\n");

        imshow("Original " + to_string(idx + 1), src);
        imshow("Result " + to_string(idx + 1), result);
    }

    waitKey(0);
    return 0;
}
