#pragma once
#include "opencv2/opencv.hpp"
#include <vector>
#include <random>


#define  WHITE cv::Scalar(255, 255, 255)
#define  RED cv::Scalar(0, 0, 255)
#define  GREEN cv::Scalar(0, 255, 0)
#define  BLUE cv::Scalar(255, 0, 0)
#define  YELLOW cv::Scalar(0, 255, 255)

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