
#include "common/typedef.h"

#include "convex_hull_01.h"

void ConvexHull() {


	int width = 1920 / 2;
	int height = 1920 / 2;
	cv::Mat img = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);

	std::vector<cv::Point> all_dots;
	std::vector<int> extreme_dots_index;

	int size = 300;
	for (int i = 0; i < size; i++)
	{

		all_dots.push_back(cv::Point(rand() % width, rand() % height));
	}

	//ConvexHull_01(all_dots, extreme_dots_index);
	ConvexHull_02(all_dots, extreme_dots_index);
	//int last_extreme_point_index = -1;
	for (size_t i = 0; i < all_dots.size(); i++)
	{
		if (extreme_dots_index[i]) {
			circle(img, all_dots[i], 2, BLUE, 4);
			//if (last_extreme_point_index != -1) {
			//	line(img, cv::Point(all_dots[last_extreme_point_index].x, all_dots[last_extreme_point_index].y), cv::Point(all_dots[i].x, all_dots[i].y), WHITE, 2);
			//}
			//last_extreme_point_index = i;
		}
		else {
			circle(img, all_dots[i], 2, RED, 2);
		}
	}

	imshow("ConvexHull", img);
	cv::waitKey(0);
}