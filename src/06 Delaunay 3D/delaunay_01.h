#pragma once
#include "common/typedef.h"
#include "common/helper.h"
#include "common/timer/timer.h"

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
			_n_simplices.radius,
			_n_simplices.radius2
		);
		};

	auto inside_simplices_sphere = [&bw_dt_struct](const cv::Point3d vertex_position, n_simplices _n_simplices) {
		double distance = VectorLengthSqr(vertex_position, _n_simplices.coordinates);

		if (abs(distance - _n_simplices.radius2) > 0.00001 && false) {
			return  distance < _n_simplices.radius2;
		}
		else {
			auto get_position = [&bw_dt_struct](int index) {return bw_dt_struct.all_point[index]; };
			REAL* pa = new REAL[3](get_position(_n_simplices.index_p1).x, get_position(_n_simplices.index_p1).y, get_position(_n_simplices.index_p1).z);

			REAL* pb = new REAL[3](get_position(_n_simplices.index_p2).x, get_position(_n_simplices.index_p2).y, get_position(_n_simplices.index_p2).z);
			REAL* pc = new REAL[3](get_position(_n_simplices.index_p3).x, get_position(_n_simplices.index_p3).y, get_position(_n_simplices.index_p3).z);
			REAL* pd = new REAL[3](get_position(_n_simplices.index_p4).x, get_position(_n_simplices.index_p4).y, get_position(_n_simplices.index_p4).z);
			REAL* pe = new REAL[3](vertex_position.x, vertex_position.y, vertex_position.z);

			auto res = insphereexact(pa, pb, pc, pd, pe) * orient3dexact(pa, pb, pc, pd) > 0;
			free(pa);
			free(pb);
			free(pc);
			free(pd);
			free(pe);
			return res;
		}


		};

	auto is_face_inside_facelist = [](n_simplices_face& face, std::vector<n_simplices_face>& n_simplices_face_to_reserve_list, bool find_and_remove) {
		int index = 0;
		for (auto it = n_simplices_face_to_reserve_list.begin(); it != n_simplices_face_to_reserve_list.end(); it++) {
			auto face_item = *it;
			bool b1 = face_item.index_p1 == face.index_p1 || face_item.index_p1 == face.index_p2 || face_item.index_p1 == face.index_p3;
			bool b2 = face_item.index_p2 == face.index_p1 || face_item.index_p2 == face.index_p2 || face_item.index_p2 == face.index_p3;
			bool b3 = face_item.index_p3 == face.index_p1 || face_item.index_p3 == face.index_p2 || face_item.index_p3 == face.index_p3;
			if (b1 && b2 && b3) {
				if (find_and_remove) {
					n_simplices_face_to_reserve_list.erase(it);
				}
				return true;
			}

		}
		index++;
		return false;
		};

	auto is_face_inside_facemap = [](n_simplices_face& face, std::unordered_map<std::string, n_simplices_face>& n_simplices_face_to_reserve_map, bool find_and_remove) {

		auto search_key = face.get_key();
		auto it = n_simplices_face_to_reserve_map.find(search_key);
		if (it == n_simplices_face_to_reserve_map.end()) {
			return false;
		}
		else
		{
			if (find_and_remove) {
				n_simplices_face_to_reserve_map.erase(it);
			}
			return true;
		}

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
		//Any data point found to be in non-general position is rejectedbecause it leads to a non-unique tessellation.
		ASSERT(bad_n_simplices_list.size() > 0); //may raise error 

		//Stage 2:
		bool use_map = false;
		std::vector<n_simplices_face> n_simplices_face_to_reserve_list;
		if (use_map) {
			std::unordered_map<std::string, n_simplices_face> n_simplices_face_to_reserve_map;

			for (auto& simplices : bad_n_simplices_list) {
				n_simplices_face face1 = n_simplices_face(simplices.index_p1, simplices.index_p2, simplices.index_p3);
				n_simplices_face face2 = n_simplices_face(simplices.index_p1, simplices.index_p2, simplices.index_p4);
				n_simplices_face face3 = n_simplices_face(simplices.index_p1, simplices.index_p3, simplices.index_p4);
				n_simplices_face face4 = n_simplices_face(simplices.index_p2, simplices.index_p3, simplices.index_p4);
				//If any (n - 1)-face is found to occur twice, both occurrences are dropped from the list since this face is shared by two adjacentn-simplices
				if (is_face_inside_facemap(face1, n_simplices_face_to_reserve_map, true)) {

				}
				else {
					n_simplices_face_to_reserve_map.insert(std::make_pair(face1.get_key(), face1));
				}

				if (is_face_inside_facemap(face2, n_simplices_face_to_reserve_map, true)) {

				}
				else {
					n_simplices_face_to_reserve_map.insert(std::make_pair(face2.get_key(), face2));
				}

				if (is_face_inside_facemap(face3, n_simplices_face_to_reserve_map, true)) {

				}
				else {
					n_simplices_face_to_reserve_map.insert(std::make_pair(face3.get_key(), face3));
				}

				if (is_face_inside_facemap(face4, n_simplices_face_to_reserve_map, true)) {

				}
				else {
					n_simplices_face_to_reserve_map.insert(std::make_pair(face4.get_key(), face4));
				}

			}


			for (auto& it : n_simplices_face_to_reserve_map) {
				n_simplices_face_to_reserve_list.push_back(it.second);

			}
		}
		else {
			//make n_simplices_face_to_reserve_list
			for (auto& simplices : bad_n_simplices_list) {
				n_simplices_face face1 = n_simplices_face(simplices.index_p1, simplices.index_p2, simplices.index_p3);
				n_simplices_face face2 = n_simplices_face(simplices.index_p1, simplices.index_p2, simplices.index_p4);
				n_simplices_face face3 = n_simplices_face(simplices.index_p1, simplices.index_p3, simplices.index_p4);
				n_simplices_face face4 = n_simplices_face(simplices.index_p2, simplices.index_p3, simplices.index_p4);
				//If any (n - 1)-face is found to occur twice, both occurrences are dropped from the list since this face is shared by two adjacentn-simplices
				if (is_face_inside_facelist(face1, n_simplices_face_to_reserve_list, true)) {
					//remove_face_from_vector(face1, n_simplices_face_to_reserve_list);
				}
				else {
					n_simplices_face_to_reserve_list.push_back(face1);
				}

				if (is_face_inside_facelist(face2, n_simplices_face_to_reserve_list, true)) {
					//remove_face_from_vector(face2, n_simplices_face_to_reserve_list);
				}
				else {
					n_simplices_face_to_reserve_list.push_back(face2);
				}

				if (is_face_inside_facelist(face3, n_simplices_face_to_reserve_list, true)) {
					//remove_face_from_vector(face3, n_simplices_face_to_reserve_list);
				}
				else {
					n_simplices_face_to_reserve_list.push_back(face3);
				}

				if (is_face_inside_facelist(face4, n_simplices_face_to_reserve_list, true)) {
					//remove_face_from_vector(face4, n_simplices_face_to_reserve_list);
				}
				else {
					n_simplices_face_to_reserve_list.push_back(face4);
				}

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
		bw_dt_struct.has_bounding_box = true;
		bw_dt_struct.n_simplices_list.push_back({ 0,1,2,3 });
		n_simplices_bounding_sphere_cal(bw_dt_struct.n_simplices_list[0]);
	}
	int i = 0;

	debug_cout("beging Incremental_construction pt");
	for (const auto& point : all_dots) {
		debug_cout("Incremental_construction pt " + std::to_string(i));
		i++;
		Incremental_construction(point, bw_dt_struct);
	}


}