#pragma once
#include "opencv2/opencv.hpp"
#include <vector>
#include <random>


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




namespace DECL {
	//doubly connected edge list (DCEL), https://github.com/AnkurRyder/DCEL

	class Vertex
	{
	public:
		uint id;
		cv::Point2d position;
		class HalfEdge* incident_edge;
		Vertex(uint _id, cv::Point2d _position) {
			id = _id;
			position = _position;
			incident_edge = nullptr;
		}

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

		HalfEdge(uint _id, Vertex* _origin, Vertex* _end) {
			id = _id;
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
		class Site* incident_site;
		Face(uint _id) {
			id = _id;
			incident_edge = nullptr;
			incident_site = nullptr;
		}
	};

	class Site {
	public:
		uint id;
		cv::Point2d position;
		Face* incident_face;
		Site(uint _id, cv::Point2d _position) {
			id = _id;
			position = _position;
		}
	};

	class DECL {
	public:
		std::vector<Face> face_list;
		std::vector<HalfEdge> halfEdge_list;
		std::vector<Vertex> vertex_list;
		std::vector<Site>site_list;

		void InsertSite(cv::Point2d position) {
			site_list.push_back({ site_list.size(), position });
		}

		void InsertVertex(cv::Point2d position) {
			vertex_list.push_back({ vertex_list.size(), position });
		}
		void InsertHalfEdge() {

		}

	};
}