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

	};

	//run time

	struct Edge {
		struct Vertex* orig;
		struct Vertex* end;
	};

	struct Vertex
	{
		cv::Point2d position;
		bool is_infinity = false;
		Edge* edge_below = nullptr;
		Edge* edge_above = nullptr;
		std::vector<Edge*>* p_connect_edge_array;
		//struct Strip_Region* p_connect_region_array[2];//if the vtx is a infinity-vtx, then it may has one or two connect region
		struct Strip_Region* inside_region;//the region belong to

		Vertex() {
			p_connect_edge_array = new std::vector<Edge*>();
		}
		~Vertex() {
			delete(p_connect_edge_array);
		}
	};

	struct Strip_Region {
		bool empty = true;

		Edge* edge_below = nullptr;
		Edge* edge_above = nullptr;
		std::vector<Vertex*> vertex_array;
		std::vector<Vertex*> temp_vertex_array;
		std::vector<Edge> edge_array;
		struct Strip* strip;

		struct {
			Vertex* top_left;
			Vertex* top_right;
			Vertex* buttom_left;
			Vertex* buttom_right;
		}infinity_vtx;
	};

	struct Strip {
		std::vector<Strip_Region*> region;

		double region_left_x = -1;
		double region_right_x = -1;
	};

	struct CDT
	{
		MemoryPool vertex_pool;
		std::vector<Strip*> strip_array;//for draw 
	};


}

void CDT_01(CDT_01_datastruct::PSLG& plsg, CDT_01_datastruct::CDT& cdt) {
	//CDT from L.P. Chew's alg
	//Chew's alg require at least one Constrained edge
	ASSERT(plsg.boundary_array.size() != 0);

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
		p->p_connect_edge_array = new std::vector<Edge*>();
		p->inside_region = nullptr;
	}

	//Step 3: add predefine edge
	std::vector<CDT_01_datastruct::Edge> constrained_edge_array;
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
				std::swap(start, end);
			}

			constrained_edge_array.push_back({});
			CDT_01_datastruct::Edge& edge = constrained_edge_array[constrained_edge_array.size() - 1];

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
	std::vector<CDT_01_datastruct::Edge*> event_queue;
	cdt.vertex_pool.traversalInit();
	while (true) {
		auto next = (Vertex*)cdt.vertex_pool.traverse();
		if (next == (Vertex*)NULL) {
			break;
		}
		line_x_current = next->position.x;

		//update event
		for (auto& e : constrained_edge_array) {
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
		priority_queue<CDT_01_datastruct::Edge*> edge_intersection_queue;
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

		next->edge_above = nullptr;
		next->edge_below = nullptr;
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

	cdt.vertex_pool.traversalInit();

	while (true) {
		auto next = (Vertex*)cdt.vertex_pool.traverse();
		if (next == (Vertex*)NULL) {
			break;
		}

	}

	int a = 0;

	auto strip_region_group_merge = [&](std::vector<Strip_Region*> left_merge_region_group, Strip* strip_left, std::vector<Strip_Region*>right_merge_region_group, Strip* strip_right, bool is_begin_left, bool is_end_left)->Strip_Region* {
		ASSERT(strip_left->region_right_x == strip_right->region_left_x);
		ASSERT(strip_left->region_right_x != -1);
		Strip_Region* new_region = new Strip_Region();
		bool left_group_empty = left_merge_region_group.size() == 0;
		bool right_group_empty = right_merge_region_group.size() == 0;
		//Step 1
		//combine vtx
		{
			for (auto& r : left_merge_region_group) {
				for (auto& vtx : r->vertex_array) {
					new_region->vertex_array.push_back(vtx);
				}
			}

			for (auto& r : right_merge_region_group) {
				for (auto& vtx : r->vertex_array) {
					new_region->vertex_array.push_back(vtx);
				}
			}
		}

		//cal infinity vtx
		{
			//top
			{
				if (is_begin_left) {
					//top-left
					new_region->infinity_vtx.top_left = left_merge_region_group[0]->infinity_vtx.top_left;

					//top-right
					if (strip_right->region_right_x == -1) {
						new_region->infinity_vtx.top_right = nullptr;
					}
					else if (!right_group_empty && left_merge_region_group[0]->edge_above == right_merge_region_group[0]->edge_above) {
						//may share same edge
						new_region->infinity_vtx.top_right = right_merge_region_group[0]->infinity_vtx.top_right;
					}
					else if (left_merge_region_group[0]->edge_above != nullptr) {
						//do intersection
						base_type::IntersectionResult res = LineIntersectionCalulate(
							{ {strip_right->region_right_x,-1000} ,{strip_right->region_right_x,1000} }
							,
							{ {left_merge_region_group[0]->edge_above->orig->position} ,{left_merge_region_group[0]->edge_above->end->position} }
						);
						new_region->infinity_vtx.top_right = new Vertex();
						new_region->infinity_vtx.top_right->position = res.intersectionPoint;
						new_region->infinity_vtx.top_right->is_infinity = true;
					}
				}
				else {
					//top-right
					new_region->infinity_vtx.top_right = right_merge_region_group[0]->infinity_vtx.top_right;

					//top-left
					if (strip_right->region_left_x == -1) {
						new_region->infinity_vtx.top_left = nullptr;
					}
					else if (!left_group_empty && right_merge_region_group[0]->edge_above == left_merge_region_group[0]->edge_above) {
						//may share same edge
						new_region->infinity_vtx.top_left = left_merge_region_group[0]->infinity_vtx.top_left;
					}
					else if (right_merge_region_group[0]->edge_above != nullptr) {
						//do intersection
						base_type::IntersectionResult res = LineIntersectionCalulate(
							{ {strip_left->region_left_x,-1000} ,{strip_right->region_left_x,1000} }
							,
							{ {right_merge_region_group[0]->edge_above->orig->position} ,{right_merge_region_group[0]->edge_above->end->position} }
						);
						new_region->infinity_vtx.top_left = new Vertex();
						new_region->infinity_vtx.top_left->position = res.intersectionPoint;
						new_region->infinity_vtx.top_left->is_infinity = true;
					}
				}

			}

			//down
			{
				if (is_end_left) {
					//buttom-left
					new_region->infinity_vtx.buttom_left = left_merge_region_group[0]->infinity_vtx.buttom_left;

					//buttom-right
					if (strip_right->region_right_x == -1) {
						new_region->infinity_vtx.buttom_right = nullptr;
					}
					else if (!right_group_empty && left_merge_region_group[0]->edge_below == right_merge_region_group[0]->edge_below) {
						//may share same edge
						new_region->infinity_vtx.buttom_right = right_merge_region_group[0]->infinity_vtx.buttom_right;
					}
					else if (left_merge_region_group[0]->edge_below != nullptr) {
						//do intersection
						base_type::IntersectionResult res = LineIntersectionCalulate(
							{ {strip_right->region_right_x,-1000} ,{strip_right->region_right_x,1000} }
							,
							{ {left_merge_region_group[0]->edge_below->orig->position} ,{left_merge_region_group[0]->edge_below->end->position} }
						);
						new_region->infinity_vtx.buttom_right = new Vertex();
						new_region->infinity_vtx.buttom_right->position = res.intersectionPoint;
						new_region->infinity_vtx.buttom_right->is_infinity = true;
					}
				}
				else {
					//buttom-right
					new_region->infinity_vtx.buttom_right = right_merge_region_group[0]->infinity_vtx.buttom_right;

					//buttom-left
					if (strip_right->region_left_x == -1) {
						new_region->infinity_vtx.buttom_left = nullptr;
					}
					else if (!left_group_empty && right_merge_region_group[0]->edge_below == left_merge_region_group[0]->edge_below) {
						//may share same edge
						new_region->infinity_vtx.buttom_left = left_merge_region_group[0]->infinity_vtx.buttom_left;
					}
					else if (right_merge_region_group[0]->edge_below != nullptr) {
						//do intersection
						base_type::IntersectionResult res = LineIntersectionCalulate(
							{ {strip_left->region_left_x,-1000} ,{strip_right->region_left_x,1000} }
							,
							{ {right_merge_region_group[0]->edge_below->orig->position} ,{right_merge_region_group[0]->edge_below->end->position} }
						);
						new_region->infinity_vtx.buttom_left = new Vertex();
						new_region->infinity_vtx.buttom_left->position = res.intersectionPoint;
						new_region->infinity_vtx.buttom_left->is_infinity = true;
					}
				}

			}
		}

		//copy edge
		{
			auto is_infinity_edge = [](Edge* e) ->bool {return e->end->is_infinity || e->orig->is_infinity; };

			for (auto& r : left_merge_region_group) {
				for (auto& e : r->edge_array) {
					if (!is_infinity_edge(&e))
						new_region->edge_array.push_back(e);
				}
			}

			for (auto& r : right_merge_region_group) {
				for (auto& e : r->edge_array) {
					if (!is_infinity_edge(&e))
						new_region->edge_array.push_back(e);
				}
			}
		}

		auto is_vtx_in_vtx_array = [](Vertex* vtx, std::vector<Vertex*> vtx_array) {
			return std::find(vtx_array.begin(), vtx_array.end(), vtx) != vtx_array.end();
			};
		auto is_edge_cross = [](double x, Edge* e)->bool {return (e->end->position.x<x && e->orig->position.x>x) || (e->end->position.x > x && e->orig->position.x < x); };

		//add edges that cross the boundary between the two strips
		std::vector<Edge*> cross_edge_array;
		for (Edge& e : constrained_edge_array) {

			if (is_vtx_in_vtx_array(e.end, new_region->vertex_array) && is_vtx_in_vtx_array(e.orig, new_region->vertex_array)) {
				new_region->temp_vertex_array.push_back({});
				auto new_cross_edge = new_region->edge_array[new_region->edge_array.size() - 1];
				new_cross_edge.orig = e.orig;
				new_cross_edge.end = e.end;
			}

			else if ((is_vtx_in_vtx_array(e.end, new_region->vertex_array) || is_vtx_in_vtx_array(e.orig, new_region->vertex_array)) && is_edge_cross(strip_left->region_right_x, &e)) {
				//find new edge start
				Vertex* new_edge_start;
				{
					if (is_vtx_in_vtx_array(e.end, new_region->vertex_array))
					{
						new_edge_start = e.end;

					}
					else
					{
						new_edge_start = e.orig;
					}
				}

				//find new_edge end
				new_region->edge_array.push_back({});
				auto new_cross_edge = new_region->edge_array[new_region->edge_array.size() - 1];

				if (new_edge_start->position.x < strip_left->region_right_x) {
					//there is already one vtx can be use, must be one of inf vtx 
					ASSERT(strip_right->region_right_x != -1 && (e.orig->position.x > strip_right->region_right_x || e.end->position.x > strip_right->region_right_x));
					Strip_Region* target_region = nullptr;
					for (auto& r : right_merge_region_group) {
						if (r->edge_above == &e || r->edge_below == &e) {
							target_region = r;
							break;
						}
					}
					ASSERT(target_region != nullptr);
					auto& v = target_region->edge_below == &e ? target_region->infinity_vtx.buttom_right : target_region->infinity_vtx.top_right;
					ASSERT(v->is_infinity == true);

					v->p_connect_edge_array->push_back(&new_cross_edge);//TODO:how to delete othre unsed edge
					new_cross_edge.orig = new_edge_start;
					new_cross_edge.end = v;
					cross_edge_array.push_back(&new_cross_edge);
				}
				else {
					//there is already one vtx can be use, must be one of inf vtx 
					ASSERT(strip_left->region_left_x != -1 && (e.orig->position.x > strip_left->region_left_x || e.end->position.x > strip_left->region_left_x));
					Strip_Region* target_region = nullptr;
					for (auto& r : right_merge_region_group) {
						if (r->edge_above == &e || r->edge_below == &e) {
							target_region = r;
							break;
						}
					}
					ASSERT(target_region != nullptr);
					auto& v = target_region->edge_below == &e ? target_region->infinity_vtx.buttom_left : target_region->infinity_vtx.top_left;
					ASSERT(v->is_infinity == true);

					v->p_connect_edge_array->push_back(&new_cross_edge);//TODO:how to delete othre unsed edge
					new_cross_edge.orig = new_edge_start;
					new_cross_edge.end = v;
					cross_edge_array.push_back(&new_cross_edge);

				}
			}
		}

		//Step 2
		for (auto& cross_edge : cross_edge_array) {
			Vertex* A = cross_edge->orig;
			Vertex* B = cross_edge->end;
			while (true) {
				//1) Eliminate A - edges that can be shown to be illegal because of their interaction with B;
				auto eliminate_illegal_egde = [](std::vector<Edge*>& edge_array, std::vector<Vertex*> check_vtx, Vertex* interaction_vtx) {
					auto it = edge_array.begin();
					while (it != edge_array.end()) {
						bool is_illegal = false;
						auto edge = *it;
						cv::Point2d center;
						double radius;

						CalculateBoundingCircle<cv::Point2d>(edge->orig->position, edge->end->position, interaction_vtx->position, center, radius);
						double radius2 = radius * radius;
						for (auto vtx : check_vtx) {
							if (vtx == edge->orig || vtx == edge->end || interaction_vtx)
								continue;

							if (VectorLengthSqr(vtx->position, center) < radius2) {
								is_illegal = true;
								break;
							}
						}
						if (is_illegal) {
							it = edge_array.erase(it);
						}
						else {
							it++;
						}
					}

					};

				eliminate_illegal_egde(*(A->p_connect_edge_array), new_region->vertex_array, B);

				//2) Eliminate B - edges that can be shown to be illegal because of their interaction with A;
				eliminate_illegal_egde(*(B->p_connect_edge_array), new_region->vertex_array, A);

				//3) Let C be a candidate where AC is the next edge counterclockwise around A from AB(if such an edge exists);
				auto find_next_edge_ccw = [](Edge* edge, std::vector<Edge*> edge_array)->Edge* {
					ASSERT(edge_array.size() != 0);

					//get rid of self
					std::vector<Edge*> edge_array_copy(edge_array.begin(), edge_array.end());
					{
						auto t = std::find(edge_array_copy.begin(), edge_array_copy.end(), edge);
						if (t != edge_array_copy.end()) {
							edge_array_copy.erase(t);
						}
					}

					if (edge_array_copy.size() == 0)
						return nullptr;

					if (edge_array_copy.size() == 1)
						return edge_array_copy[0];

					Vertex* common_vtx;
					Vertex* non_common_vtx;
					if (edge->end == edge_array_copy[0]->orig || edge->end == edge_array_copy[0]->end)
					{
						common_vtx = edge->end;
						non_common_vtx = edge->orig;
					}
					else
					{
						non_common_vtx = edge->orig;
						common_vtx = edge->end;
					}

					Edge* return_val = nullptr;

					bool is_next_edge_at_left = false;
					for (int i = 0; i < edge_array_copy.size(); i++) {
						auto& ei = edge_array_copy[i];
						ASSERT(ei->orig == common_vtx || ei->end == common_vtx);
						auto ei_non_common_vtx = ei->orig != common_vtx ? ei->orig : ei->end;
						if (ToLeft(common_vtx->position, non_common_vtx->position,/*<*/ ei_non_common_vtx->position)) {
							is_next_edge_at_left = true;
							return_val = ei;
							break;
						}

					}

					if (is_next_edge_at_left) {
						for (int i = 0; i < edge_array_copy.size(); i++) {
							auto& ei = edge_array_copy[i];
							ASSERT(ei->orig == common_vtx || ei->end == common_vtx);
							auto ei_non_common_vtx = ei->orig != common_vtx ? ei->orig : ei->end;
							if (ToLeft(common_vtx->position, non_common_vtx->position, ei_non_common_vtx->position)) {
								if (ToLeft(common_vtx->position, ei_non_common_vtx->position, return_val->orig != common_vtx ? return_val->orig->position : return_val->end->position)) {
									return_val = ei;
								}
							}

						}
					}
					else {
						for (int i = 0; i < edge_array_copy.size(); i++) {
							auto& ei = edge_array_copy[i];
							ASSERT(ei->orig == common_vtx || ei->end == common_vtx);
							auto ei_non_common_vtx = ei->orig != common_vtx ? ei->orig : ei->end;
							if (!ToLeft(common_vtx->position, non_common_vtx->position, ei_non_common_vtx->position)) {
								if (!ToLeft(common_vtx->position, ei_non_common_vtx->position, return_val->orig != common_vtx ? return_val->orig->position : return_val->end->position)) {
									return_val = ei;
								}
							}

						}
					}
					return return_val;
					};
				auto find_next_edge_cw = [](Edge* edge, std::vector<Edge*> edge_array)->Edge* {
					ASSERT(edge_array.size() != 0);

					//get rid of self
					std::vector<Edge*> edge_array_copy(edge_array.begin(), edge_array.end());
					{
						auto t = std::find(edge_array_copy.begin(), edge_array_copy.end(), edge);
						if (t != edge_array_copy.end()) {
							edge_array_copy.erase(t);
						}
					}

					if (edge_array_copy.size() == 0)
						return nullptr;

					if (edge_array_copy.size() == 1)
						return edge_array_copy[0];

					Vertex* common_vtx;
					Vertex* non_common_vtx;
					if (edge->end == edge_array_copy[0]->orig || edge->end == edge_array_copy[0]->end)
					{
						common_vtx = edge->end;
						non_common_vtx = edge->orig;
					}
					else
					{
						non_common_vtx = edge->orig;
						common_vtx = edge->end;
					}

					Edge* return_val = nullptr;

					bool is_next_edge_at_right = false;
					for (int i = 0; i < edge_array_copy.size(); i++) {
						auto& ei = edge_array_copy[i];
						ASSERT(ei->orig == common_vtx || ei->end == common_vtx);
						auto ei_non_common_vtx = ei->orig != common_vtx ? ei->orig : ei->end;
						if (!ToLeft(common_vtx->position, non_common_vtx->position,/*<*/ ei_non_common_vtx->position)) {
							is_next_edge_at_right = true;
							return_val = ei;
							break;
						}

					}

					if (is_next_edge_at_right) {
						for (int i = 0; i < edge_array_copy.size(); i++) {
							auto& ei = edge_array_copy[i];
							ASSERT(ei->orig == common_vtx || ei->end == common_vtx);
							auto ei_non_common_vtx = ei->orig != common_vtx ? ei->orig : ei->end;
							if (!ToLeft(common_vtx->position, non_common_vtx->position, ei_non_common_vtx->position)) {
								if (ToLeft(common_vtx->position, ei_non_common_vtx->position, return_val->orig != common_vtx ? return_val->orig->position : return_val->end->position)) {
									return_val = ei;
								}
							}

						}
					}
					else {
						for (int i = 0; i < edge_array_copy.size(); i++) {
							auto& ei = edge_array_copy[i];
							ASSERT(ei->orig == common_vtx || ei->end == common_vtx);
							auto ei_non_common_vtx = ei->orig != common_vtx ? ei->orig : ei->end;
							if (ToLeft(common_vtx->position, non_common_vtx->position, ei_non_common_vtx->position)) {
								if (!ToLeft(common_vtx->position, ei_non_common_vtx->position, return_val->orig != common_vtx ? return_val->orig->position : return_val->end->position)) {
									return_val = ei;
								}
							}

						}
					}
					return return_val;
					};
				Vertex* C = nullptr;
				Edge* AC = find_next_edge_ccw(cross_edge, *(A->p_connect_edge_array));
				if (AC != nullptr) {
					C = AC->end != A ? AC->end : AC->orig;
				}
				//4) Let D be a candidate where BD is the next edge  clockwise around B from BA(if such an edge exists) :
				Vertex* D = nullptr;
				Edge* BD = find_next_edge_ccw(cross_edge, *(B->p_connect_edge_array));
				if (BD != nullptr) {
					D = BD->end != B ? BD->end : BD->orig;
				}

				//break if no candidates exist;
				if (C == nullptr && D == nullptr) {
					break;
				}
				//5) Let X be the candidate that correspcnds to the lower of circles ABC and ABD;
				Vertex* X = nullptr;
				if (C == nullptr) {
					X = D;
				}
				else if (D == nullptr)
				{
					X = C;
				}
				else
				{
					cv::Point2d ABC_center;
					double ABC_radius;
					CalculateBoundingCircle<cv::Point2d>(A->position, B->position, C->position, ABC_center, ABC_radius);

					cv::Point2d ABD_center;
					double ABD_radius;
					CalculateBoundingCircle<cv::Point2d>(A->position, B->position, D->position, ABD_center, ABD_radius);

					if (ABC_center.y < ABD_center.y) {
						X = C;
					}
					else
					{
						X = D;
					}
				}
				//6)Add edge AX or BX as appropriate and call this new edge AB;
				new_region->edge_array.push_back({});
				Edge* new_edge = &new_region->edge_array[new_region->edge_array.size() - 1];
				if (X = C) {
					new_edge->orig = B;
					new_edge->end = X;
				}
				else {
					new_edge->orig = A;
					new_edge->end = X;
				}
				A = new_edge->orig;
				B = new_edge->end;
			}

		}


		return new_region;
		};

	auto strip_merge = [&](Strip* t1, Strip* t2)->Strip* {

		Strip* new_strip = new Strip();

		//TODO:is region order top-to-down

		int region_size_t1 = t1->region.size();
		int region_size_t2 = t2->region.size();
		int region_index_t1 = 0;
		int region_index_t2 = 0;

		//find a start
		auto cmp_start = [&](int index_l, int index_r, bool& is_left)->Strip_Region* {
			Strip_Region* region;

			if (index_l >= region_size_t1) {
				region = t2->region[index_r];
				is_left = false;
			}
			else if (index_r >= region_size_t2) {
				region = t1->region[index_l];
				is_left = true;
			}
			else {
				auto inf_t1_tl = t1->region[index_l]->infinity_vtx.top_left;
				auto inf_t2_tr = t2->region[index_r]->infinity_vtx.top_right;

				if (inf_t1_tl == nullptr) {
					region = t1->region[index_l];
					is_left = true;
				}
				else if (inf_t2_tr == nullptr) {
					region = t2->region[index_r];
					is_left = false;
				}
				else {
					region = inf_t1_tl->position.y > inf_t2_tr->position.y ? t1->region[index_l] : t2->region[index_r];
					is_left = inf_t1_tl->position.y > inf_t2_tr->position.y ? true : false;
				}
			}

			return region;
			};
		auto is_region_connect = [&](Strip_Region* region_left, Strip_Region* region_right)->bool {
			bool res = false;
			if (region_left->infinity_vtx.top_right == nullptr && region_left->infinity_vtx.buttom_right == nullptr) {
				res = true;
			}
			else if (region_right->infinity_vtx.top_left == nullptr && region_right->infinity_vtx.buttom_left == nullptr) {
				res = true;
			}
			else if (region_left->infinity_vtx.top_right == nullptr && region_right->infinity_vtx.top_left == nullptr) {
				res = true;
			}
			else if (region_left->infinity_vtx.buttom_right == nullptr && region_right->infinity_vtx.buttom_left == nullptr) {
				res = true;
			}

			else if (region_left->infinity_vtx.top_right == nullptr && region_right->infinity_vtx.top_left->position.y >= region_left->infinity_vtx.buttom_right->position.y) {
				res = true;
			}
			else if (region_right->infinity_vtx.top_left == nullptr && region_left->infinity_vtx.top_right->position.y >= region_right->infinity_vtx.buttom_left->position.y) {
				res = true;
			}
			else if (region_left->infinity_vtx.buttom_right == nullptr && region_right->infinity_vtx.buttom_left->position.y <= region_left->infinity_vtx.top_right->position.y) {
				res = true;
			}
			else if (region_right->infinity_vtx.buttom_left == nullptr && region_left->infinity_vtx.buttom_right->position.y <= region_right->infinity_vtx.top_left->position.y) {
				res = true;
			}


			return res;

			};
		auto get_connect_region = [&](Strip_Region* region, Strip* strip, bool is_left)->std::vector<Strip_Region*> {
			std::vector<Strip_Region*> res;
			for (auto& region_item : strip->region) {
				if (is_left && is_region_connect(region, region_item)) {
					res.push_back(region_item);
				}
				else if (!is_left && is_region_connect(region_item, region)) {
					res.push_back(region_item);
				}
			}
			return res;
			};
		auto left_buttom_lower = [](Strip_Region* region_left, Strip_Region* region_right)->bool {
			if (region_left->infinity_vtx.buttom_right == nullptr) {
				return true;
			}
			if (region_right->infinity_vtx.buttom_left == nullptr) {
				return false;
			}
			return region_left->infinity_vtx.buttom_right->position.y < region_right->infinity_vtx.buttom_left->position.y;

			};

		while (region_index_t1 < region_size_t1 || region_index_t2 < region_size_t2) {
			//begin merge all sub region

			//find sub region merge group
			{
				bool is_begin_left, is_end_left = true;
				Strip_Region* merge_sub_region_begin;

				merge_sub_region_begin = cmp_start(region_index_t1, region_index_t2, is_begin_left);

				std::vector<Strip_Region*> merge_region_group_left;
				std::vector<Strip_Region*> merge_region_group_right;
				//prepare  merge_sub_region_left and merge_sub_region_right
				{
					if (is_begin_left) {
						merge_region_group_left.push_back(merge_sub_region_begin);
						region_index_t1++;
					}
					else {
						merge_region_group_right.push_back(merge_sub_region_begin);
						region_index_t2++;
					}

					//s1: inf bottom is nullptr
					if (merge_sub_region_begin->infinity_vtx.buttom_right == nullptr && is_begin_left) {
						//put a sub region in right of range(merge_sub_region_begin..top_left.y,nullptr)
						for (; region_index_t2 < region_size_t2; region_index_t2++) {
							merge_region_group_right.push_back(t2->region[region_index_t2]);
						}

					}
					else if (merge_sub_region_begin->infinity_vtx.buttom_left == nullptr && !is_begin_left)
					{
						for (; region_index_t1 < region_size_t1; region_index_t1++) {
							merge_region_group_left.push_back(t1->region[region_index_t1]);
						}

					}
					else {
						//get here means merge_sub_region_begin->infinity_vtx.buttom is nuo nullptr
						Strip* search_strip = is_begin_left ? t2 : t1;
						bool serach_from = is_begin_left;
						Strip_Region* search_region = merge_sub_region_begin;
						std::vector<Strip_Region*>* res_to_put_vector;
						{
							if (serach_from) {
								res_to_put_vector = &merge_region_group_right;
							}
							else {
								res_to_put_vector = &merge_region_group_left;
							}
						}

						while (true) {
							auto res = get_connect_region(search_region, search_strip, serach_from);
							if (res.size() > 0) {
								bool has_find_new_region = false;
								for (auto& res_item : res) {
									if (std::find((*res_to_put_vector).begin(), (*res_to_put_vector).end(), res_item) != (*res_to_put_vector).end()) {
										(*res_to_put_vector).push_back(res_item);
										serach_from ? region_index_t2++ : region_index_t1++;
										has_find_new_region = true;
									}
								}

								if (has_find_new_region) {
									search_strip = search_strip == t2 ? t1 : t2;
									serach_from = !serach_from;
									search_region = res[res.size() - 1];
									res_to_put_vector = serach_from ? &merge_region_group_right : &merge_region_group_left;
								}
								else {
									break;
								}

							}
							else {
								break;
							}
						}

					}
				}
				//calculate is_end_left
				if (merge_region_group_left.size() == 0)
					is_end_left = false;
				else if (merge_region_group_right.size() == 0) {
					is_end_left = true;
				}
				else {
					is_end_left = left_buttom_lower(merge_region_group_left[merge_region_group_left.size() - 1], merge_region_group_right[merge_region_group_right.size() - 1]);
				}

				auto new_region = strip_region_group_merge(merge_region_group_left, t1, merge_region_group_right, t2, is_begin_left, is_end_left);
				new_strip->region.push_back(new_region);
			}




		}

		//delete(t1);
		//delete(t2);
		return new_strip;
		};

	auto divide_and_conquer = [&](auto&& divide_and_conquer, int index_begin, int index_end)->Strip* {
		int size = index_end - index_begin;

		if (size > 1) {
			//split
			int center = size / 2 + index_begin;

			Strip* t1 = divide_and_conquer(divide_and_conquer, index_begin, center);
			Strip* t2 = divide_and_conquer(divide_and_conquer, center, index_end);

			return strip_merge(t1, t2);
		}
		else if (size == 1) {
			auto vtx = (Vertex*)cdt.vertex_pool[index_begin];
			Strip* strip = new Strip();
			cdt.strip_array.push_back(strip);
			strip->region.push_back(new Strip_Region());
			Strip_Region* region = (strip->region[0]);
			region->infinity_vtx.buttom_left = nullptr;
			region->infinity_vtx.buttom_right = nullptr;
			region->infinity_vtx.top_left = nullptr;
			region->infinity_vtx.top_right = nullptr;

			region->empty = false;
			region->vertex_array.push_back(vtx);
			region->strip = strip;
			vtx->inside_region = region;
			vtx->p_connect_edge_array = new std::vector<Edge*>();
			//calculate region infinity point
			if (index_begin != 0 && index_begin != cdt.vertex_pool.items - 1) {

				auto vtx_left = (Vertex*)cdt.vertex_pool[index_begin - 1];
				auto vtx_right = (Vertex*)cdt.vertex_pool[index_begin + 1];

				double region_x_left = vtx_left->position.x + (vtx->position.x - vtx_left->position.x) / 2;
				double region_x_right = vtx->position.x + (vtx_right->position.x - vtx->position.x) / 2;

				strip->region_left_x = region_x_left;
				strip->region_right_x = region_x_right;

				if (vtx->edge_above != nullptr) {
					region->edge_above = vtx->edge_above;
					//top-left
					{
						if (vtx_left->edge_above == vtx->edge_above && vtx_left->inside_region != nullptr && vtx_left->inside_region->infinity_vtx.top_right != nullptr) {
							//if they share the same above edge, then vtx_left's top-right infinity is vtx's top-left infinity
							region->infinity_vtx.top_left = vtx_left->inside_region->infinity_vtx.top_right;
						}
						else {
							//intersection test and make infinity
							base_type::IntersectionResult res = LineIntersectionCalulate(
								{ {region_x_left,-1000} ,{region_x_left,1000} }
								,
								{ {vtx->edge_above->orig->position} ,{vtx->edge_above->end->position} }
							);
							region->infinity_vtx.top_left = new Vertex();
							region->infinity_vtx.top_left->is_infinity = true;

							region->infinity_vtx.top_left->position = res.intersectionPoint;
						}
					}

					//top-right
					{
						if (vtx_right->edge_above == vtx->edge_above && vtx_right->inside_region != nullptr && vtx_right->inside_region->infinity_vtx.top_left != nullptr) {
							region->infinity_vtx.top_right = vtx_right->inside_region->infinity_vtx.top_left;
						}
						else {
							//intersection test and make infinity
							base_type::IntersectionResult res = LineIntersectionCalulate(
								{ {region_x_right,-1000} ,{region_x_right,1000} }
								,
								{ {vtx->edge_above->orig->position} ,{vtx->edge_above->end->position} }
							);
							region->infinity_vtx.top_right = new Vertex();
							region->infinity_vtx.top_right->is_infinity = true;

							region->infinity_vtx.top_right->position = res.intersectionPoint;
						}
					}

				}
				if (vtx->edge_below != nullptr) {
					region->edge_below = vtx->edge_below;
					//buttom-left
					{
						if (vtx_left->edge_below == vtx->edge_below && vtx_left->inside_region != nullptr && vtx_left->inside_region->infinity_vtx.buttom_right != nullptr) {
							region->infinity_vtx.buttom_left = vtx_left->inside_region->infinity_vtx.buttom_right;
						}
						else {
							//intersection test and make infinity
							base_type::IntersectionResult res = LineIntersectionCalulate(
								{ {region_x_left,-1000} ,{region_x_left,1000} }
								,
								{ {vtx->edge_below->orig->position} ,{vtx->edge_below->end->position} }
							);
							region->infinity_vtx.buttom_left = new Vertex();
							region->infinity_vtx.buttom_left->is_infinity = true;

							region->infinity_vtx.buttom_left->position = res.intersectionPoint;
						}
					}

					//buttom-right
					{
						if (vtx_right->edge_below == vtx->edge_below && vtx_right->inside_region != nullptr && vtx_right->inside_region->infinity_vtx.buttom_left != nullptr) {
							region->infinity_vtx.buttom_right = vtx_right->inside_region->infinity_vtx.buttom_left;
						}
						else {
							//intersection test and make infinity
							base_type::IntersectionResult res = LineIntersectionCalulate(
								{ {region_x_right,-1000} ,{region_x_right,1000} }
								,
								{ {vtx->edge_below->orig->position} ,{vtx->edge_below->end->position} }
							);
							region->infinity_vtx.buttom_right = new Vertex();
							region->infinity_vtx.buttom_right->is_infinity = true;
							region->infinity_vtx.buttom_right->position = res.intersectionPoint;
						}
					}
				}

			}

			else if (index_begin == 0) {
				auto vtx_right = (Vertex*)cdt.vertex_pool[index_begin + 1];
				double region_x_right = vtx->position.x + (vtx_right->position.x - vtx->position.x) / 2;
				strip->region_right_x = region_x_right;

				if (vtx->edge_above != nullptr) {


					//top-right
					{
						if (vtx_right->edge_above == vtx->edge_above && vtx_right->inside_region != nullptr && vtx_right->inside_region->infinity_vtx.top_left != nullptr) {
							region->infinity_vtx.top_right = vtx_right->inside_region->infinity_vtx.top_left;
						}
						else {
							//intersection test and make infinity
							base_type::IntersectionResult res = LineIntersectionCalulate(
								{ {region_x_right,-1000} ,{region_x_right,1000} }
								,
								{ {vtx->edge_above->orig->position} ,{vtx->edge_above->end->position} }
							);
							region->infinity_vtx.top_right = new Vertex();
							region->infinity_vtx.top_right->is_infinity = true;
							region->infinity_vtx.top_right->position = res.intersectionPoint;
						}
					}

				}
				if (vtx->edge_below != nullptr) {

					//buttom-right
					{
						if (vtx_right->edge_below == vtx->edge_below && vtx_right->inside_region != nullptr && vtx_right->inside_region->infinity_vtx.buttom_left != nullptr) {
							region->infinity_vtx.buttom_right = vtx_right->inside_region->infinity_vtx.buttom_left;
						}
						else {
							//intersection test and make infinity
							base_type::IntersectionResult res = LineIntersectionCalulate(
								{ {region_x_right,-1000} ,{region_x_right,1000} }
								,
								{ {vtx->edge_below->orig->position} ,{vtx->edge_below->end->position} }
							);
							region->infinity_vtx.buttom_right = new Vertex();
							region->infinity_vtx.buttom_right->is_infinity = true;
							region->infinity_vtx.buttom_right->position = res.intersectionPoint;
						}
					}
				}

			}
			else {
				ASSERT(index_begin == cdt.vertex_pool.items - 1);
				auto vtx_left = (Vertex*)cdt.vertex_pool[index_begin - 1];
				double region_x_left = vtx_left->position.x + (vtx->position.x - vtx_left->position.x) / 2;
				strip->region_left_x = region_x_left;

				if (vtx->edge_above != nullptr) {
					//top-left
					{
						if (vtx_left->edge_above == vtx->edge_above && vtx_left->inside_region != nullptr && vtx_left->inside_region->infinity_vtx.top_right != nullptr) {
							//if they share the same above edge, then vtx_left's top-right infinity is vtx's top-left infinity
							region->infinity_vtx.top_left = vtx_left->inside_region->infinity_vtx.top_right;
						}
						else {
							//intersection test and make infinity
							base_type::IntersectionResult res = LineIntersectionCalulate(
								{ {region_x_left,-1000} ,{region_x_left,1000} }
								,
								{ {vtx->edge_above->orig->position} ,{vtx->edge_above->end->position} }
							);
							region->infinity_vtx.top_left = new Vertex();
							region->infinity_vtx.top_left->is_infinity = true;
							region->infinity_vtx.top_left->position = res.intersectionPoint;
						}
					}
				}
				if (vtx->edge_below != nullptr) {
					//buttom-left
					{
						if (vtx_left->edge_below == vtx->edge_below && vtx_left->inside_region != nullptr && vtx_left->inside_region->infinity_vtx.buttom_right != nullptr) {
							region->infinity_vtx.buttom_left = vtx_left->inside_region->infinity_vtx.buttom_right;
						}
						else {
							//intersection test and make infinity
							base_type::IntersectionResult res = LineIntersectionCalulate(
								{ {region_x_left,-1000} ,{region_x_left,1000} }
								,
								{ {vtx->edge_below->orig->position} ,{vtx->edge_below->end->position} }
							);
							region->infinity_vtx.buttom_left = new Vertex();
							region->infinity_vtx.buttom_left->is_infinity = true;
							region->infinity_vtx.buttom_left->position = res.intersectionPoint;
						}
					}
				}
			}

			return strip;
		}
		else {
			ASSERT(false);
		}
		};


	divide_and_conquer(divide_and_conquer, 0, cdt.vertex_pool.items);
}