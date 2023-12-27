#pragma once
#include "common/typedef.h"
#include "common/helper.h"
#include "common/file/format_convert.h"
#include "common/memory.h"

extern void Delaunay_3D_01(std::vector<cv::Point3d>& all_dots, Delaunay3D_01_datastruct::BW_DT_struct& bw_dt_struct);

namespace CDT_3D_01_datastruct {
	struct Edge;


	struct Vertex {
		cv::Point3d position;
		std::vector<Edge*>* connect_edge_array;
		bool vtx_from_plc = 0;

		Edge* nearest_edge = nullptr;
		double nearest_edge_distance;
		Vertex* nearest_vtx = nullptr;
		double nearest_vtx_distance;
	};

	struct Face {
		Vertex* p1;
		Vertex* p2;
		Vertex* p3;
	};

	struct Edge {
		Vertex* orig;
		Vertex* end;

		//for draw debug
		bool draw_red = false;
	};

	struct PLC {
		MemoryPool vertex_pool;
		MemoryPool edge_pool;
		MemoryPool face_pool;

		int size = 40;

		bool init_from_file(std::string path) {
			FILE* fp = fopen(path.c_str(), "r");
			if (fp == (FILE*)NULL) {
				//printf("File I/O Error:  Cannot create file %s.\n", vtk_file_path);
				return false;
			}

			char buffer[2048];
			char* bufferp;
			int line_count = 0;

			std::vector<cv::Point3d> vertex_array;
			std::vector<base_type::Triangle_Index> face_index_array;

			int nboundaryindex = 0, iboundaryindex = 0;
			int currentboundary = -1;
			int currentboundaryid = -1;
			while ((bufferp = read_line(buffer, fp, &line_count)) != NULL) {

				auto is_line_type = [](char* _buffer, const char* type) {
					char s[20];
					sscanf(_buffer, "%s", &s);
					return strcmp(s, type) == 0;
					};

				if (is_line_type(buffer, "v")) {
					//vertex read
					double x, y, z;
					sscanf(bufferp, "%*s %lf %lf %lf", &x, &y, &z);

					vertex_array.push_back({ x * size, y * size, z * size }
					);
				}
				else if (is_line_type(buffer, "f")) {
					uint x, y, z;
					sscanf(bufferp, "%*s %d %d %d", &x, &y, &z);

					face_index_array.push_back({ x - 1 , y - 1, z - 1 });
				}
			}
			fclose(fp);

			//check
			for (int i = 0; i < vertex_array.size(); i++) {
				for (int j = 0; j < vertex_array.size(); j++) {
					if (i == j) {
						continue;
					}
					else {
						ASSERT(vertex_array[i] != vertex_array[j]);
					}
				}
			}


			vertex_pool.initializePool(sizeof(Vertex), vertex_array.size() * 2, 8, 64);
			edge_pool.initializePool(sizeof(Edge), face_index_array.size() * 3, 8, 64);
			face_pool.initializePool(sizeof(Face), face_index_array.size() * 2, 8, 64);

			for (auto vtx : vertex_array) {
				auto p = (Vertex*)vertex_pool.allocate();
				p->position = vtx;
				p->vtx_from_plc = true;
				p->connect_edge_array = new std::vector<Edge*>();
			}




			for (auto face : face_index_array) {
				auto f = (Face*)face_pool.allocate();
				f->p1 = (Vertex*)vertex_pool[face.p1];
				f->p2 = (Vertex*)vertex_pool[face.p2];
				f->p3 = (Vertex*)vertex_pool[face.p3];

				auto find_edge = [&](Vertex* p1, Vertex* p2, Edge*& res)->bool {
					for (int i = 0; i < edge_pool.size(); i++) {
						auto e = (Edge*)edge_pool[i];
						if (e->end == p1 && e->orig == p2) {
							res = e;
							return true;
						}
						if (e->orig == p1 && e->end == p2) {
							res = e;
							return true;
						}
					}
					return false;
					};

				Edge* e = nullptr;
				if (!find_edge(f->p1, f->p2, e)) {
					e = (Edge*)edge_pool.allocate();
					e->end = f->p1;
					e->orig = f->p2;
					e->draw_red = false;
					f->p1->connect_edge_array->push_back(e);
					f->p2->connect_edge_array->push_back(e);
				}
				if (!find_edge(f->p2, f->p3, e)) {
					e = (Edge*)edge_pool.allocate();
					e->end = f->p2;
					e->orig = f->p3;
					e->draw_red = false;
					f->p2->connect_edge_array->push_back(e);
					f->p3->connect_edge_array->push_back(e);
				}
				if (!find_edge(f->p3, f->p1, e)) {
					e = (Edge*)edge_pool.allocate();
					e->end = f->p3;
					e->orig = f->p1;
					e->draw_red = false;
					f->p3->connect_edge_array->push_back(e);
					f->p1->connect_edge_array->push_back(e);
				}

			}


			for (int i = 0; i < edge_pool.size(); i++) {
				auto edge = (Edge*)edge_pool[i];
				ASSERT(edge->draw_red == false);

			}


			return true;
		}

		VulkanDrawData toVulkanDrawData() {
			VulkanDrawData vulkan_data;

			vulkan_data.numberOfPoint = vertex_pool.size();
			vulkan_data.points = (double*)malloc(vulkan_data.numberOfPoint * 3 * sizeof(double));

			//point
			for (int i = 0; i < vertex_pool.size(); i++) {
				auto vtx = (Vertex*)vertex_pool[i];
				vulkan_data.points[i * 3] = vtx->position.x;
				vulkan_data.points[i * 3 + 1] = -vtx->position.y;
				vulkan_data.points[i * 3 + 2] = vtx->position.z;
			}

			//edge
			vulkan_data.numberOfTriangle = edge_pool.size();
			vulkan_data.triangles = (int*)malloc(vulkan_data.numberOfTriangle * 3 * sizeof(int));
			vulkan_data.triangleColoring = true;
			vulkan_data.triangleColors = (double*)malloc(vulkan_data.numberOfTriangle * 3 * sizeof(double));
			for (int i = 0; i < vulkan_data.numberOfTriangle * 3; i++) {
				vulkan_data.triangleColors[i] = 1;//set white
			}
			//for (int i = 0; i < face_pool.size(); i++) {

			//	auto face = (Face*)face_pool[i];

			//	vulkan_data.triangles[i * 3] = vertex_pool.get_index(face->p1);
			//	vulkan_data.triangles[i * 3 + 1] = vertex_pool.get_index(face->p2);
			//	vulkan_data.triangles[i * 3 + 2] = vertex_pool.get_index(face->p3);

			//}
			for (int i = 0; i < edge_pool.size(); i++) {
				auto edge = (Edge*)edge_pool[i];

				vulkan_data.triangles[i * 3] = vertex_pool.get_index(edge->end);
				vulkan_data.triangles[i * 3 + 1] = vertex_pool.get_index(edge->end);
				vulkan_data.triangles[i * 3 + 2] = vertex_pool.get_index(edge->orig);

				if (edge->draw_red) {
					vulkan_data.triangleColors[i * 3] = 1;
					vulkan_data.triangleColors[i * 3 + 1] = 0;
					vulkan_data.triangleColors[i * 3 + 2] = 0;
				}

			}



			return vulkan_data;
		}
	};

}


void CDT_3D_01(CDT_3D_01_datastruct::PLC& plc, Delaunay3D_01_datastruct::BW_DT_struct& bw_dt_struct) {
	//Constrained Delaunay Tetrahedralizations and Provably Good Boundary Recovery （Shewchuk）
	using namespace CDT_3D_01_datastruct;

	auto vertex_pool_allocate_default = [&plc](cv::Point3d position) {
		auto new_v = (Vertex*)plc.vertex_pool.allocate();
		new_v->connect_edge_array = new std::vector<Edge*>();
		new_v->vtx_from_plc = false;
		new_v->position = position;
		return  new_v;
		};
	auto edge_pool_allocate_default = [&plc](Vertex* a, Vertex* b) {
		auto new_e = (Edge*)plc.edge_pool.allocate();
		new_e->draw_red = false;
		new_e->orig = a;
		new_e->end = b;
		return new_e;

		};
	auto is_edge_in_array = [](Edge* e, std::vector<Edge*>array)->bool {
		return std::find(array.begin(), array.end(), e) != array.end();
		};
	auto remove_edge_in_array = [](Edge* e, std::vector<Edge*>array) {
		auto it = std::find(array.begin(), array.end(), e);
		if (it != array.end()) {
			array.erase(it);
			return true;
		}
		return false;

		};
	auto mark_edge = [](Edge* e) {
		e->draw_red = true;
		};
	auto unmark_edge = [](Edge* e) {
		e->draw_red = false;
		};

	//Delaunay_3D_01(plc.vertex_array, bw_dt_struct);

	//edge protection algorithm
	{
		//Step 1: uses protecting spheres centered at input vertices to choose locations to insert new vertices.
		{
			//find vtx that at least two segments meet at an angle less than 90◦
			auto is_segments_angle_less_90 = [](Vertex* vtx)->bool {

				for (auto e1 : *vtx->connect_edge_array) {
					for (auto e2 : *vtx->connect_edge_array) {
						if (e1 == e2)
							continue;

						auto e1_end = e1->orig == vtx ? e1->end : e1->orig;
						auto e2_end = e2->orig == vtx ? e2->end : e2->orig;
						auto d = dot(e1_end->position - vtx->position, e2_end->position - vtx->position);
						if (d > 0) {
							return true;
						}
					}
				}
				return false;
				};

			auto get_distance_between_vtx_and_edge = [](Vertex* vtx, Edge* e) {
				double t = -dot(e->orig->position - vtx->position, e->end->position - vtx->position) / VectorLengthSqr(e->orig->position - e->end->position);
				double distance = 0;
				if (t < 0 || t>1) {
					double dis_orig = VectorLengthSqr(vtx->position - e->orig->position);
					double dis_end = VectorLengthSqr(vtx->position - e->end->position);
					distance = std::min(dis_orig, dis_end);
				}
				else {
					distance = DistanceToPointSqr({ e->orig->position,e->end->position }, vtx->position);
				}
				ASSERT(distance >= 0);
				return distance;
				};

			auto lfs = [](Vertex* vtx) {
				// The local feature size lfs(vi) is simply the distance from vi to the nearest vertex or segment that doesn’t intersect vi
				return std::min(vtx->nearest_edge_distance, vtx->nearest_vtx_distance);
				};
			auto li = [](Vertex* vtx) {
				// li is the length of the shortest segment that is clipped by Si.
				double li = -1;
				for (auto e : *vtx->connect_edge_array) {
					double e_len = std::sqrt(VectorLengthSqr(e->orig->position - e->end->position));
					if (li == -1) {
						li = e_len;
					}
					else {
						li = std::min(li, e_len);
					}
				}
				return li;
				};
			auto di = [is_segments_angle_less_90](Vertex* vtx) {
				// di is the length of the shortest segment adjoining vi whose other end is clipped.
				double di = -1;
				for (auto e : *vtx->connect_edge_array) {
					Vertex* other = e->orig == vtx ? e->end : e->orig;
					if (!is_segments_angle_less_90(other))
						continue;
					double e_len = std::sqrt(VectorLengthSqr(e->orig->position - e->end->position));
					if (di == -1) {
						di = e_len;
					}
					else {
						di = std::min(di, e_len);
					}
				}
				//di may be -1
				return di;
				};

			auto get_vtx_clip_r = [lfs, li, di](Vertex* vtx) {

				ASSERT(vtx->nearest_vtx);
				ASSERT(vtx->nearest_edge);
				double v_lfs = lfs(vtx);
				double v_li = 0.333 * li(vtx);
				double v_di = 0.666 * di(vtx);
				if (v_di < 0) v_di = v_li;
				double r = std::min(v_lfs, std::min(v_li, v_di));
				ASSERT(r > 0);
				return r;
				};
			auto inserte_around_vtx = [get_vtx_clip_r, &plc, mark_edge, vertex_pool_allocate_default, edge_pool_allocate_default](Vertex* vtx) {
				double r = get_vtx_clip_r(vtx);

				std::vector<Edge*> insert_edge_array(*vtx->connect_edge_array);
				delete(vtx->connect_edge_array);
				vtx->connect_edge_array = new std::vector<Edge*>();
				for (auto e : insert_edge_array) {
					Vertex* other = vtx == e->orig ? e->end : e->orig;
					cv::Point3d new_vtx_position = VectorNormal(other->position - vtx->position) * r + vtx->position;
					auto new_v = vertex_pool_allocate_default(new_vtx_position);

					//make edge
					auto new_e = edge_pool_allocate_default(vtx, new_v);

					e->orig = other;
					e->end = new_v;

					(*vtx->connect_edge_array).push_back(new_e);
					(*new_v->connect_edge_array).push_back(new_e);
					(*new_v->connect_edge_array).push_back(e);

					//mark_edge(new_e);
				}

				};

			//update min distance for each vtx
			for (int i = 0; i < plc.vertex_pool.size(); i++) {
				auto vtx = (Vertex*)plc.vertex_pool[i];
				double min_distance = -1;
				Edge* min_distance_edge = nullptr;
				int j = 0;
				for (; j < plc.edge_pool.size(); j++) {
					auto e = (Edge*)plc.edge_pool[j];
					if (is_edge_in_array(e, *vtx->connect_edge_array)) {
						continue;
					}

					if (min_distance == -1)
					{
						min_distance = get_distance_between_vtx_and_edge(vtx, e);
						min_distance_edge = e;
					}

					else
					{
						auto m = get_distance_between_vtx_and_edge(vtx, e);
						if (m < min_distance) {
							min_distance = m;
							min_distance_edge = e;
						}

					}
				}
				vtx->nearest_edge = min_distance_edge;
				vtx->nearest_edge_distance = std::sqrt(min_distance);

				min_distance = -1;
				Vertex* min_distance_vtx = nullptr;
				j = 0;
				for (; j < plc.vertex_pool.size(); j++) {
					if (i == j)
						continue;
					auto vtx_j = (Vertex*)plc.vertex_pool[j];
					ASSERT(vtx_j->position != vtx->position);
					if (min_distance == -1)
					{
						min_distance = VectorLengthSqr(vtx_j->position, vtx->position);
						min_distance_vtx = vtx_j;
					}
					else {
						auto m = VectorLengthSqr(vtx_j->position, vtx->position);
						if (m < min_distance) {
							min_distance_vtx = vtx_j;
							min_distance = m;
						}
					}

				}
				vtx->nearest_vtx = min_distance_vtx;
				vtx->nearest_vtx_distance = std::sqrt(min_distance);

				ASSERT(vtx->nearest_vtx != vtx);
				ASSERT(vtx->nearest_edge->orig != vtx);
				ASSERT(vtx->nearest_edge->end != vtx);
				ASSERT(vtx->nearest_vtx_distance > 0);
				ASSERT(vtx->nearest_edge_distance > 0);
			}

			for (int i = 0; i < plc.vertex_pool.size(); i++) {
				auto vtx = (Vertex*)plc.vertex_pool[i];
				if (is_segments_angle_less_90(vtx)) {
					inserte_around_vtx(vtx);
				}
			}
		}



		//Step 2: The second step recovers the segments that are not ends by recursive bisection（递归平分）
		{
			//Any segment that is not strongly Delaunay is split in two with a new vertex at its midpoint
			//<b>The simplex s is strongly Delaunay</b>: if there is a circumsphere S of s such that no vertex of X lies inside or on S, except the vertices of s.

			auto edge_split = [&plc, mark_edge, remove_edge_in_array, vertex_pool_allocate_default, edge_pool_allocate_default](Edge* edge_to_split) {
				Vertex* start = edge_to_split->orig;
				Vertex* end = edge_to_split->end;

				auto new_vtx = vertex_pool_allocate_default((edge_to_split->orig->position + edge_to_split->end->position) / 2);
				auto new_edge = edge_pool_allocate_default(new_vtx, end);

				new_vtx->connect_edge_array->push_back(new_edge);
				new_vtx->connect_edge_array->push_back(edge_to_split);

				ASSERT(remove_edge_in_array(edge_to_split, *end->connect_edge_array));
				(*end->connect_edge_array).push_back(new_edge);

				edge_to_split->end = new_vtx;

				mark_edge(edge_to_split);
				mark_edge(new_edge);
				};

			int iter_times = 0;
			debug_cout("begin edge protection algorithm step 2");

			while (true) {
				debug_cout("split iter: " + std::to_string(iter_times));
				std::vector<Edge*> non_strongly_delaunay_edge_array;

				for (int i = 0; i < plc.edge_pool.size(); i++) {
					auto e = (Edge*)plc.edge_pool[i];
					cv::Point3d circumsphere_center = (e->end->position + e->orig->position) / 2.0;
					double circumsphere_radius_sqr = VectorLengthSqr(e->end->position - circumsphere_center);
					for (int j = 0; j < plc.vertex_pool.size(); j++) {
						auto v = (Vertex*)plc.vertex_pool[j];
						if (e->end == v || e->orig == v)
							continue;

						if (VectorLengthSqr(v->position - circumsphere_center) <= circumsphere_radius_sqr) {
							non_strongly_delaunay_edge_array.push_back(e);
							break;
						}
					}
				}

				if (non_strongly_delaunay_edge_array.size() == 0)
					//When every subsegment is strongly Delaunay, the PLC is edge-protected and the algorithm terminates.
					break;

				debug_cout("find non_strongly_delaunay_edge: " + std::to_string(non_strongly_delaunay_edge_array.size()));
				for (int i = 0; i < non_strongly_delaunay_edge_array.size(); i++) {
					auto bad_e = non_strongly_delaunay_edge_array[i];
					edge_split(bad_e);
					//mark_edge(bad_e);
				}
				
				iter_times++;
			}
		}
	}

}