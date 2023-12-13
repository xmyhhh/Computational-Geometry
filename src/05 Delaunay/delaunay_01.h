#pragma once

#include "common/typedef.h"
#include "common/helper.h"


namespace delaunay_01_datastruct {
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
		Face() {
			incident_edge = nullptr;
		}
	};

	class DECL {
	public:
		std::vector<Vertex*> vertex_list;
		std::vector<Face*> face_list;
		std::vector<HalfEdge*> edge_list;

		cv::Point2d boundary;

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

		void AddFace(Face* v) {
			face_list.push_back(v);
		}
		void DelFace(Face* v) {
			for (auto iter = face_list.begin(); iter != face_list.end(); iter++) {
				if (*iter == v) {
					face_list.erase(iter);
					break;
				}
			}
		}

		void AddEdge(HalfEdge* v) {
			edge_list.push_back(v);
		}
		void DelEdge(HalfEdge* v) {
			for (auto iter = edge_list.begin(); iter != edge_list.end(); iter++) {
				if (*iter == v) {
					edge_list.erase(iter);
					break;
				}
			}
		}

		DECL() {
			vertex_list.reserve(300);
			face_list.reserve(300);
			edge_list.reserve(300);
		}
	};
}

void Delaunay_01(std::vector<cv::Point>& all_point, delaunay_01_datastruct::DECL& decl) {
	//Delaunay Triangulation
	//Incremental insert + flip Algorithm

	using namespace delaunay_01_datastruct;
	auto create_pair_edge = [](Vertex* f, Vertex* t) {
		HalfEdge* edge;
		HalfEdge* edge_twin;

		edge = new  HalfEdge(f, t);
		edge_twin = new  HalfEdge(t, f);

		edge->twin = edge_twin;
		edge_twin->twin = edge;

		return edge;
		};

	auto Incremental_insertion = [&](const cv::Point2d vertex_position, DECL& decl) {
		Vertex* vertex_to_add = new  Vertex(vertex_position);
		decl.AddVertex(vertex_to_add);

		if (decl.vertex_list.size() < 3) {
			return;
		}

		if (decl.vertex_list.size() == 3) {
			Face* face = new  Face();
			decl.AddFace(face);
			HalfEdge* e01;
			HalfEdge* e12;
			HalfEdge* e20;
			bool ccw = ToLeft(decl.vertex_list[0]->position, decl.vertex_list[1]->position, decl.vertex_list[2]->position);

			if (ccw) {
				e01 = create_pair_edge(decl.vertex_list[0], decl.vertex_list[1]);
				e12 = create_pair_edge(decl.vertex_list[1], decl.vertex_list[2]);
				e20 = create_pair_edge(decl.vertex_list[2], decl.vertex_list[0]);
			}
			else {
				e01 = create_pair_edge(decl.vertex_list[1], decl.vertex_list[0]);
				e12 = create_pair_edge(decl.vertex_list[2], decl.vertex_list[1]);
				e20 = create_pair_edge(decl.vertex_list[0], decl.vertex_list[2]);
			}

			{
				face->incident_edge = e01;
				decl.AddEdge(e01);
				decl.AddEdge(e01->twin);
				decl.AddEdge(e12);
				decl.AddEdge(e12->twin);
				decl.AddEdge(e20);
				decl.AddEdge(e20->twin);

				if (ccw) {
					e01->succ = e12;
					e12->pred = e01;
					e01->twin->succ = e12->twin;
					e12->twin->pred = e01->twin;

					e01->pred = e20;
					e20->succ = e01;
					e01->twin->succ = e20->twin;
					e20->twin->pred = e01->twin;

					e12->succ = e20;
					e20->pred = e12;
					e12->twin->pred = e20->twin;
					e20->twin->succ = e12->twin;

				}
				else {
					//TODO:add code here
					ASSERT(false);
				}
				e01->incident_face = face;
				e12->incident_face = face;
				e20->incident_face = face;
			}
			return;
		}

		//Step 1: find out which face the new vtx in and slip it(minewhile set possible_bad_edge_list)
		Face* inside_face = nullptr;
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

		std::vector< HalfEdge*> possible_bad_edge_list;
		ASSERT(inside_face);

		//link split one tri to three tri
		auto p_edge0 = inside_face->incident_edge;
		auto p_edge1 = inside_face->incident_edge->succ;
		auto p_edge2 = inside_face->incident_edge->succ->succ;

		//add edge 0
		HalfEdge* new_edge0 = create_pair_edge(vertex_to_add, p_edge0->origin);
		decl.AddEdge(new_edge0);
		decl.AddEdge(new_edge0->twin);

		//add edge 1
		HalfEdge* new_edge1 = create_pair_edge(p_edge0->end, vertex_to_add);
		decl.AddEdge(new_edge1);
		decl.AddEdge(new_edge1->twin);

		//form face 0
		Face* p_face0 = new  Face();
		decl.AddFace(p_face0);
		p_face0->incident_edge = p_edge0;
		p_edge0->incident_face = p_face0;
		new_edge0->incident_face = p_face0;
		new_edge1->incident_face = p_face0;

		{
			p_edge0->succ = new_edge1;
			new_edge1->pred = p_edge0;

			p_edge0->pred = new_edge0;
			new_edge0->succ = p_edge0;

			new_edge1->succ = new_edge0;
			new_edge0->pred = new_edge1;
		}
		auto diffrent_v = p_edge1->end;
		//add edge 2
		HalfEdge* new_edge2 = create_pair_edge(vertex_to_add, diffrent_v);
		decl.AddEdge(new_edge2);
		decl.AddEdge(new_edge2->twin);

		//form face 1
		Face* p_face1 = new  Face();
		decl.AddFace(p_face1);
		p_face1->incident_edge = p_edge1;
		p_edge1->incident_face = p_face1;
		new_edge2->twin->incident_face = p_face1;
		new_edge1->twin->incident_face = p_face1;
		{
			p_edge1->succ = new_edge2->twin;
			new_edge2->twin->pred = p_edge1;

			p_edge1->pred = new_edge1->twin;
			new_edge1->twin->succ = p_edge1;

			new_edge2->twin->succ = new_edge1->twin;
			new_edge1->twin->pred = new_edge2->twin;
		}

		//form face 2
		Face* p_face2 = new  Face();
		decl.AddFace(p_face2);
		p_face2->incident_edge = p_edge2;
		p_edge2->incident_face = p_face2;
		new_edge2->incident_face = p_face2;
		new_edge0->twin->incident_face = p_face2;
		{
			p_edge2->succ = new_edge0->twin;
			new_edge0->twin->pred = p_edge2;

			p_edge2->pred = new_edge2;
			new_edge2->succ = p_edge2;

			new_edge0->twin->succ = new_edge2;
			new_edge2->pred = new_edge0->twin;
		}
		decl.DelFace(inside_face);
		possible_bad_edge_list.push_back(p_edge0);
		possible_bad_edge_list.push_back(p_edge1);
		possible_bad_edge_list.push_back(p_edge2);

		//Step 2: process possible_bad_edge_list
		while (possible_bad_edge_list.size()) {
			HalfEdge* possible_bad_edge = possible_bad_edge_list[possible_bad_edge_list.size() - 1];
			possible_bad_edge_list.pop_back();
			if (possible_bad_edge->twin->incident_face != nullptr) {
				auto test_vertex = possible_bad_edge->twin->succ->end;
				auto con_test_vertex = possible_bad_edge->succ->end;
				ASSERT(test_vertex != con_test_vertex);
				bool is_in_circle = InCircle2D(possible_bad_edge->origin->position, possible_bad_edge->end->position, possible_bad_edge->pred->origin->position, test_vertex->position);
				if (is_in_circle) {
					//flip
					auto origin_face_a = possible_bad_edge->incident_face;
					auto origin_face_a_edge_0 = possible_bad_edge->succ;
					auto origin_face_a_edge_1 = possible_bad_edge->pred;

					auto origin_face_b = possible_bad_edge->twin->incident_face;
					auto origin_face_b_edge_0 = possible_bad_edge->twin->pred;
					auto origin_face_b_edge_1 = possible_bad_edge->twin->succ;

					possible_bad_edge_list.push_back(origin_face_b_edge_0);
					possible_bad_edge_list.push_back(origin_face_b_edge_1);

					possible_bad_edge->origin = con_test_vertex;
					possible_bad_edge->end = test_vertex;
					possible_bad_edge->twin->origin = test_vertex;
					possible_bad_edge->twin->end = con_test_vertex;

					origin_face_a->incident_edge = possible_bad_edge;
					origin_face_b->incident_edge = possible_bad_edge->twin;

					origin_face_a_edge_0->incident_face = origin_face_a;
					origin_face_b_edge_0->incident_face = origin_face_a;

					origin_face_a_edge_1->incident_face = origin_face_b;
					origin_face_b_edge_1->incident_face = origin_face_b;

					possible_bad_edge->succ = origin_face_b_edge_0;
					origin_face_b_edge_0->pred = possible_bad_edge;
					possible_bad_edge->pred = origin_face_a_edge_0;
					origin_face_a_edge_0->succ = possible_bad_edge;
					origin_face_b_edge_0->succ = origin_face_a_edge_0;
					origin_face_a_edge_0->pred = origin_face_b_edge_0;

					possible_bad_edge->twin->succ = origin_face_a_edge_1;
					origin_face_a_edge_1->pred = possible_bad_edge->twin;
					possible_bad_edge->twin->pred = origin_face_b_edge_1;
					origin_face_b_edge_1->succ = possible_bad_edge->twin;
					origin_face_b_edge_1->pred = origin_face_a_edge_1;
					origin_face_a_edge_1->succ = origin_face_b_edge_1;
				}
			}
		}
		};



	Incremental_insertion(cv::Point2d(0, 0), decl);
	Incremental_insertion(cv::Point2d(decl.boundary.x * 2, 0), decl);
	Incremental_insertion(cv::Point2d(0, decl.boundary.y * 2), decl);

	for (const auto& point : all_point) {
		debug_cout("\n");
		debug_cout("*********start with site:" + vector_to_string(point));
		Incremental_insertion(point, decl);
		debug_cout("*********end with site:" + vector_to_string(point));
		debug_cout("\n");
	}



}