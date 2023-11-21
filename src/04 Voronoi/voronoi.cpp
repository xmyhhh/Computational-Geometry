#include "voronoi_01.h"






void Voronoi() {

	int width = 1920 / 2;
	int height = 1920 / 2;
	cv::Mat img = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);

	std::vector<cv::Point> all_point;
	std::vector<Edge_Index> all_edge;


	int size = 2;
	for (int i = 0; i < size; i++)
	{

		auto p1 = cv::Point(rand() % width, rand() % height);
		all_point.push_back(p1);

	}
	DECL::DECL decl;
	decl.boundary = cv::Point2d(width, height);
	Voronoi_01(all_point, decl);

	for (size_t i = 0; i < decl.site_list.size(); i++)
	{
		auto site = decl.site_list[i];
		auto face = decl.site_list[i].incident_face;

		draw_circle_origin_buttom_left(width, height, img, site.position, 2, RED, 2);
		auto start_edge = face->incident_edge;

		do {
			draw_line_origin_buttom_left(width, height, img, start_edge->origin->position, start_edge->end->position, BLUE, 5);
			start_edge = start_edge->succ;
		} while (face->incident_edge != start_edge);

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