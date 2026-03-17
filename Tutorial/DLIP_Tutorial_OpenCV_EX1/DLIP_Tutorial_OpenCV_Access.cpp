//#include <iostream>
//#include <opencv2/opencv.hpp>
//
//using namespace std;
//using namespace cv;
//
//int main()
//{
//	/*  read image  */
//	Mat img = imread("../../Image/symbol.jpg");
//	imshow("img", img);
//
//	/*  Crop(Region of Interest)  */
//	Rect r(10, 10, 150, 150);	 // (x, y, width, height)
//	Mat roiImg = img(r);
//	imshow("roiImg", roiImg);
//
//	/*  Rotate  */
//	Mat rotImg;
//	rotate(img, rotImg, ROTATE_90_CLOCKWISE);
//	imshow("rotImg", rotImg);
//
//	/*  Resize  */
//	Mat resizedImg;
//	resize(img, resizedImg, Size(1000, 100));
//	imshow("resizedImg", resizedImg);
//
//	waitKey(0);
//}

#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

int main()
{
    // 원본 이미지 읽기
    Mat img = imread("../../Image/symbol.jpg");
    if (img.empty()) {
        cout << "Image not found!" << endl;
        return -1;
    }

    // HSV 변환
    Mat hsv;
    cvtColor(img, hsv, COLOR_BGR2HSV);

    // 파란색 영역 마스크 만들기 (심볼 부분을 잡기 위해 배경 제외)
    Mat mask;
    inRange(hsv, Scalar(90, 50, 50), Scalar(140, 255, 255), mask); // 파란색 범위

    // 심볼만 남기기 위해 반전
    //bitwise_not(mask, mask);

    // 출력 이미지 (모두 파란색 배경으로 초기화)
    Mat output(img.size(), CV_8UC3, Scalar(226, 108, 73)); // BGR값: (B=242, G=110, R=78) → #4E6EF2는 나중에 변환

    // 심볼 부분을 흰색으로 채움
    output.setTo(Scalar(255, 255, 255), mask);

    imshow("Result", output);
    imwrite("result.png", output);  // 결과 저장
    waitKey(0);


}
