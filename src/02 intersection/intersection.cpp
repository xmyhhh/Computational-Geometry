
#include "common/typedef.h"
#include "intersection_01.h"
using namespace base_type;
void Intersection() {

	int width = 1920 / 2;
	int height = 1920 / 2;
	cv::Mat img = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);

	std::vector<Line> all_lines;
	std::vector <IntersectionResult> intersectionResults;

	int size = 30;
	for (int i = 0; i < size; i++)
	{

		auto p1 = cv::Point(rand() % width, rand() % height);
		auto p2 = cv::Point(rand() % width, rand() % height);
		if (((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)) < 100000) {
			continue;
		}
		all_lines.push_back(
			{
			p1,
			p2
			}
		);
	}

	//{
	//	all_lines.push_back(
	//		{
	//		cv::Point(0,0) * 1.0,
	//		cv::Point(100,150) * 1.0
	//		}
	//	);
	//	all_lines.push_back(
	//		{
	//		cv::Point(0,100) * 1.0,
	//		cv::Point(100,0) * 1.0
	//		}
	//	);
	//}

	Intersection_01(all_lines, intersectionResults);

	//draw code begin
	for (size_t i = 0; i < all_lines.size(); i++)
	{
		all_lines[i].p1.y = height - all_lines[i].p1.y;
		all_lines[i].p2.y = height - all_lines[i].p2.y;
		line(img, all_lines[i].p1, all_lines[i].p2, WHITE, 1);
	}

	for (size_t i = 0; i < intersectionResults.size(); i++)
	{
		intersectionResults[i].l1.p1.y = height - intersectionResults[i].l1.p1.y;
		intersectionResults[i].l1.p2.y = height - intersectionResults[i].l1.p2.y;
		intersectionResults[i].l2.p1.y = height - intersectionResults[i].l2.p1.y;
		intersectionResults[i].l2.p2.y = height - intersectionResults[i].l2.p2.y;

		intersectionResults[i].intersectionPoint.y = height - intersectionResults[i].intersectionPoint.y;

		circle(img, intersectionResults[i].intersectionPoint, 2, RED, 2);
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


	imshow("Intersection", img);
	cv::waitKey(0);
}