#pragma once
#include "typedef.h"

//geo tesl
bool ToLeft(const cv::Point& p1, const cv::Point& p2, const cv::Point& p3);
bool ToLeft(const cv::Point2d& p1, const cv::Point2d& p2, const cv::Point2d& s);

bool InCircle2D(const cv::Point2d& p1, const cv::Point2d& p2, const cv::Point2d& p3, const cv::Point2d& s);

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

template<typename PointT >
void CalculateBoundingSphere(const PointT& p1, const PointT& p2, const PointT& p3, const PointT& p4, PointT& center, double& radius)
{
	double a = p1.x - p2.x, b = p1.y - p2.y, c = p1.z - p2.z;
	double a1 = p3.x - p4.x, b1 = p3.y - p4.y, c1 = p3.z - p3.z;
	double a2 = p2.x - p3.x, b2 = p2.y - p3.y, c2 = p2.z - p3.z;
	double A = p1.x * p1.x - p2.x * p2.x;
	double B = p1.y * p1.y - p2.y * p2.y;
	double C = p1.z * p1.z - p2.z * p2.z;
	double A1 = p3.x * p3.x - p4.x * p4.x;
	double B1 = p3.y * p3.y - p4.y * p4.y;
	double C1 = p3.z * p3.z - p4.z * p4.z;
	double A2 = p2.x * p2.x - p3.x * p3.x;
	double B2 = p2.y * p2.y - p3.y * p3.y;
	double C2 = p2.z * p2.z - p3.z * p3.z;
	double P = (A + B + C) / 2;
	double Q = (A1 + B1 + C1) / 2;
	double R = (A2 + B2 + C2) / 2;


	double D = a * b1 * c2 + a2 * b * c1 + c * a1 * b2 - (a2 * b1 * c + a1 * b * c2 + a * b2 * c1);
	double Dx = P * b1 * c2 + b * c1 * R + c * Q * b2 - (c * b1 * R + P * c1 * b2 + Q * b * c2);
	double Dy = a * Q * c2 + P * c1 * a2 + c * a1 * R - (c * Q * a2 + a * c1 * R + c2 * P * a1);
	double Dz = a * b1 * R + b * Q * a2 + P * a1 * b2 - (a2 * b1 * P + a * Q * b2 + R * b * a1);


	center.x = (Dx / D);
	center.y = (Dy / D);
	center.z = (Dz / D);
	radius = std::sqrt((p1.x - center.x) * (p1.x - center.x) +
		(p1.y - center.y) * (p1.y - center.y) +
		(p1.z - center.z) * (p1.z - center.z));
}

template<typename PointT >
void CalculateBoundingCircle(const PointT& p1, const PointT& p2, const PointT& p3, PointT& center, double& radius)
{
	// Calculating lengths of the sides of the triangle formed by the coordinates
	double x1 = p1.x, y1 = p1.y, x2 = p2.x, y2 = p2.y, x3 = p3.x, y3 = p3.y;

	double a = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
	double b = std::sqrt((x3 - x1) * (x3 - x1) + (y3 - y1) * (y3 - y1));
	double c = std::sqrt((x3 - x2) * (x3 - x2) + (y3 - y2) * (y3 - y2));

	// Calculating the radius of the circumscribed circle using triangle sides
	radius = (a * b * c) / (sqrt((a + b + c) * (b + c - a) * (c + a - b) * (a + b - c)));

	// Calculating the coordinates of the center of the circumscribed circle (x, y)
	double d = 2 * (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));
	center.x = ((x1 * x1 + y1 * y1) * (y2 - y3) + (x2 * x2 + y2 * y2) * (y3 - y1) + (x3 * x3 + y3 * y3) * (y1 - y2)) / d;
	center.y = ((x1 * x1 + y1 * y1) * (x3 - x2) + (x2 * x2 + y2 * y2) * (x1 - x3) + (x3 * x3 + y3 * y3) * (x2 - x1)) / d;
}


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

