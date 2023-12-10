#include "common/typedef.h"
#include "common/helper.h"
#include "KDTreeVectorOfVectorsAdaptor.h"


using namespace Interpolation4D_datastruct;

typedef std::vector<std::vector<double>> vector_of_vectors_double;

void Interpolation4D_01(Interpolation4DIO& io) {
	//init kd-tree
	vector_of_vectors_double samples;
	samples.resize(io.numberOfPoint);
	for (size_t i = 0; i < io.numberOfPoint; i++)
	{
		samples[i].resize(3);
		samples[i][0] = io.points[i * (io.numberOfAttr + 3)];
		samples[i][1] = io.points[i * (io.numberOfAttr + 3) + 1];
		samples[i][2] = io.points[i * (io.numberOfAttr + 3) + 2];
	}

	typedef KDTreeVectorOfVectorsAdaptor<vector_of_vectors_double, double> kd_tree;

	kd_tree mat_index(3 /*dim*/, samples, 10 /* max leaf */);

	// Query point:
	static std::vector<double> query_pt(3);
	const size_t        num_results = 3;
	std::vector<size_t> ret_indexes(num_results);
	std::vector<double> out_dists_sqr(num_results);
	nanoflann::KNNResultSet<double> resultSet(num_results);
	resultSet.init(&ret_indexes[0], &out_dists_sqr[0]);

	for (size_t i = 0; i < io.numberOfQueryPoints; i++) {
		query_pt[0] = io.queryPoints[i * (io.numberOfAttr + 3)];
		query_pt[1] = io.queryPoints[i * (io.numberOfAttr + 3) + 1];
		query_pt[2] = io.queryPoints[i * (io.numberOfAttr + 3) + 2];

		// do a knn search
		mat_index.index->findNeighbors(resultSet, &query_pt[0]);

		double total_dis = 0.0;

		for (int j = 0; j < resultSet.size(); j++) {
			total_dis += out_dists_sqr[j];
		}

		for (int j = 0; j < io.numberOfAttr; j++) {
			double est_value = 0.0;
			for (int k = 0; k < resultSet.size(); k++) {
				est_value += (out_dists_sqr[k] / total_dis) * io.points[ret_indexes[k] * (io.numberOfAttr + 3) + 3 + j];//scale * attr
			}
			io.queryPoints[i * (io.numberOfAttr + 3) + 3 + j] = est_value;
		}

	}


	//std::cout << "knnSearch(nn=" << num_results << "): \n";
	//for (size_t i = 0; i < resultSet.size(); i++)
	//	std::cout << "ret_index[" << i << "]=" << ret_indexes[i]
	//	<< " out_dist_sqr=" << out_dists_sqr[i] << std::endl;



}