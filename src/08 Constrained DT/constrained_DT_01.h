#pragma once

#include "common/typedef.h"
#include "common/helper.h"
#include "common/memory.h"
#include "common/file/format_convert.h"
#include "common/priority_queue.h"

namespace CDT_01_datastruct {

	bool cmp(cv::Point2d x, cv::Point2d y) {

		return x.x < y.x;
	}

	//input
	struct PSLG {

		struct Boundary
		{
			std::vector <int> point_array;
		};

		int factor = 5;
		cv::Point2d offset;
		cv::Point2d center;

		double rotate_angle = 0;

		std::vector <cv::Point2d> vertex_array;
		std::vector <Boundary> boundary_array;

		bool init_from_file(std::string path) {
			FILE* fp = fopen(path.c_str(), "r");
			if (fp == (FILE*)NULL)
			{
				//printf("File I/O Error:  Cannot create file %s.\n", vtk_file_path);
				return false;
			}

			char buffer[2048];
			char* bufferp;
			int line_count = 0;


			int nverts = 0, iverts = 0;
			int nboundaryindex = 0, iboundaryindex = 0;
			int currentboundary = -1;
			int currentboundaryid = -1;
			while ((bufferp = read_line(buffer, fp, &line_count)) != NULL)
			{
				if (nverts == 0) {
					sscanf(bufferp, "%d %*s %*s", &nverts);

				}
				else if (nverts > iverts) {
					(double)strtod(bufferp, &bufferp);
					//sscanf(bufferp, "%*s %e %e %*s", &x, &y);
					vertex_array.push_back({ (double)strtod(bufferp, &bufferp),(double)strtod(bufferp, &bufferp) });
					iverts++;
				}
				else if (nboundaryindex == 0) {
					sscanf(bufferp, "%d", &nboundaryindex);
				}
				else if (nboundaryindex > iboundaryindex) {
					static int orig, end, id;
					sscanf(bufferp, "%*s %d %d %d", &orig, &end, &id);

					if (iboundaryindex == 143) {
						int aa = 0;
					}
					if (id != currentboundaryid) {
						currentboundaryid = id;
						currentboundary += 1;
					}
					if (currentboundary >= boundary_array.size()) {
						boundary_array.push_back({});

					}

					boundary_array[currentboundary].point_array.push_back(end);
					iboundaryindex++;
				}
				else
				{
					break;
				}
			}
			fclose(fp);

			init_center_and_roation();

			return true;
		}


		cv::Point2d get_position(int b_id, int v_id) {
			assert(b_id < boundary_array.size());
			assert(v_id < boundary_array[b_id].point_array.size());
			return (Rotate(vertex_array[boundary_array[b_id].point_array[v_id] - 1] - center, rotate_angle) + center) * factor + offset;
		}

		cv::Point2d get_position(int v_id) {
			assert(v_id < vertex_array.size());
			return (Rotate((vertex_array[v_id] - center), rotate_angle) + center) * factor + offset;
		}


	private:
		void init_center_and_roation() {
			center = cv::Point2d(0, 0);
			for (auto& vtx : vertex_array) {
				center += vtx;
			}
			center.x = center.x / vertex_array.size();
			center.y = center.y / vertex_array.size();

			do {
				std::vector<cv::Point2d> tmp_vtx_array;
				for (int i = 0; i < vertex_array.size(); i++) {
					tmp_vtx_array.push_back(get_position(i));
				}

				std::sort(tmp_vtx_array.begin(), tmp_vtx_array.end(), cmp);
				bool has_same_x = false;
				for (int i = 1; i < tmp_vtx_array.size(); i++) {
					if (tmp_vtx_array[i].x == tmp_vtx_array[i - 1].x) {
						has_same_x = true;
						break;
					}
				}
				if (!has_same_x) {
					break;
				}
				rotate_angle += 1;
			} while (true);


		}
		void add_rect_boundary() {
			vertex_array.push_back({ 10, 10 });
			vertex_array.push_back({ 10, -10 });
			vertex_array.push_back({ -10, 10 });
			vertex_array.push_back({ -10, -10 });
			boundary_array.push_back({});
			boundary_array[boundary_array.size() - 1].point_array.push_back(vertex_array.size() - 1);
			boundary_array[boundary_array.size() - 1].point_array.push_back(vertex_array.size() - 2);
			boundary_array[boundary_array.size() - 1].point_array.push_back(vertex_array.size() - 3);
			boundary_array[boundary_array.size() - 1].point_array.push_back(vertex_array.size() - 4);
		}
	};

	//run time
	struct Vertex;

	struct PreDefineEdge {
		Vertex* orig;
		Vertex* end;
	};

	struct Vertex
	{
		cv::Point2d position;
		bool is_infinity = false;
		PreDefineEdge* edge_below = nullptr;
		PreDefineEdge* edge_above = nullptr;
		std::vector<PreDefineEdge*>* p_connect_edge_array;
	};

	struct Strip_Region {
		bool empty = true;
		std::vector<Vertex*> vertex_array;
		std::vector<PreDefineEdge> edge_array;
	};

	struct Strip {
		std::vector<Strip_Region> region;
	};

	struct CDT
	{
		MemoryPool vertex_pool;
		std::vector<Strip> strip_array;
	};


}

void CDT_01(CDT_01_datastruct::PSLG& plsg, CDT_01_datastruct::CDT& cdt) {
	//CDT from Chew's alg
	using namespace CDT_01_datastruct;

	//Step 1: sort vertex
	std::vector<cv::Point2d> tmp_vtx_array;
	for (int i = 0; i < plsg.boundary_array.size(); i++) {
		for (int j = 0; j < plsg.boundary_array[i].point_array.size(); j++) {
			tmp_vtx_array.push_back(plsg.get_position(i, j));
		}
	}
	std::sort(tmp_vtx_array.begin(), tmp_vtx_array.end(), cmp);

	//Step 2: put vertex into pool
	cdt.vertex_pool.initializePool(sizeof(Vertex), 100, 8, 32);
	for (auto& vtx : tmp_vtx_array) {
		auto p = (Vertex*)cdt.vertex_pool.allocate();
		p->position = vtx;
		p->is_infinity = false;
		p->p_connect_edge_array = new std::vector<PreDefineEdge*>();
	}

	//Step 3: add predefine edge
	std::vector<CDT_01_datastruct::PreDefineEdge> edge_array;
	for (int i = 0; i < plsg.boundary_array.size(); i++) {
		for (int j = 0; j < plsg.boundary_array[i].point_array.size(); j++) {
			cv::Point2d start;
			cv::Point2d end;
			if (j == 0) {
				start = plsg.get_position(i, plsg.boundary_array[i].point_array.size() - 1);
				end = plsg.get_position(i, j);
			}
			else {
				start = plsg.get_position(i, j - 1);
				end = plsg.get_position(i, j);
			}
			if (start.x > end.x) {
				std::swap(start,end);
			}

			edge_array.push_back({});
			CDT_01_datastruct::PreDefineEdge& edge = edge_array[edge_array.size() - 1];

			//find index in pool
			cdt.vertex_pool.traversalInit();
			bool find_start = false;
			bool find_end = false;
			while (true) {
				auto next = cdt.vertex_pool.traverse();

				if (find_start && find_end)
					break;

				if (next == (void*)NULL) {
					ASSERT(false);
				}

				auto& cea = ((Vertex*)next)->p_connect_edge_array;

				if (((Vertex*)next)->position == start) {
					edge.orig = (Vertex*)next;
					cea->push_back(&edge);
					find_start = true;
				}
				if (((Vertex*)next)->position == end) {
					edge.end = (Vertex*)next;
					cea->push_back(&edge);
					find_end = true;
				}
			}
		}
	}

	//Step 4: update each vertex edge_below and edge_above using line-Sweep alg
	double line_x_last = 0;
	double line_x_current = 0;
	std::vector<CDT_01_datastruct::PreDefineEdge*> event_queue;
	cdt.vertex_pool.traversalInit();

	while (true) {
		auto next = (Vertex*)cdt.vertex_pool.traverse();
		if (next == (Vertex*)NULL) {
			break;
		}
		line_x_current = next->position.x;

		//update event
		for (auto& e : edge_array) {
			if (e.orig->position.x > line_x_last && e.orig->position.x <= line_x_current) {
				event_queue.push_back(&e);
			}
			if (e.end->position.x >= line_x_last && e.end->position.x < line_x_current) {

				//find which index to remove
				int index = 0;
				for (auto queue_item : event_queue) {
					if (queue_item == &e)
						break;
					index++;
				}
				event_queue.erase(event_queue.begin() + index);
			}
		}

		//find edge_below and edge_above
		priority_queue<CDT_01_datastruct::PreDefineEdge*> edge_intersection_queue;
		for (auto queue_item : event_queue) {
			if (queue_item->end == next || queue_item->orig == next) {
				//if this edge is self's edge, we ignore here
				continue;
			}
			auto res = LineIntersectionCalulate(
				{ {line_x_current,-1000} ,{line_x_current,1000} }
				,
				{ {queue_item->orig->position} ,{queue_item->end->position} }
			);
			edge_intersection_queue.push_back(queue_item, res.intersectionPoint.y);
		}

		if (edge_intersection_queue.size() >= 1) {
			if (edge_intersection_queue.max_priority() > next->position.y) {
				//has above
				next->edge_above = edge_intersection_queue.get_upper(next->position.y).t;
			}
			if (edge_intersection_queue.min_priority() < next->position.y) {
				//has below
				next->edge_below = edge_intersection_queue.get_lower(next->position.y).t;
			}
		}


		line_x_last = line_x_current;
	}


	int a = 0;



	auto strip_merge = [](Strip* t1, Strip* t2)->Strip* {

		Strip* pt = new Strip();
		return pt;
		};

	auto divide_and_conquer = [&](auto&& divide_and_conquer, std::vector<cv::Point2d> vtx_array)->Strip* {

		if (vtx_array.size() > 1) {
			//split
			int center = vtx_array.size() / 2;
			std::vector<cv::Point2d> vtx_array_left = { vtx_array.begin(), vtx_array.begin() + center };
			std::vector<cv::Point2d> vtx_array_right = { vtx_array.begin() + center, vtx_array.end() };
			Strip* t1 = divide_and_conquer(divide_and_conquer, vtx_array_left);
			Strip* t2 = divide_and_conquer(divide_and_conquer, vtx_array_left);
			delete(t1);
			delete(t2);
			return strip_merge(t1, t2);
		}
		//else if (vtx_array.size() = 1) {

		//	p->is_infinity = false;
		//	p->position = vtx_array[0];

		//	Strip* pt = new Strip();
		//	pt->region.push_back({});
		//	pt->region[0].empty = false;
		//	pt->region[0].vertex_array.push_back(p);
		//	return pt;
		//}
		else {
			ASSERT(false);
		}
		};



}