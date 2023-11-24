#pragma once
#include "typedef.h"

bool ToLeft(const cv::Point& p1, const cv::Point& p2, const cv::Point& p3);
bool ToLeft(const cv::Point2d& p1, const cv::Point2d& p2, const cv::Point2d& s);

bool InCircle2D(const cv::Point& p1, const cv::Point& p2, const cv::Point& p3, const cv::Point& s);

//geo tesl
template<typename PointT >
bool InTriangleTest(PointT& p1, PointT& p2, PointT& p3, const PointT& s) {
	bool b1 = ToLeft(p1, p2, s);
	bool b2 = ToLeft(p2, p3, s);
	bool b3 = ToLeft(p3, p1, s);
	return (b1 == b2) && (b2 == b3);
}

template<typename PointT >
double Area2(PointT& p1, PointT& p2, PointT& s) {
	double value = p1.x * p2.y - p1.y * p2.x
		+ p2.x * s.y - p2.y * s.x
		+ s.x * p1.y - s.y * p1.x;
	return value;
}

IntersectionResult LineIntersectionCalulate(Line l1, Line l2);


//geo cul
Polygon PolygonRandomGen(int width, int height, int size = 3, float miniAngle = 30);

double VectorLengthSqr(cv::Point2d a, cv::Point2d b);
bool VectorSlop(cv::Point2d a, cv::Point2d b, double& slop);
double DistanceToPoint(Line line, cv::Point2d point);
double Abs(double in);
//draw
void draw_line_origin_buttom_left(uint width, uint height, cv::InputOutputArray img, cv::Point pt1, cv::Point pt2, const cv::Scalar& color,
	int thickness = 1, int lineType = cv::LINE_8, int shift = 0);

void draw_circle_origin_buttom_left(uint width, uint height, cv::InputOutputArray img, cv::Point center, int radius,
	const cv::Scalar& color, int thickness = 1, int lineType = cv::LINE_8, int shift = 0);

//debug
void debug_cout(std::string msg, bool on = true);

std::string vector_to_string(cv::Point2d site);