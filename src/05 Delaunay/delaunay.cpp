#include "non_delaunay_01.h"
#include "delaunay_01.h"
#include "delaunay_02.h"

void Delaunay() {

	int width = 1000;
	int height = 1000;
	cv::Mat img = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);

	std::vector<cv::Point> all_point;
	srand(13);

	int size = 20;
	for (int i = 0; i < size; i++)
	{
		auto p1 = cv::Point(rand() % width, rand() % height);
		all_point.push_back(p1);
	}
	bool use_alg_bw =1;

	delaunay_01_datastruct::DECL decl;
	delaunay_02_datastruct::BW_DT_struct bw_dt_struct;

	if (!use_alg_bw) {
		decl.boundary = cv::Point2d(width, height);
		//Non_Delaunay_01(all_point, decl);
		Delaunay_01(all_point, decl);
	}
	else {
		Delaunay_02(all_point, bw_dt_struct);
	}

	//draw code begin
	debug_cout("\n*********start draw*********");

	if (!use_alg_bw) {
		for (size_t i = 0; i < decl.face_list.size(); i++) {

			auto face = decl.face_list[i];
			auto start_edge = face->incident_edge;

			do {
				if (start_edge->origin->position == cv::Point2d(0, 0) || start_edge->end->position == cv::Point2d(0, 0))
					;
				else if (start_edge->origin->position == cv::Point2d(width * 2, 0) || start_edge->end->position == cv::Point2d(width * 2, 0))
					;
				else if (start_edge->origin->position == cv::Point2d(0, height * 2) || start_edge->end->position == cv::Point2d(0, height * 2))
					;
				else
					draw_line_origin_buttom_left(width, height, img, start_edge->origin->position, start_edge->end->position, GREEN, 1);
				start_edge = start_edge->succ;
			} while (face->incident_edge != start_edge);
		}

		for (size_t i = 3; i < decl.vertex_list.size(); i++)
		{
			auto site = *decl.vertex_list[i];
			debug_cout("draw site:" + vector_to_string(site.position));
			draw_circle_origin_buttom_left(width, height, img, site.position, 2, RED, 2);
		}
	}
	else {
		for (size_t i = 0; i < bw_dt_struct.n_simplices_list.size(); i++) {

			auto simplice = bw_dt_struct.n_simplices_list[i];

			if (simplice.index_p1 < 3 || simplice.index_p2 < 3 || simplice.index_p3 < 3)
				continue;
			draw_line_origin_buttom_left(width, height, img, bw_dt_struct.all_point[simplice.index_p1], bw_dt_struct.all_point[simplice.index_p2], GREEN, 1);
			draw_line_origin_buttom_left(width, height, img, bw_dt_struct.all_point[simplice.index_p2], bw_dt_struct.all_point[simplice.index_p3], GREEN, 1);
			draw_line_origin_buttom_left(width, height, img, bw_dt_struct.all_point[simplice.index_p1], bw_dt_struct.all_point[simplice.index_p3], GREEN, 1);

		}

		for (size_t i = 3; i < bw_dt_struct.all_point.size(); i++)
		{
			auto site = bw_dt_struct.all_point[i];
			debug_cout("draw site:" + vector_to_string(site));
			draw_circle_origin_buttom_left(width, height, img, site, 2, RED, 2);
		}
	}





#define Grid_Size 10
#define Show_Grid 1
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