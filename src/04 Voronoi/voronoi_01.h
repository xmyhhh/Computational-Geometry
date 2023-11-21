#pragma once

#include "common/typedef.h"
#include "common/helper.h"



extern void Intersection_01(std::vector<Line>& all_lines, std::vector <IntersectionResult>& intersectionResults);

void Voronoi_01(std::vector<cv::Point>& all_point, DECL::DECL& decl) {
	//Incremental Construction

	struct newRecord {
		cv::Point2d position;
		DECL::HalfEdge* half_edge;
	};
	std::vector<newRecord> record_list;

	auto Incremental_construction = [&](const cv::Point2d site, DECL::DECL& decl) {
		if (decl.site_list.size() == 0) {
			decl.AddSite(site);
			DECL::Site* site_to_add = &(*(decl.site_list.end() - 1));

			DECL::Face* face_to_add = new DECL::Face();
			face_to_add->incident_site = site_to_add;
			site_to_add->incident_face = face_to_add;

			DECL::Vertex* v0 = new DECL::Vertex({ 0,0 });
			DECL::Vertex* v1 = new DECL::Vertex({ decl.boundary.x,0 });
			DECL::Vertex* v2 = new DECL::Vertex({ 0,decl.boundary.y });
			DECL::Vertex* v3 = new DECL::Vertex({ decl.boundary.x,decl.boundary.y });

			DECL::HalfEdge* edge_to_add_0 = new DECL::HalfEdge(v0, v1);
			DECL::HalfEdge* edge_to_add_twin_0 = new DECL::HalfEdge(v1, v0);
			edge_to_add_0->twin = edge_to_add_twin_0;
			edge_to_add_twin_0->twin = edge_to_add_0;
			edge_to_add_0->incident_face = face_to_add;
			edge_to_add_twin_0->incident_face = nullptr;
			edge_to_add_0->isBorder = true;
			edge_to_add_twin_0->isBorder = true;

			DECL::HalfEdge* edge_to_add_1 = new DECL::HalfEdge(v1, v3);
			DECL::HalfEdge* edge_to_add_twin_1 = new DECL::HalfEdge(v3, v1);
			edge_to_add_1->twin = edge_to_add_twin_1;
			edge_to_add_twin_1->twin = edge_to_add_1;
			edge_to_add_1->incident_face = face_to_add;
			edge_to_add_twin_1->incident_face = nullptr;
			edge_to_add_1->isBorder = true;
			edge_to_add_twin_1->isBorder = true;

			DECL::HalfEdge* edge_to_add_2 = new DECL::HalfEdge(v2, v3);
			DECL::HalfEdge* edge_to_add_twin_2 = new DECL::HalfEdge(v3, v2);
			edge_to_add_2->twin = edge_to_add_twin_2;
			edge_to_add_twin_2->twin = edge_to_add_2;
			edge_to_add_2->incident_face = face_to_add;
			edge_to_add_twin_2->incident_face = nullptr;
			edge_to_add_2->isBorder = true;
			edge_to_add_twin_2->isBorder = true;

			DECL::HalfEdge* edge_to_add_3 = new DECL::HalfEdge(v2, v0);
			DECL::HalfEdge* edge_to_add_twin_3 = new DECL::HalfEdge(v0, v2);
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



		//step 1: find the face that is closest to the new site, use kd_tree better

		DECL::Site* closet_site;
		{
			double min_dis = Infinity;
			for (auto& site_item : decl.site_list) {
				auto r = VectorLengthSqr(site_item.position, site);
				if (r < min_dis) {
					min_dis = r;
					closet_site = &site_item;
				}
			}
		}

		DECL::Site* current_site = closet_site;
		DECL::Face* closest_face = closet_site->incident_face;
		DECL::Face* face_to_add = new DECL::Face();

		//Steo 0: add site
		decl.AddSite(site);
		DECL::Site* site_to_add = &(*(decl.site_list.end() - 1));


		bool first_both_border = false;
		do {
			//step 1: get bisector and {v, u} from closet site
			double slop;
			cv::Point2d bisector_l;
			cv::Point2d bisector_r;
			cv::Point2d mid = (closest_face->incident_site->position + site) / 2;
			auto line_eq_y = [](cv::Point2d mid, double x, double slop) {
				return slop * (x - mid.x) + mid.y;
				};
			auto line_eq_x = [](cv::Point2d mid, double y, double slop) {
				return (y - mid.y) / slop + mid.x;
				};
			if (VectorSlop(closest_face->incident_site->position, site, slop)) {
				if (slop == 0.0) {
					bisector_l = cv::Point2d(-1, mid.y);
					bisector_r = cv::Point2d(decl.boundary.x + 1, mid.y);
				}
				else {
					bisector_l = cv::Point2d(-decl.boundary.x * 0.1, line_eq_y(mid, -decl.boundary.x * 0.1, slop));
					bisector_r = cv::Point2d(decl.boundary.x * 1.1, line_eq_y(mid, decl.boundary.x * 1.1, slop));
				}
			}
			else {
				bisector_l = cv::Point2d(mid.x, -1);
				bisector_r = cv::Point2d(mid.x, decl.boundary.y + 1);
			}

			DECL::HalfEdge* current_edge = closest_face->incident_edge;//该面起点边
			DECL::HalfEdge* closet_site_intersect_edge_0 = nullptr;
			int count = 0;
			do {
				std::vector<Line> all_lines;
				std::vector<IntersectionResult> intersectionResults;
				all_lines.push_back({ bisector_l , bisector_r });
				all_lines.push_back({ current_edge->origin->position , current_edge->end->position });
				Intersection_01(all_lines, intersectionResults);
				if (!intersectionResults.empty()) {
					if (closet_site_intersect_edge_0 == nullptr) {
						closet_site_intersect_edge_0 = current_edge;
					}
					count++;
					record_list.push_back({ {intersectionResults[0].intersectionPoint}, current_edge });
				}
				current_edge = current_edge->succ;
			} while (count != 2);

			//update next inersection site
			if (!record_list[record_list.size() - 1].half_edge->isBorder && !record_list[record_list.size() - 2].half_edge->isBorder) {
				//both inersection are not border
				count--;
				current_edge = current_edge->twin;
				current_edge = current_edge->succ;
				current_site = current_edge->incident_face->incident_site;
			}
			else if (record_list[record_list.size() - 1].half_edge->isBorder != record_list[record_list.size() - 2].half_edge->isBorder) {
				//one of them are border
				if (closet_site_intersect_edge_0 != nullptr) {
					count--;
					current_edge = closet_site_intersect_edge_0->twin;
					current_edge = current_edge->succ;
					current_site = current_edge->incident_face->incident_site;
					closet_site_intersect_edge_0 = nullptr;
				}
				else {
					current_site = closet_site;
				}
			}
			else {
				//both inersection are border
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
					DECL::Site* second_closet_site;
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
		//update record, del/add and merge
		std::vector<DECL::HalfEdge*> edge_added_list;

		uint non_border_record_num = 0;
		uint border_record_num = 0;

		for (int i = 0; i < record_list.size(); i++) {
			if (record_list[i].half_edge->isBorder) {
				border_record_num++;
			}
			else {
				non_border_record_num++;
			}
		}


		if (non_border_record_num == record_list.size()) {
			//all are non-border intersection
			for (int i = 0; i < record_list.size(); i++) {
				auto& record = record_list[i];

				DECL::Vertex* vt_to_add = new DECL::Vertex(record.position);
				decl.AddVertex(vt_to_add);

				DECL::Vertex* vt_close_to_new_site =
					VectorLengthSqr(record.half_edge->origin->position, site) < VectorLengthSqr(record.half_edge->end->position, site) ?
					record.half_edge->origin : record.half_edge->end;

				//Step 1: split half edge and del the close newsite part
				DECL::HalfEdge* origin_half_edge = record.half_edge;

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
						DECL::HalfEdge* edge_to_add = new DECL::HalfEdge(vt_to_add, *(decl.vertex_list.end() - 1));
						DECL::HalfEdge* edge_to_add_twin = new DECL::HalfEdge(*(decl.vertex_list.end() - 1), vt_to_add);
						edge_to_add->twin = edge_to_add_twin;
						edge_to_add_twin->twin = edge_to_add;
						edge_added_list.push_back(edge_to_add);
						edge_added_list.push_back(edge_to_add_twin);
					}

					if (i == record_list.size() - 1) {
						DECL::HalfEdge* edge_to_add_last = new DECL::HalfEdge(*(decl.vertex_list.end() - 1), vt_to_add);
						DECL::HalfEdge* edge_to_add_twin_last = new DECL::HalfEdge(vt_to_add, *(decl.vertex_list.end() - record_list.size()));
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

				DECL::Vertex* vt_to_add = new DECL::Vertex(record.position);
				decl.AddVertex(vt_to_add);

				DECL::Vertex* vt_close_to_new_site =
					VectorLengthSqr(record.half_edge->origin->position, site) < VectorLengthSqr(record.half_edge->end->position, site) ?
					record.half_edge->origin : record.half_edge->end;

				//Step 1: split half edge and del the close newsite part
				DECL::HalfEdge* origin_half_edge = record.half_edge;

				//Step 2: add new half edge
				{
					DECL::HalfEdge* edge_to_add_border;
					DECL::HalfEdge* edge_to_add_twin_border;

					edge_to_add_border = new DECL::HalfEdge(vt_to_add, vt_close_to_new_site);
					edge_to_add_twin_border = new DECL::HalfEdge(vt_close_to_new_site, vt_to_add);
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

					DECL::HalfEdge* edge_to_add_last = new DECL::HalfEdge(last_vertex, vt_to_add);
					DECL::HalfEdge* edge_to_add_twin_last = new DECL::HalfEdge(vt_to_add, last_vertex);

					edge_to_add_last->twin = edge_to_add_twin_last;
					edge_to_add_twin_last->twin = edge_to_add_twin_last;
					edge_to_add_last->incident_face = face_to_add;
					edge_to_add_twin_last->incident_face = face_to_add;
					edge_added_list.push_back(edge_to_add_last);
					edge_added_list.push_back(edge_to_add_twin_last);

					auto border_e1 = edge_added_list[edge_added_list.size() - 6 - 6 * (i / 2)];
					auto border_e1_twin = edge_added_list[edge_added_list.size() - 5 - 6 * (i / 2)];
					auto origin_half_edge_e1 = record_list[i - 1].half_edge;

					auto border_e2 = edge_added_list[edge_added_list.size() - 4 - 6 * (i / 2)];
					auto border_e2_twin = edge_added_list[edge_added_list.size() - 3 - 6 * (i / 2)];
					auto origin_half_edge_e2 = origin_half_edge;

					if (ToLeft(last_vertex->position, vt_to_add->position, site_to_add->position))
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
				}
			}

			//set edge pred and succ(inner face around new site)

		}
		else {
			//TODO
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
					DECL::HalfEdge* edge_to_add;
					if (i == 0) {
						edge_to_add = edge_added_list[edge_added_list.size() - 1];
					}
					else {
						edge_to_add = edge_added_list[i * 2 - 1 + boader_acc];
					}

					DECL::HalfEdge* current = record.half_edge->succ;

					auto is_edge_in_record = [&](DECL::HalfEdge* edge) {
						for (auto r : record_list) {
							if (r.half_edge == edge) {
								return true;
							}
						}
						return false;
						};
					while (!is_edge_in_record(current) && !is_edge_in_record(current->twin)) {
						//del edge
						DECL::HalfEdge* edge_to_del = current;
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
		Incremental_construction(site, decl);
	}


}