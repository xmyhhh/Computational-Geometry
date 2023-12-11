#pragma once

#include "common/typedef.h"
#include "common/helper.h"




void Non_Delaunay_01(std::vector<cv::Point>& all_point, DECL_Delaunay::DECL& decl) {
	//Dlaunay Triangulation
	//Bowyer-Watson Algorithm

	auto create_pari_edge = [](DECL_Delaunay::Vertex* f, DECL_Delaunay::Vertex* t) {
		DECL_Delaunay::HalfEdge* edge;
		DECL_Delaunay::HalfEdge* edge_twin;

		edge = new DECL_Delaunay::HalfEdge(f, t);
		edge_twin = new DECL_Delaunay::HalfEdge(t, f);

		edge->twin = edge_twin;
		edge_twin->twin = edge;

		return edge;
		};

	auto Incremental_construction = [&](const cv::Point2d vertex_position, DECL_Delaunay::DECL& decl) {
		DECL_Delaunay::Vertex* vertex_to_add = new DECL_Delaunay::Vertex(vertex_position);
		decl.AddVertex(vertex_to_add);

		if (decl.vertex_list.size() < 3) {
			return;
		}

		if (decl.vertex_list.size() == 3) {
			DECL_Delaunay::Face* face = new DECL_Delaunay::Face();
			decl.AddFace(face);
			DECL_Delaunay::HalfEdge* e01;
			DECL_Delaunay::HalfEdge* e12;
			DECL_Delaunay::HalfEdge* e20;
			bool ccw = ToLeft(decl.vertex_list[0]->position, decl.vertex_list[1]->position, decl.vertex_list[2]->position);

			if (ccw) {
				e01 = create_pari_edge(decl.vertex_list[0], decl.vertex_list[1]);
				e12 = create_pari_edge(decl.vertex_list[1], decl.vertex_list[2]);
				e20 = create_pari_edge(decl.vertex_list[2], decl.vertex_list[0]);
			}
			else {
				e01 = create_pari_edge(decl.vertex_list[1], decl.vertex_list[0]);
				e12 = create_pari_edge(decl.vertex_list[2], decl.vertex_list[1]);
				e20 = create_pari_edge(decl.vertex_list[0], decl.vertex_list[2]);
			}

			{
				face->incident_edge = e01;
				decl.AddEdge(e01);
				decl.AddEdge(e01->twin);
				decl.AddEdge(e12);
				decl.AddEdge(e12->twin);
				decl.AddEdge(e20);
				decl.AddEdge(e20->twin);

				bool ccw = ToLeft(decl.vertex_list[1]->position, decl.vertex_list[0]->position, decl.vertex_list[2]->position);
				if (ccw) {
					e01->pred = e12;
					e12->succ = e01;
					e01->twin->succ = e12->twin;
					e12->twin->pred = e01->twin;

					e01->succ = e20;
					e20->pred = e01;
					e01->twin->pred = e20->twin;
					e20->twin->succ = e01->twin;

					e12->pred = e20;
					e20->succ = e12;
					e12->twin->succ = e20->twin;
					e20->twin->pred = e12->twin;

				}
				else {
					e01->pred = e20;
					e20->succ = e01;
					e01->twin->succ = e20->twin;
					e20->twin->pred = e01->twin;

					e01->succ = e12;
					e12->pred = e01;
					e01->twin->pred = e12->twin;
					e12->twin->succ = e01->twin;

					e20->pred = e12;
					e12->succ = e20;
					e20->twin->succ = e12->twin;
					e12->twin->pred = e20->twin;
				}
				e01->incident_face = face;
				e12->incident_face = face;
				e20->incident_face = face;
			}
			return;
		}

		//find out which face the new vtx in
		DECL_Delaunay::Face* inside_face = nullptr;
		{
			for (auto face : decl.face_list) {
				auto& p1 = face->incident_edge->origin;
				auto& p2 = face->incident_edge->succ->origin;
				auto& p3 = face->incident_edge->succ->succ->origin;

				ASSERT(p1 != p2);
				ASSERT(p1 != p3);
				ASSERT(p3 != p2);
				if (InTriangleTest(p1->position, p2->position, p3->position, vertex_position)) {
					inside_face = face;
					break;
				}
			}
		}

		if (inside_face) {
			debug_cout("inside_face");
			//link split one tri to three tri
			auto p_edge0 = inside_face->incident_edge;
			auto p_edge1 = inside_face->incident_edge->succ;
			auto p_edge2 = inside_face->incident_edge->succ->succ;

			//add edge 0
			DECL_Delaunay::HalfEdge* new_edge0 = create_pari_edge(vertex_to_add, p_edge0->origin);
			decl.AddEdge(new_edge0);
			decl.AddEdge(new_edge0->twin);

			//add edge 1
			DECL_Delaunay::HalfEdge* new_edge1 = create_pari_edge(p_edge0->end, vertex_to_add);
			decl.AddEdge(new_edge1);
			decl.AddEdge(new_edge1->twin);

			//form face 0
			DECL_Delaunay::Face* p_face0 = new DECL_Delaunay::Face();
			decl.AddFace(p_face0);
			p_face0->incident_edge = p_edge0;

			{
				bool ccw = ToLeft(p_edge0->origin->position, p_edge0->end->position, vertex_position);
				if (ccw) {
					p_edge0->succ = new_edge1;
					new_edge1->pred = p_edge0;

					p_edge0->pred = new_edge0;
					new_edge0->succ = p_edge0;

					new_edge1->succ = new_edge0;
					new_edge0->pred = new_edge1;

				}
				else {
					p_edge0->pred = new_edge1;
					new_edge1->succ = p_edge0;

					p_edge0->succ = new_edge0;
					new_edge0->pred = p_edge0;

					new_edge1->pred = new_edge0;
					new_edge0->succ = new_edge1;
				}
			}

			auto diffrent_v = p_edge1->end;
			auto same_v = p_edge1->origin;
			{
				if (p_edge0->origin == p_edge1->end || p_edge0->end == p_edge1->end) {
					diffrent_v = p_edge1->origin;
				}
				if (diffrent_v == p_edge1->origin) {
					same_v = p_edge1->end;
				}
			}

			//add edge 2
			DECL_Delaunay::HalfEdge* new_edge2 = create_pari_edge(diffrent_v, vertex_to_add);
			decl.AddEdge(new_edge2);
			decl.AddEdge(new_edge2->twin);

			//form face 1
			DECL_Delaunay::Face* p_face1 = new DECL_Delaunay::Face();
			decl.AddFace(p_face1);
			p_face1->incident_edge = p_edge1;

			{
				bool ccw = ToLeft(p_edge1->origin->position, p_edge1->end->position, vertex_position);
				if (ccw) {
					//TODO??
					p_edge1->succ = new_edge2;
					new_edge2->pred = p_edge1;

					p_edge1->pred = new_edge1->twin;
					new_edge1->twin->succ = p_edge1;

					new_edge2->succ = new_edge1->twin;
					new_edge1->twin->pred = new_edge2;

				}
				else {
					p_edge1->pred = new_edge2;
					new_edge2->succ = p_edge1;

					p_edge1->succ = new_edge1->twin;
					new_edge1->twin->pred = p_edge1;

					new_edge2->pred = new_edge1->twin;
					new_edge1->twin->succ = new_edge2;
				}
			}


			//form face 2
			DECL_Delaunay::Face* p_face2 = new DECL_Delaunay::Face();
			decl.AddFace(p_face2);
			p_face2->incident_edge = p_edge2;

			{
				bool ccw = ToLeft(p_edge2->origin->position, p_edge2->end->position, vertex_position);
				if (ccw) {
					p_edge2->succ = new_edge0->twin;
					new_edge0->twin->pred = p_edge2;

					p_edge2->pred = new_edge2->twin;
					new_edge2->twin->succ = p_edge2;

					new_edge0->twin->succ = new_edge2->twin;
					new_edge2->twin->pred = new_edge0->twin;

				}
				else {
					p_edge2->pred = new_edge0->twin;
					new_edge0->twin->succ = p_edge2;

					p_edge2->succ = new_edge2->twin;
					new_edge2->twin->pred = p_edge2;

					new_edge0->twin->pred = new_edge2->twin;
					new_edge2->twin->succ = new_edge0->twin;
				}

			}

			decl.DelFace(inside_face);
		}
		else {
			//find the closet edge
			DECL_Delaunay::HalfEdge* minEdge;
			{
				double distance = _Infinity;
				for (auto& edge : decl.edge_list) {
					double dis = DistanceToPoint({ edge->origin->position, edge->end->position }, vertex_position);
					if (distance > dis) {
						distance = dis;
						minEdge = edge->twin;
					}
				}
			}

			//add edge 0
			DECL_Delaunay::HalfEdge* new_edge0 = create_pari_edge(vertex_to_add, minEdge->origin);
			decl.AddEdge(new_edge0);
			decl.AddEdge(new_edge0->twin);

			//add edge 1
			DECL_Delaunay::HalfEdge* new_edge1 = create_pari_edge(minEdge->end, vertex_to_add);
			decl.AddEdge(new_edge1);
			decl.AddEdge(new_edge1->twin);

			//form face 0
			DECL_Delaunay::Face* p_face = new DECL_Delaunay::Face();
			decl.AddFace(p_face);
			p_face->incident_edge = minEdge;
			{
				bool ccw = ToLeft(minEdge->origin->position, minEdge->end->position, vertex_position);
				if (ccw) {
					minEdge->succ = new_edge1;
					new_edge1->pred = minEdge;

					minEdge->pred = new_edge0;
					new_edge0->succ = minEdge;

					new_edge0->pred = new_edge1;
					new_edge1->succ = new_edge0;

				}
				else {
					minEdge->pred = new_edge1;
					new_edge1->succ = minEdge;

					minEdge->succ = new_edge0;
					new_edge0->pred = minEdge;

					new_edge0->succ = new_edge1;
					new_edge1->pred = new_edge0;
				}
			}

		}

		};


	Incremental_construction(cv::Point2d(decl.boundary.x, decl.boundary.y), decl);
	Incremental_construction(cv::Point2d(0, 0), decl);
	Incremental_construction(cv::Point2d(decl.boundary.x, 0), decl);
	Incremental_construction(cv::Point2d(0, decl.boundary.y), decl);

	for (const auto& point : all_point) {
		debug_cout("\n");
		debug_cout("*********start with site:" + vector_to_string(point));
		Incremental_construction(point, decl);
		debug_cout("*********end with site:" + vector_to_string(point));
		debug_cout("\n");
	}



}



