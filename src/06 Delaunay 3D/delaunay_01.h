#include "common/typedef.h"
#include "common/helper.h"



namespace Delaunay3D_01_datastruct {

	struct n_simplices_face {
		int index_p1;
		int index_p2;
		int index_p3;
	};

	struct n_simplices {
		int index_p1;
		int index_p2;
		int index_p3;
		int index_p4;
		//Each (n + 1)-tuple of indices is associated with coordinates/radius of circumsphere
		cv::Point3d coordinates;
		double radius;

		bool mark_delation = false;
	};

	struct BW_DT_struct
	{
		//* A list of(n + 1)-tuples of indices to the nuclei or datapoints
		std::vector<n_simplices> n_simplices_list;
		std::vector<cv::Point3d> all_point;


		VulkanDrawData toVulkanDrawData() {
			VulkanDrawData data;

			data.numberOfPoint = all_point.size();
			data.points = (double*)malloc(data.numberOfPoint * 3 * sizeof(double));
			//memcpy(data.points, all_point.data(), data.numberOfPoint * 3 * sizeof(double));
			for (int i = 0; i < data.numberOfPoint; i++) {
				data.points[i * 3 + 0] = all_point[i].x;
				data.points[i * 3 + 1] = all_point[i].y;
				data.points[i * 3 + 2] = all_point[i].z;
			}
			data.numberOfPointAttr = 4;
			data.attr = (double*)malloc(sizeof(double) * data.numberOfPointAttr * data.numberOfPoint);
			for (int i = 0; i < data.numberOfPoint; i++) {
				if (i < 2) {
					data.attr[i * 4 + 0] = 254;
					data.attr[i * 4 + 1] = 0;
					data.attr[i * 4 + 2] = 0;
					data.attr[i * 4 + 3] = 1;
				}
				else if (i == 2) {
					data.attr[i * 4 + 0] = 0;
					data.attr[i * 4 + 1] = 0;
					data.attr[i * 4 + 2] = 254;
					data.attr[i * 4 + 3] = 1;
				}
				else if (i==3) {
					data.attr[i * 4 + 0] = 0;
					data.attr[i * 4 + 1] = 254;
					data.attr[i * 4 + 2] = 0;
					data.attr[i * 4 + 3] = 1;
				}
				else {
					data.attr[i * 4 + 0] = 255;
					data.attr[i * 4 + 1] = 255;
					data.attr[i * 4 + 2] = 255;
					data.attr[i * 4 + 3] = 0;
				}
			}
			data.numberOfTriangle = n_simplices_list.size() * 4;
			data.triangles = (int*)malloc(data.numberOfTriangle * 4 * 3 * sizeof(int));
			for (int i = 0; i < n_simplices_list.size(); i++) {
				auto& tetrahedrons = n_simplices_list[i];
				data.triangles[i * 4 * 3 + 0 * 3] = tetrahedrons.index_p1;
				data.triangles[i * 4 * 3 + 0 * 3 + 1] = tetrahedrons.index_p2;
				data.triangles[i * 4 * 3 + 0 * 3 + 2] = tetrahedrons.index_p3;

				data.triangles[i * 4 * 3 + 1 * 3] = tetrahedrons.index_p1;
				data.triangles[i * 4 * 3 + 1 * 3 + 1] = tetrahedrons.index_p2;
				data.triangles[i * 4 * 3 + 1 * 3 + 2] = tetrahedrons.index_p4;

				data.triangles[i * 4 * 3 + 2 * 3] = tetrahedrons.index_p1;
				data.triangles[i * 4 * 3 + 2 * 3 + 1] = tetrahedrons.index_p3;
				data.triangles[i * 4 * 3 + 2 * 3 + 2] = tetrahedrons.index_p4;

				data.triangles[i * 4 * 3 + 3 * 3] = tetrahedrons.index_p2;
				data.triangles[i * 4 * 3 + 3 * 3 + 1] = tetrahedrons.index_p3;
				data.triangles[i * 4 * 3 + 3 * 3 + 2] = tetrahedrons.index_p3;
			}



			return data;
		}
	};

}


void Delaunay_3D_01(std::vector<cv::Point3d>& all_dots, Delaunay3D_01_datastruct::BW_DT_struct& bw_dt_struct) {
	//Bowyer-Watson Algorithm 3d
	using namespace Delaunay3D_01_datastruct;



	auto n_simplices_bounding_sphere_cal = [&bw_dt_struct](n_simplices& _n_simplices) {
		CalculateBoundingSphere(
			bw_dt_struct.all_point[_n_simplices.index_p1],
			bw_dt_struct.all_point[_n_simplices.index_p2],
			bw_dt_struct.all_point[_n_simplices.index_p3],
			bw_dt_struct.all_point[_n_simplices.index_p4],
			_n_simplices.coordinates,
			_n_simplices.radius
		);
		};

	auto inside_simplices_sphere = [](const cv::Point3d vertex_position, n_simplices _n_simplices) {
		double distance = VectorLengthSqr(vertex_position, _n_simplices.coordinates);
		return  distance < _n_simplices.radius * _n_simplices.radius;
		};

	auto is_face_inside_facelist = [](n_simplices_face& face, std::vector<n_simplices_face>& n_simplices_face_to_reserve_list) {


		for (const auto& face_item : n_simplices_face_to_reserve_list) {
			bool b1 = face_item.index_p1 == face.index_p1 || face_item.index_p1 == face.index_p2 || face_item.index_p1 == face.index_p3;
			bool b2 = face_item.index_p2 == face.index_p1 || face_item.index_p2 == face.index_p2 || face_item.index_p2 == face.index_p3;
			bool b3 = face_item.index_p3 == face.index_p1 || face_item.index_p3 == face.index_p2 || face_item.index_p3 == face.index_p3;
			if (b1 && b2 && b3)
				return true;
		}
		return false;
		};

	auto remove_face_from_vector = [](n_simplices_face& face, std::vector<n_simplices_face>& n_simplices_face_to_reserve_list) {
		//auto it = std::find(n_simplices_face_to_reserve_list.begin(), n_simplices_face_to_reserve_list.end(), face);
		for (auto it = n_simplices_face_to_reserve_list.begin(); it != n_simplices_face_to_reserve_list.end(); it++) {
			auto face_item = *it;
			bool b1 = face_item.index_p1 == face.index_p1 || face_item.index_p1 == face.index_p2 || face_item.index_p1 == face.index_p3;
			bool b2 = face_item.index_p2 == face.index_p1 || face_item.index_p2 == face.index_p2 || face_item.index_p2 == face.index_p3;
			bool b3 = face_item.index_p3 == face.index_p1 || face_item.index_p3 == face.index_p2 || face_item.index_p3 == face.index_p3;
			if (b1 && b2 && b3) {
				n_simplices_face_to_reserve_list.erase(it);
				break;
			}
		}
		};

	auto Incremental_construction = [&](const cv::Point3d insert_vertex_position, BW_DT_struct& bw_dt_struct) {


		bw_dt_struct.all_point.push_back(insert_vertex_position);
		int insert_vertex_index = bw_dt_struct.all_point.size() - 1;

		//Stage 1: search the list of (n + 1)-tuples to find all circumspheres that contain the new point
		std::vector<n_simplices> bad_n_simplices_list;

		for (auto& simplices : bw_dt_struct.n_simplices_list) {
			if (inside_simplices_sphere(insert_vertex_position, simplices)) {
				simplices.mark_delation = true;
				bad_n_simplices_list.push_back(simplices);
			}
		}

		ASSERT(bad_n_simplices_list.size() > 0);

		//Stage 2:
		std::vector<n_simplices_face> n_simplices_face_to_reserve_list;
		//make n_simplices_face_to_reserve_list
		for (auto& simplices : bad_n_simplices_list) {
			n_simplices_face face1 = n_simplices_face(simplices.index_p1, simplices.index_p2, simplices.index_p3);
			n_simplices_face face2 = n_simplices_face(simplices.index_p1, simplices.index_p2, simplices.index_p4);
			n_simplices_face face3 = n_simplices_face(simplices.index_p1, simplices.index_p3, simplices.index_p4);
			n_simplices_face face4 = n_simplices_face(simplices.index_p2, simplices.index_p3, simplices.index_p4);
			//If any (n - 1)-face is found to occur twice, both occurrences are dropped from the list since this face is shared by two adjacentn-simplices
			if (is_face_inside_facelist(face1, n_simplices_face_to_reserve_list)) {
				remove_face_from_vector(face1, n_simplices_face_to_reserve_list);
			}
			else {
				n_simplices_face_to_reserve_list.push_back(face1);
			}

			if (is_face_inside_facelist(face2, n_simplices_face_to_reserve_list)) {
				remove_face_from_vector(face2, n_simplices_face_to_reserve_list);
			}
			else {
				n_simplices_face_to_reserve_list.push_back(face2);
			}

			if (is_face_inside_facelist(face3, n_simplices_face_to_reserve_list)) {
				remove_face_from_vector(face3, n_simplices_face_to_reserve_list);
			}
			else {
				n_simplices_face_to_reserve_list.push_back(face3);
			}

			if (is_face_inside_facelist(face4, n_simplices_face_to_reserve_list)) {
				remove_face_from_vector(face4, n_simplices_face_to_reserve_list);
			}
			else {
				n_simplices_face_to_reserve_list.push_back(face4);
			}

		}
		//new n-simplices are then formed with each of these singly occurring (n - 1)-faces and their circumcentres are calculated

		for (auto& face : n_simplices_face_to_reserve_list) {
			bw_dt_struct.n_simplices_list.push_back({ face.index_p1, face.index_p2,face.index_p3,insert_vertex_index });
			n_simplices_bounding_sphere_cal(*(bw_dt_struct.n_simplices_list.end() - 1));
		}

		auto it = bw_dt_struct.n_simplices_list.begin();
		while (it != bw_dt_struct.n_simplices_list.end()) {

			if (it->mark_delation) {
				it = bw_dt_struct.n_simplices_list.erase(it);
			}
			else {
				it++;
			}
		}
		};



	//init BW_DT_struct
	if (bw_dt_struct.all_point.size() < 4) {
		//uses a boundary tetrahedron to enclose the whole set of points

		bw_dt_struct.all_point.push_back({ 0,0,0 });
		bw_dt_struct.all_point.push_back({ 150,0,0 });
		bw_dt_struct.all_point.push_back({ 0,150,0 });
		bw_dt_struct.all_point.push_back({ 0,0,150 });

		bw_dt_struct.n_simplices_list.push_back({ 0,1,2,3 });
		n_simplices_bounding_sphere_cal(bw_dt_struct.n_simplices_list[0]);
	}

	for (const auto& point : all_dots) {
		Incremental_construction(point, bw_dt_struct);
	}







}