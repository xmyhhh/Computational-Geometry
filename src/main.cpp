#include "opencv2/opencv.hpp"

using namespace cv;
int main() {

	Mat img = Mat::zeros(Size(1920/2, 1080/2), CV_8UC3);
	imshow("Delaunay", img);
	waitKey(0);
	return 0;
}