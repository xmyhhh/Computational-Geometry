#include "common/typedef.h"
#include "common/helper.h"



using namespace Interpolation4D_datastruct;


void Interpolation4D_01(std::vector<cv::Point3d>& all_dots, Interpolation4DResult& res) {

	res.numberOfPoint = all_dots.size();
	res.points = (double*)malloc(res.numberOfPoint * 4 * sizeof(double));
	for (int i = 0; i < res.numberOfPoint; i++) {
		res.points[i * 4] = all_dots[i].x;
		res.points[i * 4 + 1] = all_dots[i].y;
		res.points[i * 4 + 2] = all_dots[i].z;
		res.points[i * 4 + 3] = 1;//attr
	}

}