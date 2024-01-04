
#include "constrained_DT_01.h"
#include <common/vulkan/VulkanTools.h>
#include "CDT.h"

void constrained_DT() {
	//this alg in not working

	int width = 2000;
	int height = 800;
	cv::Mat img = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);

	std::vector<cv::Point> all_point;
	srand(13);


	using namespace CDT_01_datastruct;
	//load plsg
	PSLG plsg;

	plsg.offset = cv::Point2d(width / 8, height / 8);
	std::string plsg_path;
#define load_guita 0
	if (load_guita) {
		plsg.factor = 45;
		plsg_path = getAssetPath() + "guita.plsg";
	}
	else {
		plsg.factor = 10;
		plsg_path = getAssetPath() + "simple.plsg";
	}
	plsg.init_from_file(plsg_path);

	CDT_01_datastruct::MyCDT cdt;
	//CDT_01(plsg, cdt);

	CDT::Triangulation<double> cdt2;
	{
		cdt2.insertVertices({
	   {-2.0, 1.47656155},
	   {-6.40527344, -40.4999084},
	   {0.0, -7.96960115},
	   {-2.00152564, 1.46877956},
	   {-2.70361328, -7.99999619},
	   {-2.70465064, -7.99901962},
	   {-7.97778273, -19.3754253},
	   {7.96885204, -5.37488127},
	   {-7.97180128, -39.7499695},
			});
		cdt2.insertEdges({
			{0, 8},
			});
		//cdt2.eraseSuperTriangle();
		cdt2.eraseOuterTriangles();
		//cdt2.eraseOuterTrianglesAndHoles();
	}

	//draw code begin
	debug_cout("\n*********start draw*********");

#define draw_plsg 0
	//draw plsg
	draw_circle_origin_buttom_left(width, height, img, { 100,100 }, 2, BLUE, 2);
	if (draw_plsg) {
		for (size_t i = 0; i < plsg.boundary_array.size(); i++) {
			for (size_t j = 0; j < plsg.boundary_array[i].point_array.size(); j++) {
				draw_circle_origin_buttom_left(width, height, img, plsg.get_position(i, j), 2, RED, 2);

				if (j == 0) {
					draw_line_origin_buttom_left(width, height, img, plsg.get_position(i, j),
						plsg.get_position(i, plsg.boundary_array[i].point_array.size() - 1),
						GREEN,
						1);
				}
				else {
					draw_line_origin_buttom_left(width, height, img, plsg.get_position(i, j),
						plsg.get_position(i, j - 1),
						GREEN, 1);
				}
			}
		}
		for (size_t i = 0; i < plsg.vertex_array.size(); i++) {
			draw_circle_origin_buttom_left(width, height, img, plsg.get_position(i), 2, RED, 2);
		}
	}

#define draw_cdt 1
	//draw cdt
	if (draw_cdt) {
		for (size_t i = 0; i < cdt.number_of_constrained_edge; i++) {
			auto ce = cdt.constrained_edge_array[i];
			draw_line_origin_buttom_left(width, height, img, ce.orig->position, ce.end->position, GREEN, 1);
		}

		for (size_t i = 0; i < cdt.strip_array.size(); i++) {
			auto s = cdt.strip_array[i];
			if (s->region_left_x != -1) {
				draw_line_origin_buttom_left(width, height, img, cv::Point2d(s->region_left_x, 1000),
					cv::Point2d(s->region_left_x, -1000), GREEN, 1);
			}
			if (s->region_right_x != -1) {
				draw_line_origin_buttom_left(width, height, img, cv::Point2d(s->region_right_x, 1000),
					cv::Point2d(s->region_right_x, -1000), GREEN, 1);
			}

			for (size_t j = 0; j < s->region.size(); j++) {
				auto r = s->region[j];
				for (size_t k = 0; k < r->vertex_array.size(); k++) {
					auto vtx = r->vertex_array[k];
					draw_circle_origin_buttom_left(width, height, img, vtx->position, 2, RED, 2);
				}
				for (size_t k = 0; k < r->edge_array.size(); k++) {
					auto e = r->edge_array[k];
					draw_line_origin_buttom_left(width, height, img, e->orig->position,
						e->end->position, GREEN, 1);
				}
			}
		}
	}


#define draw_strip 0
	if (draw_strip) {
		for (auto& strip : cdt.strip_array) {
			for (auto& region : strip->region) {
				if (region->infinity_vtx.buttom_left)
					draw_circle_origin_buttom_left(width, height, img, region->infinity_vtx.buttom_left->position, 1,
						WHITE, 2);
				if (region->infinity_vtx.buttom_right)
					draw_circle_origin_buttom_left(width, height, img, region->infinity_vtx.buttom_right->position, 1,
						WHITE, 2);
				if (region->infinity_vtx.top_left)
					draw_circle_origin_buttom_left(width, height, img, region->infinity_vtx.top_left->position, 1,
						WHITE, 2);
				if (region->infinity_vtx.top_right)
					draw_circle_origin_buttom_left(width, height, img, region->infinity_vtx.top_right->position, 1,
						WHITE, 2);
			}

		}
	}

#define Grid_Size_w 20
#define Grid_Size_h 8
#define Show_Grid 0
	{
		if (Show_Grid) {
			for (size_t i = 0; i < Grid_Size_w; i++) {
				line(img, cv::Point(width / Grid_Size_w * i, 0), cv::Point(width / Grid_Size_w * i, height), WHITE, 1);
			}
			for (size_t i = 0; i < Grid_Size_h; i++) {
				line(img, cv::Point(0, height / Grid_Size_h * i), cv::Point(width, height / Grid_Size_h * i), WHITE, 1);
			}
		}
	}


	imshow("CDT", img);
	cv::waitKey(0);
}