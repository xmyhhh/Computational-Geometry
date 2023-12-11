#include "common/typedef.h"
#include "common/helper.h"
#include "KDTreeVectorOfVectorsAdaptor.h"


using namespace Interpolation3D_datastruct;

typedef std::vector<std::vector<double>> vector_of_vectors_double;

void Interpolation4D_01(Interpolation3DIO& io) {
	//IDW-3d
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
	size_t        num_results = 5;
	if (num_results > io.numberOfPoint)
		num_results = io.numberOfPoint;
	std::vector<size_t> ret_indexes(num_results);
	std::vector<double> out_dists_sqr(num_results);


	io.vulkan_tri = (int*)malloc(sizeof(int) * num_results * 3 * io.numberOfQueryPoints);
	io.vulkan_tri_num = num_results * io.numberOfQueryPoints;

	for (size_t i = 0; i < io.numberOfQueryPoints; i++) {
		query_pt[0] = io.queryPoints[i * (io.numberOfAttr + 3)];
		query_pt[1] = io.queryPoints[i * (io.numberOfAttr + 3) + 1];
		query_pt[2] = io.queryPoints[i * (io.numberOfAttr + 3) + 2];

		// do a knn search
		num_results = mat_index.index->knnSearch(&query_pt[0], num_results, &ret_indexes[0], &out_dists_sqr[0]);

		double inverse_sum = 0.0;
		for (int j = 0; j < num_results; j++) {
			inverse_sum += 1.0 / (out_dists_sqr[j]);

		}

		std::vector<double> W_i;
		W_i.resize(num_results);
		for (int j = 0; j < num_results; j++) {
			W_i[j] = (1 / (out_dists_sqr[j] + 0.00001)) / inverse_sum;
		}


		for (int j = 0; j < io.numberOfAttr; j++) {
			double est_value = 0.0;
			for (int k = 0; k < num_results; k++) {
				est_value += W_i[k] * io.points[ret_indexes[k] * (io.numberOfAttr + 3) + 3 + j];//scale * attr
			}
			io.queryPoints[i * (io.numberOfAttr + 3) + 3 + j] = est_value;
		}

		for (int j = 0; j < num_results; j++) {
			io.vulkan_tri[i * (num_results * 3) + j * 3] = ret_indexes[j];
			io.vulkan_tri[i * (num_results * 3) + j * 3 + 1] = ret_indexes[j];
			io.vulkan_tri[i * (num_results * 3) + j * 3 + 2] = io.numberOfPoint + i;
		}
	}


	//std::cout << "knnSearch(nn=" << num_results << "): \n";
	//for (size_t i = 0; i < resultSet.size(); i++)
	//	std::cout << "ret_index[" << i << "]=" << ret_indexes[i]
	//	<< " out_dist_sqr=" << out_dists_sqr[i] << std::endl;



}