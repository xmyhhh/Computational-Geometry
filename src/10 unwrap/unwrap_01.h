#pragma once
#include "common/typedef.h"
#include "common/helper.h"
#include "common/file/format_convert.h"
#include "common/memory.h"

namespace unwrap_01_datastruct {
	struct Edge;
	struct Tetrahedra;
	struct Vertex {
		cv::Point3d position;
		std::vector<Edge*>* connect_edge_array;
		uint static_index;

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
		Tetrahedra* disjoin_tet[2];//0 is ccw , 1 is cw

		//for Gift_Wrapping
		bool face_from_plc = false;
		bool ccw_finished;
		bool cw_finished;
	};

	struct Edge {
		Vertex* orig;
		Vertex* end;

		//for draw debug
		bool draw_red = false;
	};

	struct Tetrahedra {
		Vertex* p1;
		Vertex* p2;
		Vertex* p3;
		Vertex* p4;

		Tetrahedra* neighbors[4];
		Face* faces[4];
		bool mark;

		int depth;
		//for draw
		bool draw_red = false;
	};

	struct Unwrap
	{
		MemoryPool vertex_pool;
		MemoryPool edge_pool;
		MemoryPool tetrahedra_pool;

		int size = 40;

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
			int ntetrahedras = 0, itetrahedras = 0, itetrahedrasattr = 0;
			bool readattr = false;

			while ((bufferp = read_line(buffer, fp, &line_count)) != NULL)
			{
				if (nverts == 0)
				{
					read_line(buffer, fp, &line_count); //Unstructured Grid
					read_line(buffer, fp, &line_count); //ASCII
					read_line(buffer, fp, &line_count); //DATASET UNSTRUCTURED_GRID
					read_line(buffer, fp, &line_count); //POINTS xxxx double
					sscanf(bufferp, "%*s %d %*s", &nverts);
					if (nverts < 3)
					{
						//printf("Syntax error reading header on line %d in file %s\n",
						//	line_count, vtk_file_path);
						fclose(fp);
						return false;
					}
					vertex_pool.initializePool(sizeof(Vertex), nverts, 8, 32);
				}
				else if (nverts > iverts)
				{
					auto v = (Vertex*)vertex_pool.allocate();

					v->position.x = (double)strtod(bufferp, &bufferp) / 5;
					v->position.y = (double)strtod(bufferp, &bufferp) / 5;
					v->position.z = (double)strtod(bufferp, &bufferp) / 5;
					v->static_index = iverts;
					iverts++;
				}
				else if (ntetrahedras == 0)
				{
					//CELLS 35186 175930

					sscanf(bufferp, "%*s %d %*d", &ntetrahedras);
					tetrahedra_pool.initializePool(sizeof(Tetrahedra), ntetrahedras, 8, 32);

				}
				else if (ntetrahedras > itetrahedras)
				{
					static int p0, p1, p2, p3;
					sscanf(bufferp, "%*d %d %d %d %d",
						&p0,
						&p1,
						&p2,
						&p3
					);

					auto t = (Tetrahedra*)tetrahedra_pool.allocate();
					t->p1 = (Vertex*)vertex_pool[p0];
					t->p2 = (Vertex*)vertex_pool[p1];
					t->p3 = (Vertex*)vertex_pool[p2];
					t->p4 = (Vertex*)vertex_pool[p3];
					itetrahedras++;
				}

				else {
					break;
				}
			}
			fclose(fp);

			/*	for (auto vtx : vertex_array) {
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
				}*/

			return true;
		}

		VulkanDrawData toVulkanDrawData() {
			debug_cout("prepare vulkan data");
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
			vulkan_data.numberOfTriangle += tetrahedra_pool.size() * 4;

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

				vulkan_data.triangles[i * 3 * 4] = t->p1->static_index;
				vulkan_data.triangles[i * 3 * 4 + 1] = t->p2->static_index;
				vulkan_data.triangles[i * 3 * 4 + 2] = t->p3->static_index;

				vulkan_data.triangles[i * 3 * 4 + 3] = t->p1->static_index;
				vulkan_data.triangles[i * 3 * 4 + 4] = t->p2->static_index;
				vulkan_data.triangles[i * 3 * 4 + 5] = t->p4->static_index;

				vulkan_data.triangles[i * 3 * 4 + 6] = t->p1->static_index;
				vulkan_data.triangles[i * 3 * 4 + 7] = t->p3->static_index;
				vulkan_data.triangles[i * 3 * 4 + 8] = t->p4->static_index;

				vulkan_data.triangles[i * 3 * 4 + 9] = t->p2->static_index;
				vulkan_data.triangles[i * 3 * 4 + 10] = t->p3->static_index;
				vulkan_data.triangles[i * 3 * 4 + 11] = t->p4->static_index;

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
			return vulkan_data;
		};


	};
}
void Unwrap(unwrap_01_datastruct::Unwrap& uw) {



}