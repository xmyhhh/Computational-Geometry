
#include "common/typedef.h"

#include "convex_hull_01.h"

void ConvexHull() {
	//方法一：三角形内部测试判断极点（可行，但是效率低）（n^4）

	int width = 1920 / 2;
	int height = 1920 / 2;
	cv::Mat img = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);

	std::vector<cv::Point> all_dots;
	std::vector<int> extreme_dots_index;

	int size = 1;
	for (int i = 0; i < size; i++)
	{

		all_dots.push_back(cv::Point(rand() % width, rand() % height));
	}

	ConvexHull_01(all_dots, extreme_dots_index);

	for (size_t i = 0; i < all_dots.size(); i++)
	{
		if (extreme_dots_index[i]) {
			circle(img, all_dots[i], 2, BLUE, 2);
		}
		else {
			circle(img, all_dots[i], 2, RED, 2);
		}
	}

	imshow("ConvexHull", img);
	cv::waitKey(0);
}