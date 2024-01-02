#pragma once
#include "common/typedef.h"
#include "common/helper.h"
#include "common/file/format_convert.h"
#include "common/memory.h"

extern void Delaunay_3D_02(std::vector<cv::Point3d>& all_dots, Delaunay3D_01_datastruct::BW_DT_struct& bw_dt_struct);

namespace CDT_3D_01_datastruct {
	struct Edge;

	struct Vertex {
		cv::Point3d position;
		std::vector<Edge*>* connect_edge_array;
		bool vtx_from_plc = 0;

		//for e-p
		Edge* nearest_edge = nullptr;
		double nearest_edge_distance;
		Vertex* nearest_vtx = nullptr;
		double nearest_vtx_distance;


	};

	struct Face {
		Vertex* p1;
		Vertex* p2;
		Vertex* p3;

		//for Gift_Wrapping
		Face* twin;
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
		std::vector<base_type::Triangle_Index> face_index_array;

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


			for (auto vtx : vertex_array) {
				auto p = (Vertex*)vertex_pool.allocate();
				p->position = vtx;
				p->vtx_from_plc = true;
				p->connect_edge_array = new std::vector<Edge*>();
			}

			for (auto face : face_index_array) {

				auto p1 = (Vertex*)vertex_pool[face.p1];
				auto p2 = (Vertex*)vertex_pool[face.p2];
				auto p3 = (Vertex*)vertex_pool[face.p3];

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
				if (!find_edge(p1, p2, e)) {
					e = (Edge*)edge_pool.allocate();
					e->end = p1;
					e->orig = p2;
					e->draw_red = false;
					p1->connect_edge_array->push_back(e);
					p2->connect_edge_array->push_back(e);
				}
				if (!find_edge(p2, p3, e)) {
					e = (Edge*)edge_pool.allocate();
					e->end = p2;
					e->orig = p3;
					e->draw_red = false;
					p2->connect_edge_array->push_back(e);
					p3->connect_edge_array->push_back(e);
				}
				if (!find_edge(p3, p1, e)) {
					e = (Edge*)edge_pool.allocate();
					e->end = p3;
					e->orig = p1;
					e->draw_red = false;
					p3->connect_edge_array->push_back(e);
					p1->connect_edge_array->push_back(e);
				}

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

	struct Tetrahedra {
		Vertex* p1;
		Vertex* p2;
		Vertex* p3;
		Vertex* p4;

		//for draw
		bool draw_red = false;
	};

	struct Gift_Wrapping {
		MemoryPool vertex_pool;
		MemoryPool tetrahedra_pool;

		void init_from_bw(Delaunay3D_01_datastruct::BW_DT_struct& bw_dt_struct) {
			bw_dt_struct.clear_bounding_box();

			vertex_pool.initializePool(sizeof(Vertex), bw_dt_struct.all_point.size() * 2, 8, 64);
			tetrahedra_pool.initializePool(sizeof(Tetrahedra), bw_dt_struct.n_simplices_list.size() * 2, 8, 64);

			for (int i = 0; i < bw_dt_struct.all_point.size(); i++) {
				auto p = (Vertex*)vertex_pool.allocate();
				p->position = bw_dt_struct.all_point[i];
				p->connect_edge_array = new std::vector<Edge*>();
			}
			for (int i = 0; i < bw_dt_struct.n_simplices_list.size(); i++) {
				auto p = (Tetrahedra*)tetrahedra_pool.allocate();
				p->draw_red = false;
				p->p1 = (Vertex*)vertex_pool[bw_dt_struct.n_simplices_list[i].index_p1];
				p->p2 = (Vertex*)vertex_pool[bw_dt_struct.n_simplices_list[i].index_p2];
				p->p3 = (Vertex*)vertex_pool[bw_dt_struct.n_simplices_list[i].index_p3];
				p->p4 = (Vertex*)vertex_pool[bw_dt_struct.n_simplices_list[i].index_p4];
			}
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
			vulkan_data.numberOfTriangle = tetrahedra_pool.size() * 4;
			vulkan_data.triangles = (int*)malloc(vulkan_data.numberOfTriangle * 3/*xyz*/ * sizeof(int));
			vulkan_data.triangleColoring = true;
			vulkan_data.triangleColors = (double*)malloc(vulkan_data.numberOfTriangle * 3/*rgb*/ * sizeof(double));

			for (int i = 0; i < vulkan_data.numberOfTriangle/* tetrahedra_pool.size() * 4 * 3 */; i++) {
				vulkan_data.triangleColors[i * 3] = 1;
				vulkan_data.triangleColors[i * 3 + 1] = 1;
				vulkan_data.triangleColors[i * 3 + 2] = 1;
			}

			for (int i = 0; i < tetrahedra_pool.size(); i++) {
				auto t = (Tetrahedra*)tetrahedra_pool[i];

				vulkan_data.triangles[i * 3 * 4] = vertex_pool.get_index(t->p1);
				vulkan_data.triangles[i * 3 * 4 + 1] = vertex_pool.get_index(t->p2);
				vulkan_data.triangles[i * 3 * 4 + 2] = vertex_pool.get_index(t->p3);

				vulkan_data.triangles[i * 3 * 4 + 3] = vertex_pool.get_index(t->p1);
				vulkan_data.triangles[i * 3 * 4 + 4] = vertex_pool.get_index(t->p2);
				vulkan_data.triangles[i * 3 * 4 + 5] = vertex_pool.get_index(t->p4);

				vulkan_data.triangles[i * 3 * 4 + 6] = vertex_pool.get_index(t->p1);
				vulkan_data.triangles[i * 3 * 4 + 7] = vertex_pool.get_index(t->p3);
				vulkan_data.triangles[i * 3 * 4 + 8] = vertex_pool.get_index(t->p4);

				vulkan_data.triangles[i * 3 * 4 + 9] = vertex_pool.get_index(t->p2);
				vulkan_data.triangles[i * 3 * 4 + 10] = vertex_pool.get_index(t->p3);
				vulkan_data.triangles[i * 3 * 4 + 11] = vertex_pool.get_index(t->p4);

				if (t->draw_red) {
					vulkan_data.triangleColors[i * 3 * 4] = 1;
					vulkan_data.triangleColors[i * 3 * 4 + 1] = 0;
					vulkan_data.triangleColors[i * 3 * 4 + 2] = 0;

					vulkan_data.triangleColors[i * 3 * 4 + 3] = 1;
					vulkan_data.triangleColors[i * 3 * 4 + 4] = 0;
					vulkan_data.triangleColors[i * 3 * 4 + 5] = 0;

					vulkan_data.triangleColors[i * 3 * 4 + 6] = 1;
					vulkan_data.triangleColors[i * 3 * 4 + 7] = 0;
					vulkan_data.triangleColors[i * 3 * 4 + 8] = 0;

					vulkan_data.triangleColors[i * 3 * 4 + 9] = 1;
					vulkan_data.triangleColors[i * 3 * 4 + 10] = 0;
					vulkan_data.triangleColors[i * 3 * 4 + 11] = 0;
				}

			}


			return vulkan_data;
		}
	};
}


CDT_3D_01_datastruct::Gift_Wrapping CDT_3D_01(CDT_3D_01_datastruct::PLC& plc) {
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

	//edge protection algorithm
	{
#pragma region Step 1: uses protecting spheres centered at input vertices to choose locations to insert new vertices.
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
#pragma endregion

#pragma region Step 2: The second step recovers the segments that are not ends by recursive bisection（递归平分）
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

		auto check_encroachment = [](const cv::Point3d& pa, const cv::Point3d& pb, const cv::Point3d& checkpt)
			{
				double d = (pa.x - checkpt.x) * (pb.x - checkpt.x)
					+ (pa.y - checkpt.y) * (pb.y - checkpt.y)
					+ (pa.z - checkpt.z) * (pb.z - checkpt.z);
				return d < 0.; // cos\theta < 0. ==> 90 < theta <= 180 degree.  inside
			};

		while (true) {
			debug_cout("split iter: " + std::to_string(iter_times));
			std::vector<Edge*> non_strongly_delaunay_edge_array;

			for (int i = 0; i < plc.edge_pool.size(); i++) {
				auto e = (Edge*)plc.edge_pool[i];

				for (int j = 0; j < plc.vertex_pool.size(); j++) {
					auto v = (Vertex*)plc.vertex_pool[j];
					if (e->end == v || e->orig == v)
						continue;

					if (check_encroachment(e->end->position, e->orig->position, v->position)) {
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

#pragma endregion
	}


	//Incremental Facet Insertion algorithm
	{
		//The algorithm begins with a Delaunay tetrahedralization of the vertices of a fully edge-protected PLC and incrementally recovers the missing facets one by one

		auto incremental_facet_insertion = [](Gift_Wrapping& gw, base_type::Triangle_Index f) {
			//The goal of a facet insertion algorithm is to convert T into T^f.

			//First, find all the tetrahedra in T that intersect the relative interior of f(找出所有与插入面相交的四面体).
			// It may be that f is already represented as a union of triangular faces, in which case there is nothing to do.（如果插入面刚好是某些四面体的面，那就什么也不做）
			//Otherwise, the next step is to delete from T each tetrahedron whose interior intersects f, as Figure 13 illustrates. (Tetrahedra that intersect f only on their boundaries stay put.) （如果存在这样的四面体，就删除它）

			auto is_tetrahedra_intersect_with_face = []()->bool {return true; };



			//Next, use the gift-wrapping algorithm to retriangulate the polygonal cavities created on each side of f.(接下来，使用礼品包装算法对 f 两侧创建的多边形空腔进行重切分)
			//Be forewarned that there may be more than one polygonal cavity on each side of f, because some triangular faces of the tetrahedralization might already conform to f before f is inserted.(请注意，插入面的某一边可能不止一个空腔)



			};

		Delaunay3D_01_datastruct::BW_DT_struct bw_dt_struct;
		{
			std::vector<cv::Point3d> all_dots;
			for (int i = 0; i < plc.vertex_pool.size(); i++) {
				auto v = (Vertex*)plc.vertex_pool[i];
				all_dots.push_back(v->position);
			}

			debug_cout("begin Delaunay_3D_01, total vtx:" + std::to_string(all_dots.size()));

			Delaunay_3D_02(all_dots, bw_dt_struct);
		}

		Gift_Wrapping gw;
		gw.init_from_bw(bw_dt_struct);

		//Next, insert the facets of Y into X, one by one. With each facet insertion, update T so it is still the CDT of X.
		for (int i = 0; i < plc.face_index_array.size(); i++) {
			auto f = plc.face_index_array[i];
			incremental_facet_insertion(gw, f);
		}

		return gw;
	}
}