
#include "constrained_DT_01.h"
#include <common/vulkan/VulkanTools.h>


void constrained_DT() {

	int width = 2000;
	int height = 700;
	cv::Mat img = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);

	std::vector<cv::Point> all_point;
	srand(13);



	using namespace CDT_01_datastruct;
	//load plsg 
	PSLG plsg;
	plsg.factor = 25;
	plsg.offset = cv::Point2d(width / 4, height / 4);
	auto plsg_path = getAssetPath() + "guita.plsg";
	plsg.init_from_file(plsg_path);

	CDT cdt;
	CDT_01(plsg, cdt);


	//draw code begin
	debug_cout("\n*********start draw*********");


	for (size_t i = 0; i < plsg.boundary_array.size(); i++)
	{
		for (size_t j = 0; j < plsg.boundary_array[i].point_array.size(); j++) {
			draw_circle_origin_buttom_left(width, height, img, plsg.get_position(i, j), 2, RED, 2);

			if (j == 0) {
				draw_line_origin_buttom_left(width, height, img, plsg.get_position(i, j), plsg.get_position(i, plsg.boundary_array[i].point_array.size() - 1), GREEN, 1);
			}
			else {
				draw_line_origin_buttom_left(width, height, img, plsg.get_position(i, j), plsg.get_position(i, j - 1), GREEN, 1);
			}
		}
	}
	for (size_t i = 0; i < plsg.vertex_array.size(); i++) {
		draw_circle_origin_buttom_left(width, height, img, plsg.get_position(i), 2, RED, 2);
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


	imshow("CDT", img);
	cv::waitKey(0);
}