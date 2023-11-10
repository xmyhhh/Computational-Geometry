
#include "common/typedef.h"


void ShowDot() {

	int width = 1920 / 2;
	int height = 1080 / 2;
	cv::Mat img = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);

	std::vector<cv::Point> all_Dots;

	int size = 3;
	for (int i = 0; i < size; i++)
	{
		all_Dots.push_back(cv::Point(rand() % width, rand() % height));
	}

	for (size_t i = 0; i < all_Dots.size(); i++)
	{
		circle(img, all_Dots[i], 2, RED, 2);
	}


	imshow("Dot", img);
	cv::waitKey(0);
}