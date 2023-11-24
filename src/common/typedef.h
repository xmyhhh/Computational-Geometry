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


#define  Infinity  std::numeric_limits<double>::infinity() 

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

struct PSLG {
	//In computational geometry, PSLGs have often been called planar subdivisions, 
	//with an assumption or assertion that subdivisions are polygonal rather than having curved boundaries.
	std::vector<cv::Point2d> Point;
	std::vector<Edge_Index> Segment;
	std::vector<Polygon> Hole;
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

