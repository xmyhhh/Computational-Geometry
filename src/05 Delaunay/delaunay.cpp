#include "non_delaunay_01.h"
#include "delaunay_01.h"

void Delaunay() {

	int width = 1920 ;
	int height = 1080;
	cv::Mat img = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);

	std::vector<cv::Point> all_point;
	DECL_Delaunay::DECL decl;

	srand(12);

	int size = 20;
	for (int i = 0; i < size; i++)
	{
		auto p1 = cv::Point(rand() % width, rand() % height);
		all_point.push_back(p1);
	}

	decl.boundary = cv::Point2d(width, height);
	//Non_Delaunay_01(all_point, decl);
	Delaunay_01(all_point, decl);

	//draw code begin
	debug_cout("\n*********start draw*********");

	for (size_t i = 0; i < decl.face_list.size(); i++) {

		auto face = decl.face_list[i];
		auto start_edge = face->incident_edge;
		do {
			draw_line_origin_buttom_left(width, height, img, start_edge->origin->position, start_edge->end->position, GREEN, 1);
			start_edge = start_edge->succ;
		} while (face->incident_edge != start_edge);
	}

	for (size_t i = 0; i < decl.vertex_list.size(); i++)
	{
		auto site = *decl.vertex_list[i];
		debug_cout("draw site:" + vector_to_string(site.position));
		draw_circle_origin_buttom_left(width, height, img, site.position, 2, RED, 2);
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


	imshow("Delaunay", img);
	cv::waitKey(0);
}