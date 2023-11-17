#pragma once
#include "typedef.h"
bool InTriangleTest(cv::Point &p1, cv::Point &p2, cv::Point &p3, cv::Point &s);
bool ToLeft(cv::Point &p1, cv::Point &p2, cv::Point &p3);

template<typename PointT >
double Area2(PointT& p1, PointT& p2, PointT& s) {
	double value = p1.x * p2.y - p1.y * p2.x
		+ p2.x * s.y - p2.y * s.x
		+ s.x * p1.y - s.y * p1.x;
	return value;
}

IntersectionResult LineIntersectionCalulate(Line l1, Line l2);


Polygon PolygonRandomGen(int width, int height, int size = 3, float miniAngle = 30);

double VectorLengthSqr(cv::Point2d a, cv::Point2d b);
bool VectorSlop(cv::Point2d a, cv::Point2d b, double& slop);