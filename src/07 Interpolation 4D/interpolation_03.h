#include "common/typedef.h"
#include "common/helper.h"

#include "Eigen/Dense"

using namespace Interpolation3D_datastruct;

namespace kriging3D {

	enum Model {
		Spherical = 0, //球
		Exponential = 1, //指数
		Gaussian = 2  //高斯
	};
	//TODO simpleKriging and any other kriging method
	enum KrigingType {
		OrdinaryKriging = 0,
	};


	//kriging类型
	KrigingType m_krigingType;
	//变异函数类型
	Model m_ModelType = Spherical;
	//已知点的半方差矩阵
	Eigen::MatrixXd m_covariogramMatrix;
	//块金
	double m_nugget;
	//基台
	double m_sill;
	//变成
	double m_range;

	double calculateDistance(const double* p1, const double* p2) {
		return std::sqrt(
			std::pow(p1[0] - p2[0], 2)
			+
			std::pow(p1[1] - p2[1], 2)
			+
			std::pow(p1[2] - p2[2], 2)
		);
	}

	//计算半方差
	double calculateCovariogram(double distance) {
		double covariogram = 0.0;
		switch (m_ModelType) {
		case Spherical:
			if (distance > m_range) {
				covariogram = 0.0;
			}
			else if (distance > 0.0) {
				double rate = distance / m_range;
				covariogram = m_sill * (1 - (1.5 * rate - 0.5 * std::pow(rate, 3.0)));
			}
			else {
				covariogram = m_sill;
			}
			break;
		case Exponential:
			if (distance > 0.0) {
				covariogram = (m_sill - m_nugget) * (std::exp(-distance / m_range));
			}
			else {
				covariogram = m_sill;
			}
		case Gaussian:
			if (distance > 0) {
				covariogram = (m_sill - m_nugget) * (std::exp(-std::pow(distance / m_range, 2.0)));
			}
			else {
				covariogram = m_sill;
			}
		default:
			break;
		}
		return covariogram;
	}

	//计算已知点的半方差矩阵
	void calculateCovariogramMatrix(Interpolation3DIO& io) {
		m_covariogramMatrix = Eigen::MatrixXd(io.numberOfQueryPoints + 1, io.numberOfQueryPoints + 1);
		for (int i = 0; i < io.numberOfQueryPoints; ++i) {
			m_covariogramMatrix(i, i) = calculateCovariogram(0.0);
			for (int j = i + 1; j < io.numberOfQueryPoints; ++j) {
				auto p1 = &io.points[i * (3 + io.numberOfAttr)];
				auto p2 = &io.points[j * (3 + io.numberOfAttr)];
				double d = calculateDistance(p1, p2);
				m_covariogramMatrix(i, j) = calculateCovariogram(d);
				m_covariogramMatrix(j, i) = m_covariogramMatrix(i, j);
			}
		}
		for (int i = 0; i < io.numberOfQueryPoints; ++i) {
			m_covariogramMatrix(i, io.numberOfQueryPoints) = 1.0;
			m_covariogramMatrix(io.numberOfQueryPoints, i) = 1.0;
		}
		m_covariogramMatrix(io.numberOfQueryPoints, io.numberOfQueryPoints) = 0.0;

		m_covariogramMatrix = m_covariogramMatrix.inverse();
	}

	//计算未知点的半方差向量
	Eigen::VectorXd calculateCovariogramVector(Interpolation3DIO& io, double* point) {

		Eigen::VectorXd distanceVector(io.numberOfPoint + 1);
		for (int i = 0; i < io.numberOfPoint; ++i) {
			auto p1 = &io.points[i * (3 + io.numberOfAttr)];
			double d = calculateDistance(point, p1);
			distanceVector(i) = calculateCovariogram(d);
		}
		distanceVector(io.numberOfPoint) = 1.0;
		return distanceVector;
	}

	//普通 克里金
	void ordinaryKrigingForPoint(Interpolation3DIO& io) {
		//AW = b  ->  W = A-1 * b

		for (int i = 0; i < io.numberOfQueryPoints; ++i) {
			auto query_point = &io.queryPoints[i * (3 + io.numberOfAttr)];
			double estimatedZ = 0.0;
			for (int j = 0; j < io.numberOfAttr; ++j) {
				Eigen::VectorXd b = calculateCovariogramVector(io, query_point);
				Eigen::VectorXd weights = m_covariogramMatrix * b;
				for (int k = 0; k < io.numberOfPoint; ++k) {
					auto point = &io.points[i * (3 + io.numberOfAttr)];
					estimatedZ += weights(i) * point[3 + j];
				}
			}
		}
	}
}

void Interpolation4D_03(Interpolation3DIO& io) {
	//Kriging-3d

	 //预计算

	double xMean = 0.0, yMean = 0.0, zMean;
	std::vector<double> attrMean;
	for (int i = 0; i < io.numberOfAttr; i++)
		attrMean.push_back(0);

	for (int i = 0; i < io.numberOfPoint; ++i) {
		auto point = &io.points[i * (3 + io.numberOfAttr)];
		xMean += point[0];
		yMean += point[1];
		zMean += point[2];
		for (int j = 0; j < io.numberOfAttr; j++) {
			attrMean[j] += point[3 + j];
		}
	}
	xMean /= io.numberOfPoint;
	yMean /= io.numberOfPoint;
	zMean /= io.numberOfPoint;

	for (int j = 0; j < io.numberOfAttr; j++) {
		attrMean[j] /= io.numberOfPoint;
	}

	double numerator = 0.0, denominator = 0.0;
	for (int i = 0; i < n; ++i) {
		//1. sill
		m_sill += (m_points[i].value - zMean) * (m_points[i].value - zMean);
		//2. nugget
		numerator += (m_points[i].x - xMean) * (m_points[i].y - yMean);
		denominator += (m_points[i].x - xMean) * (m_points[i].x - xMean);
	}
	m_sill /= n;
	m_nugget = yMean - (numerator / denominator * xMean);
	//3. range是变成，即有变化的距离, 偷个懒，直接把它定义为点的最大辐射距离
	//不同的Model m_range有变化？
	m_range = 0.5;

	calculateCovariogramMatrix();



}