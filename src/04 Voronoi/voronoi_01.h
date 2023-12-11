#pragma once

#include "common/typedef.h"
#include "common/helper.h"

extern void Intersection_01(std::vector<Line>& all_lines, std::vector <IntersectionResult>& intersectionResults);

namespace DECL_Voronoi {
	//doubly connected edge list (DCEL), https://github.com/AnkurRyder/DCEL

	class Vertex
	{
	public:
		uint id;
		cv::Point2d position;
		class HalfEdge* incident_edge;
		Vertex(cv::Point2d _position) {
			position = _position;
			incident_edge = nullptr;
		}
		Vertex() {}
	};

	class HalfEdge
	{
	public:
		uint id;
		HalfEdge* twin;
		HalfEdge* pred;
		HalfEdge* succ;
		Vertex* origin, * end;
		class Face* incident_face;
		bool isBorder = false;
		HalfEdge(Vertex* _origin, Vertex* _end) {

			origin = _origin;
			end = _end;
			incident_face = nullptr;
			twin = nullptr;
			pred = nullptr;
			succ = nullptr;
		}
	};

	class Face
	{
	public:
		int id;
		HalfEdge* incident_edge;
		class Site* incident_site;
		Face() {
			incident_edge = nullptr;
			incident_site = nullptr;
		}
	};

	class Site {
	public:
		cv::Point2d position;
		Face* incident_face;
		Site(cv::Point2d _position) {
			position = _position;
		}
	};

	class DECL {
	public:

		std::vector<Vertex*> vertex_list;
		std::vector<Site>site_list;

		cv::Point2d boundary;

		void AddSite(cv::Point2d s) {
			site_list.push_back(s);
		}

		void AddVertex(Vertex* v) {
			vertex_list.push_back(v);
		}
		void DelVertex(Vertex* v) {
			for (auto iter = vertex_list.begin(); iter != vertex_list.end(); iter++) {
				if (*iter == v) {
					vertex_list.erase(iter);
					break;
				}
			}
		}

		DECL() {
			vertex_list.reserve(300);
			site_list.reserve(100);
		}
	};
}


void Voronoi_01(std::vector<cv::Point>& all_point, DECL_Voronoi::DECL& decl) {
	//Incremental Construction

	struct newRecord {
		cv::Point2d position;
		DECL_Voronoi::HalfEdge* half_edge;
	};
	auto create_pari_edge = [](DECL_Voronoi::Vertex* f, DECL_Voronoi::Vertex* t) {
		DECL_Voronoi::HalfEdge* edge;
		DECL_Voronoi::HalfEdge* edge_twin;

		edge = new DECL_Voronoi::HalfEdge(f, t);
		edge_twin = new DECL_Voronoi::HalfEdge(t, f);

		edge->twin = edge_twin;
		edge_twin->twin = edge;

		return edge;
		};

	auto Incremental_construction = [&](const cv::Point2d site, DECL_Voronoi::DECL& decl) {
		if (decl.site_list.size() == 0) {
			decl.AddSite(site);
			DECL_Voronoi::Site* site_to_add = &(*(decl.site_list.end() - 1));

			DECL_Voronoi::Face* face_to_add = new DECL_Voronoi::Face();
			face_to_add->incident_site = site_to_add;
			site_to_add->incident_face = face_to_add;

			DECL_Voronoi::Vertex* v0 = new DECL_Voronoi::Vertex({ 0,0 });
			DECL_Voronoi::Vertex* v1 = new DECL_Voronoi::Vertex({ decl.boundary.x,0 });
			DECL_Voronoi::Vertex* v2 = new DECL_Voronoi::Vertex({ 0,decl.boundary.y });
			DECL_Voronoi::Vertex* v3 = new DECL_Voronoi::Vertex({ decl.boundary.x,decl.boundary.y });

			DECL_Voronoi::HalfEdge* edge_to_add_0 = new DECL_Voronoi::HalfEdge(v0, v1);
			DECL_Voronoi::HalfEdge* edge_to_add_twin_0 = new DECL_Voronoi::HalfEdge(v1, v0);
			edge_to_add_0->twin = edge_to_add_twin_0;
			edge_to_add_twin_0->twin = edge_to_add_0;
			edge_to_add_0->incident_face = face_to_add;
			edge_to_add_twin_0->incident_face = nullptr;
			edge_to_add_0->isBorder = true;
			edge_to_add_twin_0->isBorder = true;

			DECL_Voronoi::HalfEdge* edge_to_add_1 = new DECL_Voronoi::HalfEdge(v1, v3);
			DECL_Voronoi::HalfEdge* edge_to_add_twin_1 = new DECL_Voronoi::HalfEdge(v3, v1);
			edge_to_add_1->twin = edge_to_add_twin_1;
			edge_to_add_twin_1->twin = edge_to_add_1;
			edge_to_add_1->incident_face = face_to_add;
			edge_to_add_twin_1->incident_face = nullptr;
			edge_to_add_1->isBorder = true;
			edge_to_add_twin_1->isBorder = true;

			DECL_Voronoi::HalfEdge* edge_to_add_2 = new DECL_Voronoi::HalfEdge(v2, v3);
			DECL_Voronoi::HalfEdge* edge_to_add_twin_2 = new DECL_Voronoi::HalfEdge(v3, v2);
			edge_to_add_2->twin = edge_to_add_twin_2;
			edge_to_add_twin_2->twin = edge_to_add_2;
			edge_to_add_2->incident_face = face_to_add;
			edge_to_add_twin_2->incident_face = nullptr;
			edge_to_add_2->isBorder = true;
			edge_to_add_twin_2->isBorder = true;

			DECL_Voronoi::HalfEdge* edge_to_add_3 = new DECL_Voronoi::HalfEdge(v2, v0);
			DECL_Voronoi::HalfEdge* edge_to_add_twin_3 = new DECL_Voronoi::HalfEdge(v0, v2);
			edge_to_add_3->twin = edge_to_add_twin_3;
			edge_to_add_twin_3->twin = edge_to_add_3;
			edge_to_add_3->incident_face = face_to_add;
			edge_to_add_twin_3->incident_face = nullptr;
			edge_to_add_3->isBorder = true;
			edge_to_add_twin_3->isBorder = true;

			edge_to_add_0->succ = edge_to_add_1;
			edge_to_add_1->succ = edge_to_add_2;
			edge_to_add_2->succ = edge_to_add_3;
			edge_to_add_3->succ = edge_to_add_0;
			edge_to_add_0->pred = edge_to_add_3;
			edge_to_add_1->pred = edge_to_add_0;
			edge_to_add_2->pred = edge_to_add_1;
			edge_to_add_3->pred = edge_to_add_2;

			edge_to_add_twin_0->succ = edge_to_add_twin_1;
			edge_to_add_twin_1->succ = edge_to_add_twin_2;
			edge_to_add_twin_2->succ = edge_to_add_twin_3;
			edge_to_add_twin_3->succ = edge_to_add_twin_0;
			edge_to_add_twin_0->pred = edge_to_add_twin_3;
			edge_to_add_twin_1->pred = edge_to_add_twin_0;
			edge_to_add_twin_2->pred = edge_to_add_twin_1;
			edge_to_add_twin_3->pred = edge_to_add_twin_2;

			face_to_add->incident_edge = edge_to_add_0;

			return;
		}


		std::vector<newRecord> record_list;
		//step 1: find the face that is closest to the new site, use kd_tree better

		DECL_Voronoi::Site* closet_site;
		{
			double min_dis = _Infinity;
			for (auto& site_item : decl.site_list) {
				auto r = VectorLengthSqr(site_item.position, site);
				if (r < min_dis) {
					min_dis = r;
					closet_site = &site_item;
				}
			}
		}

		DECL_Voronoi::Site* current_site = closet_site;
		DECL_Voronoi::Face* closest_face = closet_site->incident_face;
		DECL_Voronoi::Face* face_to_add = new DECL_Voronoi::Face();

		DECL_Voronoi::HalfEdge* current_edge = closest_face->incident_edge;//该面起点边
		DECL_Voronoi::HalfEdge* closet_site_intersect_edge_0 = nullptr;

		debug_cout("find closet site: " + vector_to_string(closet_site->position));

		//Steo 0: add site
		decl.AddSite(site);
		DECL_Voronoi::Site* site_to_add = &(*(decl.site_list.end() - 1));

		bool first_both_border = false;
		int count = 0;
		do {
			debug_cout("intersection test with site: " + vector_to_string(current_site->position));
			//step 1: get bisector and {v, u} from closet site
			double slop;
			cv::Point2d bisector_l;
			cv::Point2d bisector_r;
			cv::Point2d mid = (current_site->position + site) / 2;
			auto line_eq_y = [](cv::Point2d mid, double x, double slop) {
				return slop * (x - mid.x) + mid.y;
				};
			auto line_eq_x = [](cv::Point2d mid, double y, double slop) {
				return (y - mid.y) / slop + mid.x;
				};
			if (VectorSlop(current_site->position, site, slop)) {
				if (slop == 0.0) {
					bisector_l = cv::Point2d(-decl.boundary.x * 0.001, mid.y);
					bisector_r = cv::Point2d(decl.boundary.x * 1.001, mid.y);
				}
				else {
					bisector_l = cv::Point2d(-decl.boundary.x * 0.001, line_eq_y(mid, -decl.boundary.x * 0.001, slop));
					bisector_r = cv::Point2d(decl.boundary.x * 1.001, line_eq_y(mid, decl.boundary.x * 1.001, slop));
					debug_cout("bisector_l: " + vector_to_string(bisector_l));
					debug_cout("bisector_r: " + vector_to_string(bisector_r));
				}
			}
			else {
				bisector_l = cv::Point2d(mid.x, -1);
				bisector_r = cv::Point2d(mid.x, decl.boundary.y + 1);
			}

			do {
				debug_cout("intersection test with edge: form*" + vector_to_string(current_edge->origin->position) + " to*" + vector_to_string(current_edge->end->position));
				std::vector<Line> all_lines;
				std::vector<IntersectionResult> intersectionResults;
				all_lines.push_back({ bisector_l , bisector_r });
				all_lines.push_back({ current_edge->origin->position , current_edge->end->position });
				Intersection_01(all_lines, intersectionResults);
				if (!intersectionResults.empty()) {
					if (!current_edge->isBorder && closet_site_intersect_edge_0 == nullptr) {
						closet_site_intersect_edge_0 = current_edge;
					}
					count++;
					record_list.push_back({ {intersectionResults[0].intersectionPoint}, current_edge });
					debug_cout("!!found intersection with position: " + vector_to_string(intersectionResults[0].intersectionPoint));
				}
				current_edge = current_edge->succ;

			} while (count != 2);

			debug_cout("intersection test end, go to next face");

			//Step 2: update next inersection site
			if (!record_list[record_list.size() - 1].half_edge->isBorder && !record_list[record_list.size() - 2].half_edge->isBorder) {
				//both inersection are not border
				debug_cout("both inersection are not border");
				count--;
				current_edge = current_edge->twin;
				current_edge = current_edge->succ;
				current_site = current_edge->incident_face->incident_site;
			}
			else if (record_list[record_list.size() - 1].half_edge->isBorder != record_list[record_list.size() - 2].half_edge->isBorder) {
				//one of them are border
				debug_cout("one of them are border");
				if (closet_site_intersect_edge_0 != nullptr) {
					count--;
					current_edge = closet_site_intersect_edge_0->twin;
					current_edge = current_edge->succ;
					current_site = current_edge->incident_face->incident_site;
					closet_site_intersect_edge_0 = nullptr;
				}
				else {
					goto after_all_intersection_record_find;
				}
			}
			else {
				//both inersection are border
				debug_cout("both inersection are border");
				if (!first_both_border) {
					first_both_border = true;
					auto site_to_add_dir = ToLeft(record_list[record_list.size() - 1].position, record_list[record_list.size() - 2].position, site_to_add->position);
					for (auto& site_item : decl.site_list) {
						if (&site_item == closet_site) {
							continue;
						}
						auto r_dir = ToLeft(record_list[record_list.size() - 1].position, record_list[record_list.size() - 2].position, site_item.position);
						if (site_to_add_dir == r_dir) {
							goto after_all_intersection_record_find;
						}

					}

					//find second closet site
					DECL_Voronoi::Site* second_closet_site;
					double min_dis = -INFINITY;
					for (auto& site_item : decl.site_list) {
						if (&site_item == closet_site) {
							continue;
						}
						auto r = VectorLengthSqr(site_item.position, site);
						if (r < min_dis) {
							min_dis = r;
							second_closet_site = &site_item;
						}
					}

					count = 0;
					current_edge = second_closet_site->incident_face->incident_edge;
					current_site = second_closet_site;

				}
				else {
					goto after_all_intersection_record_find;
				}

			}
		} while (current_site != closet_site);


	after_all_intersection_record_find:
		debug_cout("found all record");
		//update record, del/add and merge
		std::vector<DECL_Voronoi::HalfEdge*> edge_added_list;

		uint non_border_record_num = 0;
		uint border_record_num = 0;
		{
			for (int i = 0; i < record_list.size(); i++) {
				if (record_list[i].half_edge->isBorder) {
					border_record_num++;
				}
				else {
					non_border_record_num++;
				}
			}
		}

		if (non_border_record_num == record_list.size()) {
			//all are non-border intersection
			for (int i = 0; i < record_list.size(); i++) {
				auto& record = record_list[i];

				DECL_Voronoi::Vertex* vt_to_add = new DECL_Voronoi::Vertex(record.position);
				decl.AddVertex(vt_to_add);

				DECL_Voronoi::Vertex* vt_close_to_new_site =
					VectorLengthSqr(record.half_edge->origin->position, site) < VectorLengthSqr(record.half_edge->end->position, site) ?
					record.half_edge->origin : record.half_edge->end;

				//Step 1: split half edge and del the close newsite part
				DECL_Voronoi::HalfEdge* origin_half_edge = record.half_edge;

				if (vt_close_to_new_site == origin_half_edge->origin) {
					origin_half_edge->origin = vt_to_add;
					origin_half_edge->twin->end = vt_to_add;
				}
				else {
					origin_half_edge->end = vt_to_add;
					origin_half_edge->twin->origin = vt_to_add;
				}

				//Step 2: add new half edge
				{
					decl.DelVertex(vt_close_to_new_site);
					delete(vt_close_to_new_site);

					if (i > 0) {
						DECL_Voronoi::HalfEdge* edge_to_add = new DECL_Voronoi::HalfEdge(vt_to_add, *(decl.vertex_list.end() - 1));
						DECL_Voronoi::HalfEdge* edge_to_add_twin = new DECL_Voronoi::HalfEdge(*(decl.vertex_list.end() - 1), vt_to_add);
						edge_to_add->twin = edge_to_add_twin;
						edge_to_add_twin->twin = edge_to_add;
						edge_added_list.push_back(edge_to_add);
						edge_added_list.push_back(edge_to_add_twin);
					}

					if (i == record_list.size() - 1) {
						DECL_Voronoi::HalfEdge* edge_to_add_last = new DECL_Voronoi::HalfEdge(*(decl.vertex_list.end() - 1), vt_to_add);
						DECL_Voronoi::HalfEdge* edge_to_add_twin_last = new DECL_Voronoi::HalfEdge(vt_to_add, *(decl.vertex_list.end() - record_list.size()));
						edge_to_add_last->twin = edge_to_add_twin_last;
						edge_to_add_twin_last->twin = edge_to_add_twin_last;
						edge_to_add_last->incident_face = face_to_add;
						edge_to_add_twin_last->incident_face = face_to_add;
						edge_added_list.push_back(edge_to_add_last);
						edge_added_list.push_back(edge_to_add_twin_last);
					}
				}

			}
		}
		else if (border_record_num == record_list.size()) {
			//all are border intersection
			ASSERT(record_list.size() == 2 || record_list.size() == 4);
			for (int i = 0; i < record_list.size(); i++) {
				auto& record = record_list[i];

				DECL_Voronoi::Vertex* vt_to_add = new DECL_Voronoi::Vertex(record.position);
				decl.AddVertex(vt_to_add);

				DECL_Voronoi::Vertex* vt_close_to_new_site =
					VectorLengthSqr(record.half_edge->origin->position, site) < VectorLengthSqr(record.half_edge->end->position, site) ?
					record.half_edge->origin : record.half_edge->end;

				//Step 1: split half edge and del the close newsite part
				DECL_Voronoi::HalfEdge* origin_half_edge = record.half_edge;

				//Step 2: add new half edge
				{
					DECL_Voronoi::HalfEdge* edge_to_add_border;
					DECL_Voronoi::HalfEdge* edge_to_add_twin_border;

					edge_to_add_border = new DECL_Voronoi::HalfEdge(vt_to_add, vt_close_to_new_site);
					edge_to_add_twin_border = new DECL_Voronoi::HalfEdge(vt_close_to_new_site, vt_to_add);
					edge_to_add_border->isBorder = true;
					edge_to_add_twin_border->isBorder = true;

					edge_to_add_border->twin = edge_to_add_twin_border;
					edge_to_add_twin_border->twin = edge_to_add_border;
					edge_to_add_border->incident_face = face_to_add;
					edge_to_add_twin_border->incident_face = face_to_add;

					edge_added_list.push_back(edge_to_add_border);
					edge_added_list.push_back(edge_to_add_twin_border);
				}

				if (vt_close_to_new_site == origin_half_edge->origin) {
					origin_half_edge->origin = vt_to_add;
					origin_half_edge->twin->end = vt_to_add;
				}
				else {
					origin_half_edge->end = vt_to_add;
					origin_half_edge->twin->origin = vt_to_add;
				}

				if ((i + 1) % 2 == 0) {
					auto last_vertex = *(decl.vertex_list.end() - 2);

					DECL_Voronoi::HalfEdge* edge_to_add_last = create_pari_edge(last_vertex, vt_to_add);
					DECL_Voronoi::HalfEdge* edge_to_add_twin_last = edge_to_add_last->twin;

					edge_to_add_last->isBorder = false;
					edge_to_add_twin_last->isBorder = false;

					edge_to_add_last->incident_face = face_to_add;
					edge_to_add_twin_last->incident_face = closest_face;

					edge_added_list.push_back(edge_to_add_last);
					edge_added_list.push_back(edge_to_add_twin_last);

					auto border_e1 = edge_added_list[edge_added_list.size() - 6 - 6 * (i / 2)];
					auto border_e1_twin = edge_added_list[edge_added_list.size() - 5 - 6 * (i / 2)];
					auto origin_half_edge_e1 = record_list[i - 1].half_edge;

					auto border_e2 = edge_added_list[edge_added_list.size() - 4 - 6 * (i / 2)];
					auto border_e2_twin = edge_added_list[edge_added_list.size() - 3 - 6 * (i / 2)];
					auto origin_half_edge_e2 = origin_half_edge;

					bool ccw = ToLeft(last_vertex->position, vt_to_add->position, site_to_add->position);

					if (ccw)
					{
						//if intersection is ccw
						//inner
						border_e1->pred = origin_half_edge->pred;
						origin_half_edge_e1->pred->succ = border_e1;

						border_e1_twin->succ = origin_half_edge_e1->pred->twin;
						origin_half_edge_e1->pred->twin->pred = border_e1_twin;

						border_e2->succ = origin_half_edge_e2->succ;
						origin_half_edge_e2->succ->pred = border_e2;

						border_e2_twin->pred = origin_half_edge_e2->succ->twin;
						origin_half_edge_e2->succ->twin->succ = border_e2_twin;

						edge_to_add_last->succ = border_e2;
						border_e2->pred = edge_to_add_last;
						edge_to_add_last->pred = border_e1;
						border_e1->succ = edge_to_add_last;
						//outter
						origin_half_edge_e1->pred = edge_to_add_last->twin;
						edge_to_add_last->twin->succ = origin_half_edge_e1;

						origin_half_edge_e2->succ = edge_to_add_last->twin;
						edge_to_add_last->twin->pred = origin_half_edge_e2;
					}
					else {
						border_e2->pred = origin_half_edge->pred;
						origin_half_edge_e2->pred->succ = border_e2;

						border_e2_twin->succ = origin_half_edge_e2->pred->twin;
						origin_half_edge_e2->pred->twin->pred = border_e2_twin;

						border_e1->succ = origin_half_edge_e1->succ;
						origin_half_edge_e1->succ->pred = border_e1;

						border_e1_twin->pred = origin_half_edge_e1->succ->twin;
						origin_half_edge_e1->succ->twin->succ = border_e1_twin;

						//set edge pred and succ(inner face around new site)
						edge_to_add_last->succ = border_e1;
						border_e1->pred = edge_to_add_last;
						edge_to_add_last->pred = border_e2;
						border_e2->succ = edge_to_add_last;

						//outter
						origin_half_edge_e2->pred = edge_to_add_last->twin;
						edge_to_add_last->twin->succ = origin_half_edge_e2;

						origin_half_edge_e1->succ = edge_to_add_last->twin;
						edge_to_add_last->twin->pred = origin_half_edge_e1;
					}

					//update edge face
					while (border_e1 != border_e2) {
						if (ccw) {
							border_e1->pred->incident_face = face_to_add;
							border_e1 = border_e1->pred;
						}
						else {
							border_e1->succ->incident_face = face_to_add;
							border_e1 = border_e1->succ;
						}
					}

				}
			}


		}
		else {
			//non-border alne with border intersection

			uint first_border_intersection_index = -1;
			uint last_border_intersection_index = -1;
			{
				for (int i = 0; i < record_list.size(); i++) {
					auto& record = record_list[i];
					if (record.half_edge->isBorder) {
						if (first_border_intersection_index == -1)
							first_border_intersection_index = i;
						else
							last_border_intersection_index = i;
					}
				}
			}

			bool ccw;
			{
				auto start_point = record_list[last_border_intersection_index - 1].position;
				auto next_point = record_list[last_border_intersection_index].position;
				ccw = ToLeft(start_point, next_point, site_to_add->position);
			}

			//create edge, record possiblely like(e.g. b->e->e->-e>->b)(e.g. e->e->b->-e>->b)
			{
				for (int i = first_border_intersection_index; i >= 0; i--) {
					newRecord& record = record_list[i];
					DECL_Voronoi::Vertex* vt_to_add = new DECL_Voronoi::Vertex(record.position);
					DECL_Voronoi::Vertex* vt_close_to_new_site =
						VectorLengthSqr(record.half_edge->origin->position, site) < VectorLengthSqr(record.half_edge->end->position, site) ?
						record.half_edge->origin : record.half_edge->end;
					DECL_Voronoi::HalfEdge* origin_half_edge = record.half_edge;

					decl.AddVertex(vt_to_add);

					//create first border edge
					if (i == first_border_intersection_index)
					{
						DECL_Voronoi::HalfEdge* edge_to_add_border;
						DECL_Voronoi::HalfEdge* edge_to_add_twin_border;

						{
							edge_to_add_border = create_pari_edge(vt_to_add, vt_close_to_new_site);
							edge_to_add_twin_border = edge_to_add_border->twin;

							edge_to_add_border->isBorder = true;
							edge_to_add_twin_border->isBorder = true;

							edge_to_add_border->incident_face = face_to_add;
							edge_to_add_twin_border->incident_face = nullptr;

							edge_added_list.push_back(edge_to_add_border);
							edge_added_list.push_back(edge_to_add_twin_border);
						}

						//update pred and succ
						//first we have to find which edge we need as pred/succ
						newRecord record_succ;
						{
							if (first_border_intersection_index == 0)
							{
								record_succ = record_list[1];
							}
							else {
								record_succ = record_list[first_border_intersection_index - 1];
							}
						}

						DECL_Voronoi::HalfEdge** edge_to_add_border_attach_dir;
						{
							if (
								vt_to_add->position.y < vt_close_to_new_site->position.y ||
								vt_to_add->position.x < vt_close_to_new_site->position.x
								) {
								edge_to_add_border_attach_dir = &edge_to_add_border->pred;
							}
							else {
								edge_to_add_border_attach_dir = &edge_to_add_border->succ;
							}
						}

						//TODO:ERROR
						if (ccw) {
							if (true) {
								edge_to_add_border->succ = record.half_edge->succ->twin->succ;
								record.half_edge->succ->twin->succ->pred = edge_to_add_border;

								edge_to_add_twin_border->pred = record.half_edge->succ->twin->succ->twin;
								record.half_edge->succ->twin->succ->twin->succ = edge_to_add_twin_border;
							}
							else {
								edge_to_add_border->pred = record.half_edge->pred;
								record.half_edge->pred->succ = edge_to_add_border;

								edge_to_add_twin_border->succ = record.half_edge->pred->twin;
								record.half_edge->pred->twin->pred = edge_to_add_twin_border;
							}

						}
						else {
							if (true) {
								edge_to_add_border->pred = record.half_edge->pred->twin->pred;
								record.half_edge->pred->twin->pred->succ = edge_to_add_border;

								edge_to_add_twin_border->succ = record.half_edge->pred->twin->pred->twin;
								record.half_edge->pred->twin->pred->twin->pred = edge_to_add_twin_border;
							}
							else {
								edge_to_add_border->succ = record.half_edge->succ;
								record.half_edge->succ->pred = edge_to_add_border;

								edge_to_add_twin_border->pred = record.half_edge->succ->twin;
								record.half_edge->succ->twin->succ = edge_to_add_twin_border;
							}

						}

						//move origin border edge vertex
						if (vt_close_to_new_site == origin_half_edge->origin) {
							origin_half_edge->origin = vt_to_add;
							origin_half_edge->twin->end = vt_to_add;
						}
						else {
							origin_half_edge->end = vt_to_add;
							origin_half_edge->twin->origin = vt_to_add;
						}
					}

					//create non-border edge
					if (i > 0) {
						auto& record_succ = record_list[i - 1];
						DECL_Voronoi::Vertex* vt_to_add_succ = new DECL_Voronoi::Vertex(record_succ.position);
						decl.AddVertex(vt_to_add_succ);

						auto new_edge = create_pari_edge(vt_to_add, vt_to_add_succ);
						new_edge->incident_face = face_to_add;
						new_edge->twin->incident_face = record.half_edge->incident_face;
						edge_added_list.push_back(new_edge);
						edge_added_list.push_back(new_edge->twin);

						//move origin border edge vertex
						if (vt_close_to_new_site == origin_half_edge->origin) {
							origin_half_edge->origin = vt_to_add;
							origin_half_edge->twin->end = vt_to_add;
						}
						else {
							origin_half_edge->end = vt_to_add;
							origin_half_edge->twin->origin = vt_to_add;
						}
					}

				}

				for (int i = first_border_intersection_index + 1; i < record_list.size(); i++) {
					auto& record = record_list[i];

					DECL_Voronoi::Vertex* vt_to_add = new DECL_Voronoi::Vertex(record.position);
					decl.AddVertex(vt_to_add);

					DECL_Voronoi::Vertex* vt_close_to_new_site =
						VectorLengthSqr(record.half_edge->origin->position, site) < VectorLengthSqr(record.half_edge->end->position, site) ?
						record.half_edge->origin : record.half_edge->end;

					//Step 1: split half edge and del the close newsite part
					DECL_Voronoi::HalfEdge* origin_half_edge = record.half_edge;


					newRecord record_succ;
					if (i == first_border_intersection_index + 1)
						record_succ = record_list[0];
					else
						record_succ = record_list[i - 1];

					DECL_Voronoi::Vertex* vt_to_add_succ = new DECL_Voronoi::Vertex(record_succ.position);
					decl.AddVertex(vt_to_add_succ);
					DECL_Voronoi::HalfEdge* non_border_edge = create_pari_edge(vt_to_add, vt_to_add_succ);

					//create non-border edge
					{
						non_border_edge->incident_face = face_to_add;
						non_border_edge->twin->incident_face = record.half_edge->incident_face;
						edge_added_list.push_back(non_border_edge);
						edge_added_list.push_back(non_border_edge->twin);

						//move origin border edge vertex
						if (vt_close_to_new_site == origin_half_edge->origin) {
							origin_half_edge->origin = vt_to_add;
							origin_half_edge->twin->end = vt_to_add;
						}
						else {
							origin_half_edge->end = vt_to_add;
							origin_half_edge->twin->origin = vt_to_add;
						}
					}

					//create last border edge
					if (i == record_list.size() - 1) {
						{
							DECL_Voronoi::HalfEdge* edge_to_add_border;
							DECL_Voronoi::HalfEdge* edge_to_add_twin_border;

							{
								edge_to_add_border = create_pari_edge(vt_to_add, vt_close_to_new_site);
								edge_to_add_twin_border = edge_to_add_border->twin;

								edge_to_add_border->isBorder = true;
								edge_to_add_twin_border->isBorder = true;

								edge_to_add_border->incident_face = face_to_add;
								edge_to_add_twin_border->incident_face = face_to_add;

								edge_added_list.push_back(edge_to_add_border);
								edge_added_list.push_back(edge_to_add_twin_border);
							}

							//set pred and succ(inner)
							if (ccw) {
								bool tri_cuts = (
									Area2(vt_to_add->position, vt_to_add_succ->position, record.half_edge->succ->origin->position)
									*
									Area2(vt_to_add->position, vt_to_add_succ->position, record.half_edge->succ->end->position)
									) < 0;

								if (tri_cuts) {
									edge_to_add_border->succ = record.half_edge->succ->twin->succ;
									record.half_edge->succ->twin->succ->pred = edge_to_add_border;

									edge_to_add_twin_border->pred = record.half_edge->succ->twin->succ->twin;
									record.half_edge->succ->twin->succ->twin->succ = edge_to_add_twin_border;
								}
								else {
									edge_to_add_border->pred = record.half_edge->pred;
									record.half_edge->pred->succ = edge_to_add_border;

									edge_to_add_twin_border->succ = record.half_edge->pred->twin;
									record.half_edge->pred->twin->pred = edge_to_add_twin_border;
								}

							}
							else {
								//TODO:<=是不是可以优化，因为等于0实际上相交于端点
								bool tri_cuts = (
									Area2(vt_to_add->position, vt_to_add_succ->position, record.half_edge->pred->origin->position)
									*
									Area2(vt_to_add->position, vt_to_add_succ->position, record.half_edge->pred->end->position)
									) <= 0;


								if (tri_cuts) {
									edge_to_add_border->pred = record.half_edge->pred->twin->pred;
									record.half_edge->pred->twin->pred->succ = edge_to_add_border;

									edge_to_add_twin_border->succ = record.half_edge->pred->twin->pred->twin;
									record.half_edge->pred->twin->pred->twin->pred = edge_to_add_twin_border;
								}
								else {
									edge_to_add_border->succ = record.half_edge->succ;
									record.half_edge->succ->pred = edge_to_add_border;

									edge_to_add_twin_border->pred = record.half_edge->succ->twin;
									record.half_edge->succ->twin->succ = edge_to_add_twin_border;
								}
							}
						}
					}
					//update pred and succ(outter face)
					/*				if (ccw) {
										record.half_edge->succ = non_border_edge->twin;
										non_border_edge->twin->pred = record.half_edge;

										record_succ_same_face_edge->pred = non_border_edge->twin;
										non_border_edge->twin->succ = record_succ_same_face_edge;
									}
									else {
										record.half_edge->pred = non_border_edge->twin;
										non_border_edge->twin->succ = record.half_edge;

										record_succ_same_face_edge->succ = non_border_edge->twin;
										non_border_edge->twin->pred = record_succ_same_face_edge;
									}*/
				}
			}

			{
				//update pred and succ(inner)
				for (int i = 2; i < edge_added_list.size(); i += 2) {

					auto edge = edge_added_list[i];
					DECL_Voronoi::HalfEdge* edge_last = edge_added_list[i - 2];


					//inner
					if (ccw) {
						edge->pred = edge_last;
						edge->twin->succ = edge_last->twin;
						edge_last->succ = edge;
						edge_last->twin->pred = edge->twin;
					}
					else {
						edge->succ = edge_last;
						edge->twin->pred = edge_last->twin;
						edge_last->pred = edge;
						edge_last->twin->succ = edge->twin;
					}

				}

				//update pred and succ(outter)
				{
					for (int i = first_border_intersection_index; i > 0; i--) {

						auto& record = record_list[i];
						newRecord record_succ;
						record_succ = record_list[i - 1];
						DECL_Voronoi::HalfEdge* record_succ_same_face_edge = record_succ.half_edge;
						{
							if (record_succ.half_edge->incident_face != record.half_edge->incident_face) {
								record_succ_same_face_edge = record_succ.half_edge->twin;
							}
						}

						auto& non_border_edge = edge_added_list[(first_border_intersection_index - i) * 2];

						if (ccw) {
							record.half_edge->succ = non_border_edge->twin;
							non_border_edge->twin->pred = record.half_edge;

							record_succ_same_face_edge->pred = non_border_edge->twin;
							non_border_edge->twin->succ = record_succ_same_face_edge;
						}
						else {
							record.half_edge->pred = non_border_edge->twin;
							non_border_edge->twin->succ = record.half_edge;

							record_succ_same_face_edge->succ = non_border_edge->twin;
							non_border_edge->twin->pred = record_succ_same_face_edge;
						}
					}

					for (int i = first_border_intersection_index + 1; i < record_list.size(); i++) {
						auto& record = record_list[i];
						newRecord record_succ;
						if (i == first_border_intersection_index + 1)
							record_succ = record_list[0];
						else
							record_succ = record_list[i - 1];
						DECL_Voronoi::HalfEdge* record_succ_same_face_edge = record_succ.half_edge;
						{
							if (record_succ.half_edge->incident_face != record.half_edge->incident_face) {
								record_succ_same_face_edge = record_succ.half_edge->twin;
							}
						}

						auto& non_border_edge = edge_added_list[i * 2];

						if (ccw) {
							record.half_edge->succ = non_border_edge->twin;
							non_border_edge->twin->pred = record.half_edge;

							record_succ_same_face_edge->pred = non_border_edge->twin;
							non_border_edge->twin->succ = record_succ_same_face_edge;
						}
						else {
							record.half_edge->pred = non_border_edge->twin;
							non_border_edge->twin->succ = record.half_edge;

							record_succ_same_face_edge->succ = non_border_edge->twin;
							non_border_edge->twin->pred = record_succ_same_face_edge;
						}
					}
				}
			}
		}

		//step 3: set face
		face_to_add->incident_site = site_to_add;
		site_to_add->incident_face = face_to_add;
		face_to_add->incident_edge = edge_added_list[0];

		//step 4: set edge pred and succ(inner face around new site)

		if (non_border_record_num == record_list.size()) {
			//all non boeder
			for (int i = 0; i < edge_added_list.size(); i += 2) {
				auto& edge = edge_added_list[i];
				if (i == 0) {
					edge->pred = edge_added_list[edge_added_list.size() - 2];
					edge->succ = edge_added_list[2];
				}
				else if (i == (edge_added_list.size() - 2)) {
					edge->pred = edge_added_list[i - 2];
					edge->succ = edge_added_list[0];
				}
				else {
					edge->pred = edge_added_list[i - 2];
					edge->succ = edge_added_list[i + 2];
				}
				edge->incident_face = face_to_add;
			}

			//step 4: check if need del halfedge which is surround by new site's cell(outter face around new site)
			{
				int boader_acc = 0;
				for (int i = 0; i < (record_list.size() == 2 ? 1 : record_list.size()); i++) {
					auto& record = record_list[i];
					if (record.half_edge->isBorder) {
						boader_acc++;
					}
					DECL_Voronoi::HalfEdge* edge_to_add;
					if (i == 0) {
						edge_to_add = edge_added_list[edge_added_list.size() - 1];
					}
					else {
						edge_to_add = edge_added_list[i * 2 - 1 + boader_acc];
					}

					DECL_Voronoi::HalfEdge* current = record.half_edge->succ;

					auto is_edge_in_record = [&](DECL_Voronoi::HalfEdge* edge) {
						for (auto r : record_list) {
							if (r.half_edge == edge) {
								return true;
							}
						}
						return false;
						};
					while (!is_edge_in_record(current) && !is_edge_in_record(current->twin)) {
						//del edge
						DECL_Voronoi::HalfEdge* edge_to_del = current;
						current = current->succ;
						if (!record.half_edge->isBorder) {
							if (edge_to_del->incident_face->incident_edge == edge_to_del) {
								edge_to_del->incident_face->incident_edge = edge_to_del->succ;
							}
							delete(edge_to_del);
						}
					}

					record.half_edge->succ = edge_to_add;
					edge_to_add->succ = current;
					current->pred = edge_to_add;
					edge_to_add->pred = record.half_edge;
				}

			}
		}
		};

	for (const auto& site : all_point) {
		debug_cout("\n");
		debug_cout("*********start with site:" + vector_to_string(site));
		Incremental_construction(site, decl);
		debug_cout("*********end with site:" + vector_to_string(site));
	}


}