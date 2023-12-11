#include "common/typedef.h"
#include "common/helper.h"

#include "Eigen/Dense"

using namespace Interpolation4D_datastruct;

typedef std::vector<std::vector<double>> vector_of_vectors_double;

void Interpolation4D_02(Interpolation4DIO& io) {
	//RBF-3d

	auto inverse_multiquadric = [](double r) {
		double epsilon = 0.108992436915951;
		return 1.0 / std::sqrt(std::pow(epsilon * r, 2.0) + 1.0);
		};

	auto multiquadric = [](double r) {
		double epsilon = 0.108992436915951;
		double tmp = (epsilon * r);
		return std::sqrt(1.0 + (tmp * tmp));
		};


	//Step 1: calculate mat_A
	Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> Matrix_A(io.numberOfPoint, io.numberOfPoint);
	for (auto i = 0; i < io.numberOfPoint; i++) {
		for (auto j = i; j < io.numberOfPoint; j++) {
			double distance;
			auto p1 = &io.points[i * (3 + io.numberOfAttr)];
			auto p2 = &io.points[j * (3 + io.numberOfAttr)];
			if (i == j)
				distance = 0.0;
			else {
				distance = std::sqrt(
					std::pow(p1[0] - p2[0], 2)
					+
					std::pow(p1[1] - p2[1], 2)
					+
					std::pow(p1[2] - p2[2], 2)
				);

				distance = multiquadric(distance);
			}
			Matrix_A(i, j) = distance;
			Matrix_A(j, i) = distance;
		}
	}

	//Step 2: calculate mat_b
	typedef  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> Matrix_Type;
	std::vector<Matrix_Type> Matrix_b_array;


	for (auto i = 0; i < io.numberOfAttr; i++) {
		Matrix_b_array.push_back({ io.numberOfPoint, 1 });
		for (auto j = 0; j < io.numberOfPoint; j++) {
			auto point = &io.points[j * (3 + io.numberOfAttr)];//point_j
			auto& Matrix_b = Matrix_b_array[i];
			Matrix_b(j, 0) = point[3 + i];//point_j's attr i
		}
	}


	//Step 3: calculate mat_x
	std::vector<Eigen::VectorXd> Matrix_w_array;
	for (auto i = 0; i < io.numberOfAttr; i++) {
		Matrix_w_array.push_back(Matrix_A.householderQr().solve(Matrix_b_array[i]));
	}

	//Step 4: calculate predict
	for (int i = 0; i < io.numberOfQueryPoints; i++) {
		auto queryPoints = &io.queryPoints[i * (3 + io.numberOfAttr)];
		for (auto j = 0; j < io.numberOfAttr; j++) {
			double estimated_value = 0.0;
			for (auto k = 0; k < io.numberOfPoint; k++) {
				double distance;
				auto points = &io.points[k * (3 + io.numberOfAttr)];

				distance = std::sqrt(
					std::pow(points[0] - queryPoints[0], 2)
					+
					std::pow(points[1] - queryPoints[1], 2)
					+
					std::pow(points[2] - queryPoints[2], 2)
				);

				distance = multiquadric(distance);

				estimated_value += Matrix_w_array[j](k) * distance;

			}
			if (estimated_value < 1 || estimated_value>255) {
				int aa = 0;
			}
			else {
				int aa = 0;
			}
			io.queryPoints[i * (io.numberOfAttr + 3) + 3 + j] = estimated_value;
		}


	}

}