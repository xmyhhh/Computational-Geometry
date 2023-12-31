#pragma once

#include "typedef.h"
#include "predicates/predicates.h"

//liner linear algebra
//PLU 分解;
bool lu_decmp(double lu[4][4], int n, int *ps, double *d, int N);

void lu_solve(double lu[4][4], int n, int *ps, double *b, int N);

//geo tesl
bool ToLeft(const cv::Point &p1, const cv::Point &p2, const cv::Point &p3);

bool ToLeft(const cv::Point2d &p1, const cv::Point2d &p2, const cv::Point2d &s);

bool ToLeft3D(const cv::Point3d &p1, const cv::Point3d &p2, const cv::Point3d &p3, const cv::Point3d &s);


bool InCircle2D(const cv::Point2d &p1, const cv::Point2d &p2, const cv::Point2d &p3, const cv::Point2d &s);

bool InCircle3D(const cv::Point3d &p1, const cv::Point3d &p2, const cv::Point3d &p3, const cv::Point3d &p4, const cv::Point3d &s);

template<typename PointT>
bool InTriangleTest(PointT &p1, PointT &p2, PointT &p3, const PointT &s) {
    bool b1 = ToLeft(p1, p2, s);
    bool b2 = ToLeft(p2, p3, s);
    bool b3 = ToLeft(p3, p1, s);
    return (b1 == b2) && (b2 == b3);
}

template<typename PointT>
double Area2(PointT &p1, PointT &p2, PointT &s) {
    double value = p1.x * p2.y - p1.y * p2.x
                   + p2.x * s.y - p2.y * s.x
                   + s.x * p1.y - s.y * p1.x;
    return value;
}


//geo cul
base_type::IntersectionResult LineIntersectionCalulate(base_type::Line l1, base_type::Line l2);

base_type::IntersectionResult3d RayIntersectionCalulate_Plane(base_type::Plane plane, base_type::Ray3d line);

bool isPointOnTriangle(base_type::Triangle3d tri, cv::Point3d p);

base_type::IntersectionResult3d RayIntersectionCalulate_Triangle(base_type::Triangle3d tri, base_type::Ray3d line);

base_type::PolygonRandom PolygonRandomGen(int width, int height, int size = 3, float miniAngle = 30);

inline cv::Point2d Rotate(cv::Point2d point, double angle) {
    if (angle == 0) {
        return point;
    }
    double angle_deg = angle / 180;

    return cv::Point2d((point.x * std::cos(angle_deg) - std::sin(angle_deg) * point.y), (point.x * std::sin(angle_deg) - std::cos(angle_deg) * point.y));
}

inline cv::Point3d Rotate3d(cv::Point3d point, double yaw, double pitch, double roll) {
    //https://en.wikipedia.org/wiki/Rotation_matrix
    //yaw * pitch * roll * point
    //A yaw is a counterclockwise rotation of $ \alpha$ about the $ z$-axis.
    //A pitch is a counterclockwise rotation of $ \beta$ about the $ y$-axis.
    //A roll is a counterclockwise rotation of $ \gamma$ about the $ x$-axis.

    double a = yaw / 180;
    double b = pitch / 180;
    double r = roll / 180;
    using namespace std;

    double cos_a = cos(a), cos_b = cos(b), cos_r = cos(r);
    double sin_a = sin(a), sin_b = sin(b), sin_r = sin(r);

    double x = point.x * (cos_a * cos_b) + point.y * (cos_a * sin_b * sin_r - sin_a * cos_r) + point.z * (cos_a * sin_b * cos_r + sin_a * sin_r);
    double y = point.x * (sin_a * cos_b) + point.y * (sin_a * sin_b * sin_r + cos_a * cos_r) + point.z * (sin_a * sin_b * cos_r - cos_a * sin_r);
    double z = point.x * (-sin_b) + point.y * (cos_b * sin_r) + point.z * (cos_b * cos_r);

    return cv::Point3d(x, y, z);
}

inline cv::Point3d cross(const cv::Point3d &p1, const cv::Point3d &p2) {
    //(u1, u2, u3) x(v1, v2, v3) = (u2v3 - u3v2; u3v1 - u1v3, u1v2 - u2v1)
    double s1, s2, s3;
    s1 = p1.y * p2.z - p1.z * p2.y;
    s2 = p1.z * p2.x - p1.x * p2.z;
    s3 = p1.x * p2.y - p1.y * p2.x;
    return cv::Point3d(s1, s2, s3);
}

inline double dot(const cv::Point3d &p1, const cv::Point3d &p2) {
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

inline double dot(double *p1, double *p2) {
    return p1[0] * p2[0] + p1[1] * p2[1] + p1[2] * p2[2];
}

template<typename PointT>
bool CalculateBoundingSphere(const PointT &pa, const PointT &pb, const PointT &pc, const PointT &pd, PointT &center, double &radius, double &radius2) {

    auto lu_solve = [](double lu[4][4], int n, int *ps, double *b, int N) {
        int i, j;
        double X[4], dot;

        for (i = N; i < n + N; i++) X[i] = 0.0;

        // Vector reduction using U triangular matrix.
        for (i = N; i < n + N; i++) {
            dot = 0.0;
            for (j = N; j < i + N; j++)
                dot += lu[ps[i]][j] * X[j];
            X[i] = b[ps[i]] - dot;
        }

        // Back substitution, in L triangular matrix.
        for (i = n + N - 1; i >= N; i--) {
            dot = 0.0;
            for (j = i + 1; j < n + N; j++)
                dot += lu[ps[i]][j] * X[j];
            X[i] = (X[i] - dot) / lu[ps[i]][i];
        }

        for (i = N; i < n + N; i++) b[i] = X[i];
    };

    double A[4][4], b[4], D;
    int indx[4];

    // Compute the coefficient matrix A (3x3).
    A[0][0] = pb.x - pa.x;
    A[0][1] = pb.y - pa.y;
    A[0][2] = pb.z - pa.z;

    A[1][0] = pc.x - pa.x;
    A[1][1] = pc.y - pa.y;
    A[1][2] = pc.z - pa.z;

    A[2][0] = pd.x - pa.x;
    A[2][1] = pd.y - pa.y;
    A[2][2] = pd.z - pa.z;

    // Compute the matrix b (3).
    b[0] = 0.5 * dot(A[0], A[0]);
    b[1] = 0.5 * dot(A[1], A[1]);
    b[2] = 0.5 * dot(A[2], A[2]);

    if (!lu_decmp(A, 3, indx, &D, 0)) {
        radius = 0.0;
        return false;
    }
    lu_solve(A, 3, indx, b, 0);


    center.x = pa.x + b[0];
    center.y = pa.y + b[1];
    center.z = pa.z + b[2];

    radius2 = b[0] * b[0] + b[1] * b[1] + b[2] * b[2];
    radius = std::sqrt(radius2);

    return true;
}

template<typename PointT>
void CalculateBoundingCircle(const PointT &p1, const PointT &p2, const PointT &p3, PointT &center, double &radius) {
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

inline double VectorLengthSqr(cv::Point3d a) {
    return (a.x) * (a.x) + (a.y) * (a.y) + (a.z) * (a.z);
}

inline double VectorLength(cv::Point3d a) {
    return std::sqrt((a.x) * (a.x) + (a.y) * (a.y) + (a.z) * (a.z));
}

inline double VectorAngle(cv::Point3d a, cv::Point3d b) {
    auto d = dot(a, b);
    auto cos = d / (std::sqrt(VectorLengthSqr(a)) * std::sqrt(VectorLengthSqr(b)));
    return std::acos(cos);
}

inline double VectorLengthSqr(cv::Point2d a, cv::Point2d b) {
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

inline double VectorLengthSqr(cv::Point3d a, cv::Point3d b) {
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z);
}

inline cv::Point3d VectorNormal(cv::Point3d a) {
    return a / std::sqrt(VectorLengthSqr(a));
}

inline double Abs(double in) {
    if (in < 0) {
        return -in;
    }
    return in;
}


bool VectorSlop(cv::Point2d a, cv::Point2d b, double &slop);

inline double DistanceToPoint(base_type::Line line, cv::Point2d point) {
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

inline double DistanceToPointSqr(base_type::Line3d line, cv::Point3d point) {
    //https://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
    return VectorLengthSqr(cross(line.p2 - line.p1, line.p1 - point)) / VectorLengthSqr(line.p2 - line.p1);
}

//draw
void draw_line_origin_buttom_left(uint width, uint height, cv::InputOutputArray img, cv::Point pt1, cv::Point pt2, const cv::Scalar &color,
                                  int thickness = 1, int lineType = cv::LINE_8, int shift = 0);

void draw_circle_origin_buttom_left(uint width, uint height, cv::InputOutputArray img, cv::Point center, int radius,
                                    const cv::Scalar &color, int thickness = 1, int lineType = cv::LINE_8, int shift = 0);

//debug
void debug_cout(std::string msg, bool on = true);

std::string vector_to_string(cv::Point2d site);

