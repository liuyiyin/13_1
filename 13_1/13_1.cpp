// 13_1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv.hpp>
using namespace cv;
using namespace std;
int calcHOG(cv::Mat src, float * hist, int nAngle, int cellSize)
{
	if (cellSize> src.cols || cellSize> src.rows) {
		return -1;
	}
    int nX = src.cols / cellSize;
    int nY = src.rows / cellSize;
	int binAngle = 360 / nAngle;
	Mat gx, gy;
	Mat mag, angle;
	Sobel(src, gx, CV_32F, 1, 0, 1);
	Sobel(src, gy, CV_32F, 0, 1, 1);
	cartToPolar(gx, gy, mag, angle, true);
	cv::Rect roi;
	roi.x = 0;
	roi.y = 0;
	roi.width = cellSize;
	roi.height = cellSize;
	for (int i = 0; i < nY; i++) {
		for (int j = 0; j < nX; j++) {
			cv::Mat roiMat;
			cv::Mat roiMag;
			cv::Mat roiAgl;
			roi.x = j*cellSize;
			roi.y = i*cellSize;
			roiMat = src(roi);
			roiMag = mag(roi);
			roiAgl = angle(roi);
			int head = (i*nX + j)*nAngle;
			for (int n = 0; n < roiMat.rows; n++) {
				for (int m = 0; m < roiMat.cols; m++) {
					int pos = (int)(roiAgl.at<float>(n, m) / binAngle);
					hist[head + pos] += roiMag.at<float>(n, m);
				}
			}
		}
	}
	return 0;
}
float normL2(float * Hist1, float * Hist2, int size)
{
	float sum = 0;
	for (int i = 0; i < size; i++) {
		sum += (Hist1[i] - Hist2[i])*(Hist1[i] - Hist2[i]);
	}
	sum = sqrt(sum);
	return sum;
}
int main()
{
	cv::Mat temp = cv::imread("D:\\template.png", 0);
	cv::Mat img = cv::imread("D:\\img.png", 0);
	int nAngle = 8;
	int blockSize = 16;
	int nX = temp.cols / blockSize;
	int nY = temp.rows / blockSize;
	int bins = nX*nY*nAngle;
	int aa = 0;
	int bb = 0;
	float s = 100000;
	float * temp_hist = new float[bins];
	memset(temp_hist, 0, sizeof(float) * bins);
	int reCode = 0;

	reCode = calcHOG(temp, temp_hist, nAngle, blockSize);
	if (reCode != 0) {
		return -1;
	}
  
	for (int a = 0; a < img.rows - temp.rows; ) {
		for (int b = 0; b < img.cols - temp.cols; ) {
			Rect m( b, a, temp.cols,temp.rows);
			Mat img1 = img(m);
			float * img_hist = new float[bins];
			memset(img_hist, 0, sizeof(float) * bins);
			reCode = calcHOG(img1, img_hist, nAngle, blockSize);
			if (reCode != 0) {
				return -1;
			}
			float dis1 = normL2(temp_hist, img_hist, bins);
			if (dis1 < s) {
				s = dis1;
				aa = a;
				bb = b;
			}
			delete[] img_hist;
			b = b + 3;
		}
		a = a + 3;
	}
	cv::Rect rect;
	rect.x = aa;
	rect.y = bb;
	rect.width = 108;
	rect.height = 48;
	rectangle(img, rect, CV_RGB(255, 0, 0), 1, 8, 0);

	cv::imshow("img", img);
	waitKey(0);
	return 0;
}

