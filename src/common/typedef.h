#pragma once
#include "opencv2/opencv.hpp"
#include <vector>
#include <set>
#include <assert.h>
#include <random>
#include <iostream>
#include <string>
#include <cmath>

#define ASSERT(condition) assert(condition)
#define  WHITE cv::Scalar(255, 255, 255)
#define  RED cv::Scalar(0, 0, 255)
#define  GREEN cv::Scalar(0, 255, 0)
#define  BLUE cv::Scalar(255, 0, 0)
#define  YELLOW cv::Scalar(0, 255, 255)


#define  _Infinity  std::numeric_limits<double>::infinity() 

namespace base_type {
	struct Triangle {
		cv::Point p1;
		cv::Point p2;
		cv::Point p3;
	};

	struct Edge {
		cv::Point p1;
		cv::Point p2;
	};

	struct Line {
		cv::Point2d p1;
		cv::Point2d p2;
	};

	struct IntersectionResult {
		Line l1;
		Line l2;
		cv::Point2d intersectionPoint;
	};

	struct Triangle_Index {
		uint p1;
		uint p2;
		uint p3;
	};

	struct Edge_Index {
		uint p1;
		uint p2;
	};

	struct PolygonRandom {
		std::vector<cv::Point2d> Point;
		std::vector<Edge_Index> Segment;
	};
}

struct VulkanDrawData {
	int numberOfPoint = 0;
	double* points; //location xyz(3 double) * numberOfPoint
	int numberOfTriangle = 0;
	int* triangles;//index of triangle (3 int) * numberOfTriangle
	int numberOfPointAttr = 0;
	double* attr; //each point to size of((numberOfPointAttr double) * numberOfPoint)
};

namespace Interpolation3D_datastruct {
	struct Interpolation3DIO {
		int numberOfPoint;
		double* points; //location xyz and one attr(3+numberOfAttr double) * numberOfPoint

		int numberOfQueryPoints;
		double* queryPoints; //location xyz and one attr(3+numberOfAttr double) * numberOfPoint

		int numberOfAttr = 3;


		//use for debug 
		int vulkan_tri_num = 0;
		int* vulkan_tri;

		VulkanDrawData toVulkanDrawData() {
			VulkanDrawData vulkan_data;

			vulkan_data.numberOfTriangle = vulkan_tri_num;
			vulkan_data.triangles = (int*)malloc(sizeof(int) * 3 * vulkan_tri_num);
			memcpy(vulkan_data.triangles, vulkan_tri, sizeof(int) * 3 * vulkan_tri_num);


			vulkan_data.numberOfPoint = numberOfPoint + numberOfQueryPoints;
			vulkan_data.points = (double*)malloc(vulkan_data.numberOfPoint * 3 * sizeof(double));

			//point
			for (int i = 0; i < numberOfPoint; i++) {
				for (int j = 0; j < 3; j++) {
					vulkan_data.points[i * 3 + j] = points[i * (3 + numberOfAttr) + j];
				}
			}
			int offset = numberOfPoint * 3;
			for (int i = 0; i < numberOfQueryPoints; i++) {
				for (int j = 0; j < 3; j++) {
					vulkan_data.points[offset + i * 3 + j] = queryPoints[i * (3 + numberOfAttr) + j];
				}
			}
			//attr
			vulkan_data.numberOfPointAttr = numberOfAttr + 1;//first is type and then is real attr, type 0 is know point and type 1 is est point
			vulkan_data.attr = (double*)malloc(vulkan_data.numberOfPoint * vulkan_data.numberOfPointAttr * sizeof(double));
			for (int i = 0; i < numberOfPoint; i++) {
				for (int j = 0; j < numberOfAttr; j++) {
					vulkan_data.attr[i * vulkan_data.numberOfPointAttr + j] = points[i * (3 + numberOfAttr) + 3 + j];
				}
				vulkan_data.attr[i * vulkan_data.numberOfPointAttr + vulkan_data.numberOfPointAttr - 1] = 0;
			}

			offset = numberOfPoint * vulkan_data.numberOfPointAttr;

			for (int i = 0; i < numberOfQueryPoints; i++) {
				for (int j = 0; j < numberOfAttr; j++) {
					vulkan_data.attr[offset + i * vulkan_data.numberOfPointAttr + j] = queryPoints[i * (3 + numberOfAttr) + 3 + j];
				}
				vulkan_data.attr[offset + i * vulkan_data.numberOfPointAttr + vulkan_data.numberOfPointAttr - 1] = 1;
			}

			return vulkan_data;
		}
	};
}


namespace Delaunay3D_01_datastruct {

	struct n_simplices_face {
		n_simplices_face(int a, int b, int c) {
			index_p1 = std::min(std::min(a, b), c);
			index_p3 = std::max(std::max(a, b), c);
			if (index_p1 == a)
				index_p2 = std::min(b, c);
			else if (index_p1 == b)
				index_p2 = std::min(a, c);
			else
				index_p2 = std::min(a, b);
		}
		std::string get_key() {
			return std::to_string(index_p1) + " " + std::to_string(index_p2) + " " + std::to_string(index_p3);
		}

		int index_p1;
		int index_p2;
		int index_p3;

	};

	struct n_simplices {
		int index_p1;
		int index_p2;
		int index_p3;
		int index_p4;
		//Each (n + 1)-tuple of indices is associated with coordinates/radius of circumsphere
		cv::Point3d coordinates;
		double radius;

		bool mark_delation = false;
	};

	struct BW_DT_struct
	{
		//* A list of(n + 1)-tuples of indices to the nuclei or datapoints
		std::vector<n_simplices> n_simplices_list;
		std::vector<cv::Point3d> all_point;

		VulkanDrawData toVulkanDrawData() {
			VulkanDrawData data;

			data.numberOfPoint = all_point.size();
			data.points = (double*)malloc(data.numberOfPoint * 3 * sizeof(double));
			//memcpy(data.points, all_point.data(), data.numberOfPoint * 3 * sizeof(double));
			for (int i = 0; i < data.numberOfPoint; i++) {
				data.points[i * 3 + 0] = all_point[i].x;
				data.points[i * 3 + 1] = all_point[i].y;
				data.points[i * 3 + 2] = all_point[i].z;
			}
			data.numberOfPointAttr = 4;
			data.attr = (double*)malloc(sizeof(double) * data.numberOfPointAttr * data.numberOfPoint);
			for (int i = 0; i < data.numberOfPoint; i++) {
				if (i < 2) {
					data.attr[i * 4 + 0] = 254;
					data.attr[i * 4 + 1] = 0;
					data.attr[i * 4 + 2] = 0;
					data.attr[i * 4 + 3] = 1;
				}
				else if (i == 2) {
					data.attr[i * 4 + 0] = 0;
					data.attr[i * 4 + 1] = 0;
					data.attr[i * 4 + 2] = 254;
					data.attr[i * 4 + 3] = 1;
				}
				else if (i == 3) {
					data.attr[i * 4 + 0] = 0;
					data.attr[i * 4 + 1] = 254;
					data.attr[i * 4 + 2] = 0;
					data.attr[i * 4 + 3] = 1;
				}
				else {
					data.attr[i * 4 + 0] = 255;
					data.attr[i * 4 + 1] = 255;
					data.attr[i * 4 + 2] = 255;
					data.attr[i * 4 + 3] = 0;
				}
			}
			data.numberOfTriangle = n_simplices_list.size() * 4;
			data.triangles = (int*)malloc(data.numberOfTriangle * 4 * 3 * sizeof(int));
			for (int i = 0; i < n_simplices_list.size(); i++) {
				auto& tetrahedrons = n_simplices_list[i];
				data.triangles[i * 4 * 3 + 0 * 3] = tetrahedrons.index_p1;
				data.triangles[i * 4 * 3 + 0 * 3 + 1] = tetrahedrons.index_p2;
				data.triangles[i * 4 * 3 + 0 * 3 + 2] = tetrahedrons.index_p3;

				data.triangles[i * 4 * 3 + 1 * 3] = tetrahedrons.index_p1;
				data.triangles[i * 4 * 3 + 1 * 3 + 1] = tetrahedrons.index_p2;
				data.triangles[i * 4 * 3 + 1 * 3 + 2] = tetrahedrons.index_p4;

				data.triangles[i * 4 * 3 + 2 * 3] = tetrahedrons.index_p1;
				data.triangles[i * 4 * 3 + 2 * 3 + 1] = tetrahedrons.index_p3;
				data.triangles[i * 4 * 3 + 2 * 3 + 2] = tetrahedrons.index_p4;

				data.triangles[i * 4 * 3 + 3 * 3] = tetrahedrons.index_p2;
				data.triangles[i * 4 * 3 + 3 * 3 + 1] = tetrahedrons.index_p3;
				data.triangles[i * 4 * 3 + 3 * 3 + 2] = tetrahedrons.index_p3;
			}



			return data;
		}
	};

}