#pragma once

#include "common/typedef.h"
#include "common/helper.h"
#include <queue>

void MakeMonotonicity(Polygon polygon) {

	class BinarySearchTree {

	};

	struct CompareHeight {
		bool operator()(cv::Point2d const& p1, cv::Point2d const& p2)
		{
			return p1.y < p2.y;
		}
	};

	std::priority_queue <cv::Point2d, std::vector<cv::Point2d>, CompareHeight> pq;

	for (auto point : polygon.Point) {
		pq.push(point);
	}



	while (!pq.empty()) {
		std::cout<< pq.top().y<<"\n";
		pq.pop();
	}
}

void Monotonicity() {

	int width = 1920 / 2;
	int height = 1920 / 2;
	cv::Mat img = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);

	std::vector<Line> all_lines;
	std::vector <IntersectionResult> intersectionResults;

	auto polygon = PolygonRandomGen(width, height, 15);
	MakeMonotonicity(polygon);

	//draw code begin
	for (size_t i = 0; i < polygon.Point.size(); i++)
	{
		polygon.Point[i].y = height - polygon.Point[i].y;
		circle(img, polygon.Point[i], 2, RED, 2);
	}

	for (size_t i = 0; i < polygon.Segment.size(); i++)
	{
		line(img, polygon.Point[polygon.Segment[i].p1], polygon.Point[polygon.Segment[i].p2], WHITE, 1);
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


	imshow("Polygon", img);
	cv::waitKey(0);



}