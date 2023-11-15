#include "voronoi_01.h"

void Voronoi() {

	int width = 1920 / 2;
	int height = 1920 / 2;
	cv::Mat img = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);

	std::vector<cv::Point> all_point;
	std::vector<Edge_Index> all_edge;


	int size = 30;
	for (int i = 0; i < size; i++)
	{

		auto p1 = cv::Point(rand() % width, rand() % height);
		all_point.push_back(p1);

	}



	//draw code begin
	for (size_t i = 0; i < all_point.size(); i++)
	{
		circle(img, all_point[i], 2, RED, 2);
	}

	for (size_t i = 0; i < all_edge.size(); i++)
	{

		line(img, all_point[all_edge[i].p1], all_point[all_edge[i].p2], WHITE, 1);
	}

#define Grid_Size 10
#define Show_Grid 0
	{
		if (Show_Grid) {
			for (size_t i = 0; i < Grid_Size; i++)
			{
				line(img, cv::Point(width / Grid_Size * i, 0), cv::Point(width / Grid_Size * i, height), WHITE, 1);
			}
			for (size_t i = 0; i < Grid_Size; i++)
			{
				line(img, cv::Point(0, height / Grid_Size * i), cv::Point(width, height / Grid_Size * i), WHITE, 1);
			}
		}
	}


	imshow("Voronoi", img);
	cv::waitKey(0);
}