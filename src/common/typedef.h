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

struct Polygon {
	std::vector<cv::Point2d> Point;
	std::vector<Edge_Index> Segment;
};


struct VulkanDrawData {
	int numberOfPoint = 0;
	double* points; //location xyz(3 double) * numberOfPoint
	int numberOfTriangle = 0;
	int* triangles;//index of triangle (3 int) * numberOfTriangle
	int numberOfPointAttr = 0;
	double* attr; //each point to size of((numberOfPointAttr double) * numberOfPoint)

};


namespace DECL_Delaunay {
	//doubly connected edge list (DCEL), https://github.com/AnkurRyder/DCEL

	class Vertex
	{
	public:
		uint id;
		cv::Point2d position;
		class HalfEdge* incident_edge;
		Vertex(cv::Point2d _position) {
			position = _position;
			incident_edge = nullptr;
		}
		Vertex() {}
	};

	class HalfEdge
	{
	public:
		uint id;
		HalfEdge* twin;
		HalfEdge* pred;
		HalfEdge* succ;
		Vertex* origin, * end;
		class Face* incident_face;
		bool isBorder = false;
		HalfEdge(Vertex* _origin, Vertex* _end) {
			origin = _origin;
			end = _end;
			incident_face = nullptr;
			twin = nullptr;
			pred = nullptr;
			succ = nullptr;
		}
	};

	class Face
	{
	public:
		int id;
		HalfEdge* incident_edge;
		Face() {
			incident_edge = nullptr;
		}
	};

	class DECL {
	public:
		std::vector<Vertex*> vertex_list;
		std::vector<Face*> face_list;
		std::vector<HalfEdge*> edge_list;

		cv::Point2d boundary;

		void AddVertex(Vertex* v) {
			vertex_list.push_back(v);
		}
		void DelVertex(Vertex* v) {
			for (auto iter = vertex_list.begin(); iter != vertex_list.end(); iter++) {
				if (*iter == v) {
					vertex_list.erase(iter);
					break;
				}
			}
		}

		void AddFace(Face* v) {
			face_list.push_back(v);
		}
		void DelFace(Face* v) {
			for (auto iter = face_list.begin(); iter != face_list.end(); iter++) {
				if (*iter == v) {
					face_list.erase(iter);
					break;
				}
			}
		}

		void AddEdge(HalfEdge* v) {
			edge_list.push_back(v);
		}
		void DelEdge(HalfEdge* v) {
			for (auto iter = edge_list.begin(); iter != edge_list.end(); iter++) {
				if (*iter == v) {
					edge_list.erase(iter);
					break;
				}
			}
		}

		DECL() {
			vertex_list.reserve(300);
			face_list.reserve(300);
			edge_list.reserve(300);
		}
	};
}



namespace Delaunay3D_datastruct {

	struct DelaunayTrianlgeResult {
		int numberOfPoint;
		double* points; //location xyz(3 double) * numberOfPoint
		int numberOfTetrahedron;
		int* tetrahedrons;//index of tetrahedron (4 int) * numberOfTetrahedron


		VulkanDrawData toVulkanDrawData() {
			VulkanDrawData data;

			data.numberOfPoint = numberOfPoint;
			data.points = (double*)malloc(data.numberOfPoint * 3 * sizeof(double));
			memcpy(data.points, points, data.numberOfPoint * 3 * sizeof(double));

			data.numberOfTriangle = numberOfTetrahedron * 4;
			data.triangles = (int*)malloc(numberOfTetrahedron * 4 * 3 * sizeof(int));
			for (int i = 0; i < numberOfTetrahedron; i++) {

				data.triangles[i * 4 + 0 * 3] = tetrahedrons[i * 4 + 0];
				data.triangles[i * 4 + 0 * 3 + 1] = tetrahedrons[i * 4 + 1];
				data.triangles[i * 4 + 0 * 3 + 2] = tetrahedrons[i * 4 + 2];

				data.triangles[i * 4 + 1 * 3] = tetrahedrons[i * 4 + 0];
				data.triangles[i * 4 + 1 * 3 + 1] = tetrahedrons[i * 4 + 1];
				data.triangles[i * 4 + 1 * 3 + 2] = tetrahedrons[i * 4 + 3];

				data.triangles[i * 4 + 2 * 3] = tetrahedrons[i * 4 + 0];
				data.triangles[i * 4 + 2 * 3 + 1] = tetrahedrons[i * 4 + 2];
				data.triangles[i * 4 + 2 * 3 + 2] = tetrahedrons[i * 4 + 3];

				data.triangles[i * 4 + 3 * 3] = tetrahedrons[i * 4 + 1];
				data.triangles[i * 4 + 3 * 3 + 1] = tetrahedrons[i * 4 + 2];
				data.triangles[i * 4 + 3 * 3 + 2] = tetrahedrons[i * 4 + 3];

			}

			return data;
		}
	};



}


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