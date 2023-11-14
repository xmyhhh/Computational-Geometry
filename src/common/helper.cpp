#include "helper.h"

bool InTriangleTest(cv::Point& p1, cv::Point& p2, cv::Point& p3, cv::Point& s)
{
	bool b1 = ToLeft(p1, p2, s);
	bool b2 = ToLeft(p2, p3, s);
	bool b3 = ToLeft(p3, p1, s);

	return (b1 == b2) && (b2 == b3);
}

bool ToLeft(cv::Point& p1, cv::Point& p2, cv::Point& s)
{
	double value = p1.x * p2.y - p1.y * p2.x
		+ p2.x * s.y - p2.y * s.x
		+ s.x * p1.y - s.y * p1.x;
	return value > 0;
}


IntersectionResult LineIntersectionCalulate(Line l1, Line l2) {
	double k1 = (l1.p1.y - l1.p2.y) / (l1.p1.x - l1.p2.x);
	double k2 = (l2.p1.y - l2.p2.y) / (l2.p1.x - l2.p2.x);

	double inter_x = (k1 * l1.p1.x - k2 * l2.p1.x - l1.p1.y + l2.p1.y) / (k1 - k2);
	double inter_y = k2 * (inter_x - l2.p1.x) + l2.p1.y;
	return  { l1 ,l2 ,{inter_x, inter_y} };
}