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


struct Triangle_Index {
	uint p1;
	uint p2;
	uint p3;
};

struct Edge_Index {
	uint p1;
	uint p2;
};