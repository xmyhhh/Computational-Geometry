#include "helper.h"

using namespace base_type;

extern void Intersection_01(std::vector<Line>& all_lines, std::vector <IntersectionResult>& intersectionResults);


bool lu_decmp(double lu[4][4], int n, int* ps, double* d, int N)
{
	double scales[4];
	double pivot, biggest, mult, tempf;
	int pivotindex = 0;
	int i, j, k;

	*d = 1.0; // No row interchanges yet.

	for (i = N; i < n + N; i++)
	{
		// For each row.
		// Find the largest element in each row for row equilibration
		biggest = 0.0;
		for (j = N; j < n + N; j++)
			if (biggest < (tempf = fabs(lu[i][j])))
				biggest = tempf;
		if (biggest != 0.0)
			scales[i] = 1.0 / biggest;
		else
		{
			scales[i] = 0.0;
			return false; // Zero row: singular matrix.
		}
		ps[i] = i; // Initialize pivot sequence.
	}

	for (k = N; k < n + N - 1; k++)
	{
		// For each column.
		// Find the largest element in each column to pivot around.
		biggest = 0.0;
		for (i = k; i < n + N; i++)
		{
			if (biggest < (tempf = fabs(lu[ps[i]][k]) * scales[ps[i]]))
			{
				biggest = tempf;
				pivotindex = i;
			}
		}
		if (biggest == 0.0)
		{
			return false; // Zero column: singular matrix.
		}
		if (pivotindex != k)
		{
			// Update pivot sequence.
			j = ps[k];
			ps[k] = ps[pivotindex];
			ps[pivotindex] = j;
			*d = -(*d); // ...and change the parity of d.
		}

		// Pivot, eliminating an extra variable  each time
		pivot = lu[ps[k]][k];
		for (i = k + 1; i < n + N; i++)
		{
			lu[ps[i]][k] = mult = lu[ps[i]][k] / pivot;
			if (mult != 0.0)
			{
				for (j = k + 1; j < n + N; j++)
					lu[ps[i]][j] -= mult * lu[ps[k]][j];
			}
		}
	}

	// (lu[ps[n + N - 1]][n + N - 1] == 0.0) ==> A is singular.
	return lu[ps[n + N - 1]][n + N - 1] != 0.0;
}

void lu_solve(double lu[4][4], int n, int* ps, double* b, int N)
{
	int i, j;
	double X[4], dot;

	for (i = N; i < n + N; i++) X[i] = 0.0;

	// Vector reduction using U triangular matrix.
	for (i = N; i < n + N; i++)
	{
		dot = 0.0;
		for (j = N; j < i + N; j++)
			dot += lu[ps[i]][j] * X[j];
		X[i] = b[ps[i]] - dot;
	}

	// Back substitution, in L triangular matrix.
	for (i = n + N - 1; i >= N; i--)
	{
		dot = 0.0;
		for (j = i + 1; j < n + N; j++)
			dot += lu[ps[i]][j] * X[j];
		X[i] = (X[i] - dot) / lu[ps[i]][i];
	}

	for (i = N; i < n + N; i++) b[i] = X[i];
}


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

bool InCircle2D(const cv::Point2d& p1, const cv::Point2d& p2, const cv::Point2d& p3, const cv::Point2d& s)
{

	double a11, a12, a13;
	double a21, a22, a23;
	double a31, a32, a33;

	a11 = p1.x - s.x;
	a12 = p1.y - s.y;
	a13 = std::pow((p1.x - s.x), 2) + std::pow((p1.y - s.y), 2);

	a21 = p2.x - s.x;
	a22 = p2.y - s.y;
	a23 = std::pow((p2.x - s.x), 2) + std::pow((p2.y - s.y), 2);

	a31 = p3.x - s.x;
	a32 = p3.y - s.y;
	a33 = std::pow((p3.x - s.x), 2) + std::pow((p3.y - s.y), 2);

	double value = a11 * a22 * a33 + a12 * a23 * a31 + a13 * a21 * a32 - a31 * a22 * a13 - a32 * a23 * a11 - a21 * a12 * a33;

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

PolygonRandom PolygonRandomGen(int width, int height, int size, float miniAngle)
{
	PolygonRandom val;

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
double VectorLengthSqr(cv::Point3d a, cv::Point3d b)
{
	return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z);

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