#pragma once
#include "typedef.h"
bool InTriangleTest(cv::Point& p1, cv::Point& p2, cv::Point& p3, cv::Point& s);
bool ToLeft(cv::Point& p1, cv::Point& p2, cv::Point& p3);
bool ToLeft(cv::Point2d& p1, cv::Point2d& p2, cv::Point2d& s);
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


void draw_line_origin_buttom_left(uint width, uint height, cv::InputOutputArray img, cv::Point pt1, cv::Point pt2, const cv::Scalar& color,
	int thickness = 1, int lineType = cv::LINE_8, int shift = 0);

void draw_circle_origin_buttom_left(uint width, uint height, cv::InputOutputArray img, cv::Point center, int radius,
	const cv::Scalar& color, int thickness = 1, int lineType = cv::LINE_8, int shift = 0);