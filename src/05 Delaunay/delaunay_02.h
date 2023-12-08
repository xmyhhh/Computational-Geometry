#pragma once

#include "common/typedef.h"
#include "common/helper.h"


//generalization face, in 2-d it is a segment
struct n_simplices_face {
	int index_p1;
	int index_p2;
};

struct n_simplices {
	int index_p1;
	int index_p2;
	int index_p3;
	//Each (n + 1)-tuple of indices is associated with coordinates/radius of circumsphere
	cv::Point2d coordinates;
	double radius;

	bool mark_delation = false;
};

struct BW_DT_struct
{
	//* A list of(n + 1)-tuples of indices to the nuclei or datapoints
	std::vector<n_simplices> n_simplices_list;

	std::vector<cv::Point2d> all_point;
};

void Delaunay_02(std::vector<cv::Point>& all_point, BW_DT_struct& bw_dt_struct) {
	//Delaunay Triangulation
	//Bowyer-Watson Algorithm

	auto Incremental_construction = [&](const cv::Point2d insert_vertex_position, BW_DT_struct& bw_dt_struct) {

		auto n_simplices_bounding_circle_cal = [&bw_dt_struct](n_simplices& _n_simplices) {
			CalculateBoundingCircle(
				bw_dt_struct.all_point[_n_simplices.index_p1],
				bw_dt_struct.all_point[_n_simplices.index_p2],
				bw_dt_struct.all_point[_n_simplices.index_p3],
				_n_simplices.coordinates,
				_n_simplices.radius
			);
			};

		auto inside_simplices = [](const cv::Point2d vertex_position, n_simplices _n_simplices) {
			return VectorLengthSqr(vertex_position, _n_simplices.coordinates) < _n_simplices.radius * _n_simplices.radius;
			};

		auto is_face_inside_facelist = [](n_simplices_face& face, std::vector<n_simplices_face>& n_simplices_face_to_reserve_list) {
			for (const auto& face_item : n_simplices_face_to_reserve_list) {
				if (face_item.index_p1 == face.index_p1 && face_item.index_p2 == face.index_p2) {
					return true;
				}

				if (face_item.index_p1 == face.index_p2 && face_item.index_p2 == face.index_p1) {
					return true;
				}
			}
			return false;
			};

		auto remove_face_from_vector = [](n_simplices_face& face, std::vector<n_simplices_face>& n_simplices_face_to_reserve_list) {
			//auto it = std::find(n_simplices_face_to_reserve_list.begin(), n_simplices_face_to_reserve_list.end(), face);
			for (auto it = n_simplices_face_to_reserve_list.begin(); it != n_simplices_face_to_reserve_list.end(); it++) {
				auto face_item = *it;
				if (face_item.index_p1 == face.index_p1 && face_item.index_p2 == face.index_p2) {
					n_simplices_face_to_reserve_list.erase(it);
					break;
				}

				if (face_item.index_p1 == face.index_p2 && face_item.index_p2 == face.index_p1) {
					n_simplices_face_to_reserve_list.erase(it);
					break;
				}


			}
			};

		//init BW_DT_struct
		if (bw_dt_struct.all_point.size() < 3) {
			//uses a boundary tetrahedron to enclose the whole set of points
			//here we assume all point  in  p1(0,0) p1(2000,0) p1(0,2000) the range is min(0,0) max(1000,1000)
			bw_dt_struct.all_point.push_back({ 0,0 });
			bw_dt_struct.all_point.push_back({ 0,2000 });
			bw_dt_struct.all_point.push_back({ 2000,0 });

			bw_dt_struct.n_simplices_list.push_back({ 0,1,2 });
			n_simplices_bounding_circle_cal(bw_dt_struct.n_simplices_list[0]);
		}


		bw_dt_struct.all_point.push_back(insert_vertex_position);
		int insert_vertex_index = bw_dt_struct.all_point.size() - 1;

		//Stage 1: search the list of (n + 1)-tuples to find all circumspheres that contain the new point
		std::vector<n_simplices> bad_n_simplices_list;

		for (auto& simplices : bw_dt_struct.n_simplices_list) {
			if (inside_simplices(insert_vertex_position, simplices)) {
				simplices.mark_delation = true;
				bad_n_simplices_list.push_back(simplices);
			}
		}

		//Stage 2:
		std::vector<n_simplices_face> n_simplices_face_to_reserve_list;
		//make n_simplices_face_to_reserve_list
		for (auto& simplices : bad_n_simplices_list) {
			n_simplices_face face1 = n_simplices_face(simplices.index_p1, simplices.index_p2);
			n_simplices_face face2 = n_simplices_face(simplices.index_p2, simplices.index_p3);
			n_simplices_face face3 = n_simplices_face(simplices.index_p1, simplices.index_p3);
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
		}
		//new n-simplices are then formed with each of these singly occurring (n - 1)-faces and their circumcentres are calculated

		for (auto& face : n_simplices_face_to_reserve_list) {
			bw_dt_struct.n_simplices_list.push_back({ face.index_p1, face.index_p2,insert_vertex_index });
			n_simplices_bounding_circle_cal(*(bw_dt_struct.n_simplices_list.end() - 1));
		}


		/*	for (std::vector<n_simplices>::reverse_iterator i = bw_dt_struct.n_simplices_list.rbegin();i != my_vector.rend(); ++i) {
			}*/

		auto it = bw_dt_struct.n_simplices_list.begin();
		while (it != bw_dt_struct.n_simplices_list.end()) {

			if (it->mark_delation) {
				it = bw_dt_struct.n_simplices_list.erase(it);
			}
			else {
				it++;
			}
		}





		int a = 1;
		};

	for (const auto& point : all_point) {
		debug_cout("\n");
		debug_cout("*********start with site:" + vector_to_string(point));
		Incremental_construction(point, bw_dt_struct);
		debug_cout("*********end with site:" + vector_to_string(point));
		debug_cout("\n");
	}



}