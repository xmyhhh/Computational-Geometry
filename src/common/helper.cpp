#include "helper.h"

bool InTriangleTest(cv::Point &p1, cv::Point &p2, cv::Point &p3, cv::Point &s)
{
	bool b1 = ToLeft(p1, p2, s);
	bool b2 = ToLeft(p2, p3, s);
	bool b3 = ToLeft(p3, p1, s);

	return (b1 == b2) && (b2 == b3);
}

bool ToLeft(cv::Point &p1, cv::Point &p2, cv::Point &s)
{
	int value= p1.x * p2.y - p1.y * p2.x
		+ p2.x * s.y - p2.y * s.x
		+ s.x * p1.y - s.y * p1.x;
	return value > 0;
}
