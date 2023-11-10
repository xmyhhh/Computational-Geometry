#include "opencv2/opencv.hpp"
#include <vector>
#include <random>

struct Triangle {
	cv::Point p1;
	cv::Point p2;
	cv::Point p3;

};

void ShowTriangle() {

	int width = 1920 / 2;
	int height = 1080 / 2;
	cv::Mat img = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);

	std::vector<Triangle> all_triangles;

	int size = 3;
	for (int i = 0; i < size; i++)
	{
		all_triangles.push_back({
			.p1 = cv::Point(rand() % width, rand() % height),
			.p2 = cv::Point(rand() % width, rand() % height),
			.p3 = cv::Point(rand() % width, rand() % height)
			});
	}

	for (size_t i = 0; i < all_triangles.size(); i++)
	{
		Triangle tri = all_triangles[i];

		cv::Point points[3] = {cv::Point(tri.p1.x,tri.p1.y),cv::Point(tri.p2.x,tri.p2.y),cv::Point(tri.p3.x,tri.p3.y) };
		const cv::Point* ppt[1] = { points };

		int npt[] = { 3 };
		cv::Scalar color = cv::Scalar(rand() % 255, rand() % 255, rand() % 255, 100);
		fillPoly(img, ppt , npt, 1, color);
	}


	imshow("Triangle", img);
	cv::waitKey(0);
}