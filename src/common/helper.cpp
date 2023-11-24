#include "helper.h"

extern void Intersection_01(std::vector<Line>& all_lines, std::vector <IntersectionResult>& intersectionResults);





bool ToLeft(const cv::Point& p1, const cv::Point& p2, const cv::Point& s)
{
	double value = p1.x * p2.y - p1.y * p2.x
		+ p2.x * s.y - p2.y * s.x
		+ s.x * p1.y - s.y * p1.x;
	return value > 0;
}


bool ToLeft(const cv::Point2d& p1, const cv::Point2d& p2, const cv::Point2d& s)
{
	double value = p1.x * p2.y - p1.y * p2.x
		+ p2.x * s.y - p2.y * s.x
		+ s.x * p1.y - s.y * p1.x;
	return value > 0.0;
}

bool InCircle2D(const cv::Point& p1, const cv::Point& p2, const cv::Point& p3, const cv::Point& s)
{
	double value = 0;
	value += (p1.x - s.x) *
		(
			(p2.y - s.y) * (std::pow(p3.x - s.x, 2) + std::pow(p3.y - s.y, 2)) -
			(p3.y - s.y) * (std::pow(p2.x - s.x, 2) + std::pow(p2.y - s.y, 2))
			);

	value += -(p1.y - s.y) *
		(
			(p2.x - s.x) * (std::pow(p3.x - s.x, 2) + std::pow(p3.y - s.y, 2)) -
			(p3.x - s.x) * (std::pow(p2.x - s.x, 2) + std::pow(p2.y - s.y, 2))
			);

	value += (p2.x - s.x) * (p3.y - s.y) - (p2.y - s.y) * (p3.x - s.x);

	return value > 0.0;
};


IntersectionResult LineIntersectionCalulate(Line l1, Line l2) {
	double inter_x;
	double inter_y;

	if (l1.p1.x != l1.p2.x && l2.p1.x != l2.p2.x) {
		double k1 = (l1.p1.y - l1.p2.y) / (l1.p1.x - l1.p2.x);
		double k2 = (l2.p1.y - l2.p2.y) / (l2.p1.x - l2.p2.x);

		inter_x = (k1 * l1.p1.x - k2 * l2.p1.x - l1.p1.y + l2.p1.y) / (k1 - k2);
		inter_y = k2 * (inter_x - l2.p1.x) + l2.p1.y;
	}
	else {
		if (l1.p1.x == l1.p2.x) {
			inter_x = l1.p1.x;
			double k2 = (l2.p1.y - l2.p2.y) / (l2.p1.x - l2.p2.x);
			inter_y = k2 * (inter_x - l2.p1.x) + l2.p1.y;
		}
		else {
			inter_x = l2.p1.x;
			double k1 = (l1.p1.y - l1.p2.y) / (l1.p1.x - l1.p2.x);
			inter_y = k1 * (inter_x - l1.p1.x) + l1.p1.y;
		}
	}
	return  { l1 ,l2 ,{inter_x, inter_y} };
}

Polygon PolygonRandomGen(int width, int height, int size, float miniAngle)
{
	Polygon val;

	cv::Point2d next;
	cv::Point2d last;
	while (true) {
		while (val.Point.size() < size) {
			//next = cv::Point2d(rand() % width, rand() % height);
			while (true) {
				next = cv::Point2d(rand() % width, rand() % height);
				bool next_ok = true;
				if (val.Point.size() >= 2) {
					auto v1 = (next - val.Point[val.Point.size() - 1]);
					auto v2 = val.Point[val.Point.size() - 2] - val.Point[val.Point.size() - 1];
					auto dot = v1.ddot(v2);
					auto cosAngel = dot / VectorLengthSqr(v1, v2);
					if (cosAngel > 0.65 || cosAngel < -0.65) {
						continue;
					}
				}

				for (auto& p : val.Point) {
					if (((next.x - p.x) * (next.x - p.x) + (next.y - p.y) * (next.y - p.y)) < 1000.0) {
						next_ok = false;
					}
				}
				if (next_ok) {
					break;
				}
			}

			if (val.Point.size() > 0) {
				//check if intersect
				{
					std::vector<Line> all_lines;
					for (auto index : val.Segment) {
						all_lines.push_back({
							val.Point[index.p1],
							val.Point[index.p2]
							});
					}
					all_lines.push_back({
						next,
						val.Point[val.Point.size() - 1]
						});
					std::vector <IntersectionResult> intersectionResults;

					Intersection_01(all_lines, intersectionResults);
					bool intersect = (intersectionResults.size() != 0);
					if (!intersect) {
						val.Point.push_back(next);
						val.Segment.push_back(Edge_Index{ (uint)val.Point.size() - 1, (uint)val.Point.size() - 2 });
						last = next;
					}
				}
			}
			else {
				val.Point.push_back(next);
			}
		}
		val.Segment.push_back(Edge_Index{ (uint)size - 1, 0 });
		//check if intersect
		{
			std::vector<Line> all_lines;
			for (auto index : val.Segment) {
				all_lines.push_back({
					val.Point[index.p1],
					val.Point[index.p2]
					});
			}
			std::vector <IntersectionResult> intersectionResults;

			Intersection_01(all_lines, intersectionResults);
			bool intersect = (intersectionResults.size() != 0);
			if (!intersect) {
				break;
			}
			val.Point.clear();
			val.Segment.clear();
		}
	}

	return val;
}

double VectorLengthSqr(cv::Point2d a, cv::Point2d b)
{
	return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);

}


bool VectorSlop(cv::Point2d a, cv::Point2d b, double& slop) {
	if (a == b)
		return false;
	else if (a.x == b.x) {
		return false;
	}
	else if (a.y == b.y) {
		slop = 0.0;
		return true;
	}
	else {
		slop = -(a.x - b.x) / (a.y - b.y);
		return true;
	}
}

double DistanceToPoint(Line line, cv::Point2d point)
{
	if (line.p1.x == line.p2.x) {
		//have no slop
		return Abs(point.x - line.p2.x);
	}
	if (line.p1.y == line.p2.y) {
		return Abs(point.y - line.p2.y);
	}
	double slop = (line.p1.y - line.p2.y) / (line.p1.x - line.p2.x);
	double a = -slop;
	double b = 1;
	double c = slop * line.p1.x - line.p1.y;


	return Abs(a * point.x + b * point.y + c) / std::sqrt(a * a + b * b);
}

double Abs(double in)
{
	if (in < 0) {
		return -in;
	}
	return in;
}


void draw_line_origin_buttom_left(uint width, uint height, cv::InputOutputArray img, cv::Point pt1, cv::Point pt2, const cv::Scalar& color,
	int thickness, int lineType, int shift) {
	debug_cout("*********draw_line_origin_buttom_left: *form" + vector_to_string(pt1) + " *to" + vector_to_string(pt2));
	pt1.y = height - pt1.y;
	pt2.y = height - pt2.y;
	cv::line(img, pt1, pt2, color, thickness);
}

void draw_circle_origin_buttom_left(uint width, uint height, cv::InputOutputArray img, cv::Point center, int radius,
	const cv::Scalar& color, int thickness, int lineType, int shift) {
	center.y = height - center.y;

	cv::circle(img, center, radius, color, thickness);
}

void debug_cout(std::string msg, bool on)
{
	std::cout << msg << "\n";
}

std::string vector_to_string(cv::Point2d site) {
	return "(" + std::to_string(site.x) + ", " + std::to_string(site.y) + ")";
}