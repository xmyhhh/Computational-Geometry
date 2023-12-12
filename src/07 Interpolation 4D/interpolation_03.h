#include "common/typedef.h"
#include "common/helper.h"

#include "Eigen/Dense"

using namespace Interpolation3D_datastruct;


class KrigingInterpolation
{
public:
	enum Model {
		Spherical = 0, //球
		Exponential = 1, //指数
		Gaussian = 2  //高斯
	};
	//TODO simpleKriging and any other kriging method
	enum KrigingType {
		OrdinaryKriging = 0,
	};

	KrigingInterpolation(Interpolation3DIO& _io, int _attr_step, KrigingType krigingType = OrdinaryKriging, Model modelType = Spherical) {
		io = &_io;
		attr_step = _attr_step;
		ASSERT(attr_step < io->numberOfAttr);

		int n = io->numberOfPoint;

		double xMean = 0.0, yMean = 0.0, zMean = 0.0, valueMean = 0.0;
		for (int i = 0; i < n; ++i) {
			auto point = &io->points[i * (3 + io->numberOfAttr)];
			xMean += point[0];
			yMean += point[1];
			zMean += point[2];
			valueMean += point[3 + attr_step];
		}
		xMean /= n;
		yMean /= n;
		zMean /= n;
		valueMean /= n;

		double numerator = 0.0, denominator = 0.0;
		for (int i = 0; i < n; ++i) {
			auto point = &io->points[i * (3 + io->numberOfAttr)];
			//1. sill
			m_sill += std::pow((point[3 + attr_step] - valueMean), 2);

			//2. nugget
			numerator += (point[0] - xMean) * (point[1] - yMean) * (point[2] - zMean);
			denominator += std::pow((point[0] - xMean), 3);//TODO add z
		}
		m_sill /= n-1;
		m_nugget = yMean - (numerator / denominator * xMean);
		//3. range是变成，即有变化的距离, 偷个懒，直接把它定义为点的最大辐射距离
		//不同的Model m_range有变化？
		m_range = 0.5;

		calculateCovariogramMatrix();
	}

	//普通 克里金
	double ordinaryKrigingForPoint(double* point) {

		//AW = b  ->  W = A-1 * b
		Eigen::VectorXd b = calculateCovariogramVector(point);
		Eigen::VectorXd weights = m_covariogramMatrix * b;

		double estimatedZ = 0.0;
		for (int i = 0; i < io->numberOfPoint; ++i) {
			estimatedZ += weights(i) * io->points[i * (3 + io->numberOfAttr) + attr_step];
		}
		point[3 + attr_step] = estimatedZ;
		return estimatedZ;
	}
private:

	double calculateDistance(const double* p1, const double* p2) {
		return std::sqrt(
			std::pow(p1[0] - p2[0], 2)
			+
			std::pow(p1[1] - p2[1], 2)
			+
			std::pow(p1[2] - p2[2], 2)
		);
	}

	//计算已知点的半方差矩阵
	void calculateCovariogramMatrix() {
		int n = io->numberOfPoint;
		m_covariogramMatrix = Eigen::MatrixXd(n + 1, n + 1);
		for (int i = 0; i < n; ++i) {
			m_covariogramMatrix(i, i) = calculateCovariogram(0.0);
			for (int j = i + 1; j < n; ++j) {
				auto p1 = &io->points[i * (3 + io->numberOfAttr)];
				auto p2 = &io->points[j * (3 + io->numberOfAttr)];
				double d = calculateDistance(p1, p2);
				m_covariogramMatrix(i, j) = calculateCovariogram(d);
				m_covariogramMatrix(j, i) = m_covariogramMatrix(i, j);
			}
		}
		for (int i = 0; i < n; ++i) {
			m_covariogramMatrix(i, n) = 1.0;
			m_covariogramMatrix(n, i) = 1.0;
		}
		m_covariogramMatrix(n, n) = 0.0;

		m_covariogramMatrix = m_covariogramMatrix.inverse();
	}

	//计算未知点的半方差向量
	Eigen::VectorXd calculateCovariogramVector(double* point) {
		int n = io->numberOfPoint;
		Eigen::VectorXd distanceVector(n + 1);
		for (int i = 0; i < n; ++i) {
			auto p1 = &io->points[i * (3 + io->numberOfAttr)];
			double d = calculateDistance(point, p1);
			distanceVector(i) = calculateCovariogram(d);
		}
		distanceVector(n) = 1.0;

		return distanceVector;
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

private:
	//kriging类型
	KrigingType m_krigingType;
	//变异函数类型
	Model m_ModelType;
	//已知点的半方差矩阵
	Eigen::MatrixXd m_covariogramMatrix;
	//块金
	double m_nugget = 0;
	//基台
	double m_sill = 0;
	//变成
	double m_range = 0;

	Interpolation3DIO* io;

	int attr_step = 0;
};




void Interpolation4D_03(Interpolation3DIO& io) {
	//Kriging-3d

	for (int i = 0; i < io.numberOfAttr; i++) {
		KrigingInterpolation kr(io,i);
		for (int j = 0; j < io.numberOfQueryPoints; j++) {
			auto query_point = &io.queryPoints[j * (3 + io.numberOfAttr)];
			kr.ordinaryKrigingForPoint(query_point);
		}
	}
}