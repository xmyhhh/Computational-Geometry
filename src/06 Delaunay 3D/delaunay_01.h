#include "common/typedef.h"
#include "common/helper.h"



using namespace Delaunay3D_datastruct;


void Delaunay_3D_01(std::vector<cv::Point3d>& all_dots, DelaunayTrianlgeResult& res) {


	res.numberOfPoint = all_dots.size();
	res.points = (double*)malloc(res.numberOfPoint * 3 * sizeof(double));
	memcpy(res.points, all_dots.data(), res.numberOfPoint * 3 * sizeof(double));

	res.numberOfTetrahedron = 1;
	res.tetrahedrons = (int*)malloc(sizeof(int) * 4);
	res.tetrahedrons[0] = 0;
	res.tetrahedrons[1] = 1;
	res.tetrahedrons[2] = 2;
	res.tetrahedrons[3] = 3;
}