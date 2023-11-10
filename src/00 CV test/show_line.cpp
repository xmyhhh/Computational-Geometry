
#include "common/typedef.h"


void ShowLine() {

	int width = 1920 / 2;
	int height = 1080 / 2;
	cv::Mat img = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);

	std::vector<cv::Point> all_Dots;

	int size = 15;
	for (int i = 0; i < size; i++)
	{
		all_Dots.push_back(cv::Point(rand() % width, rand() % height));
	}

	for (size_t i = 0; i < all_Dots.size(); i++)
	{
		line(img, cv::Point(all_Dots[i].x, all_Dots[i].y), cv::Point(all_Dots[0].x, all_Dots[0].y), WHITE, 1);
	}


	imshow("Line", img);
	cv::waitKey(0);
}