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
	};

	struct Face {
		Vertex* p1;
		Vertex* p2;
		Vertex* p3;
	};

	struct Edge {
		Vertex* orig;
		Vertex* end;
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

			vertex_pool.initializePool(sizeof(Vertex), vertex_array.size() * 2, 8, 64);
			edge_pool.initializePool(sizeof(Edge), face_index_array.size() * 3, 8, 64);
			face_pool.initializePool(sizeof(Face), face_index_array.size() * 2, 8, 64);

			for (auto vtx : vertex_array) {
				auto p = (Vertex*)vertex_pool.allocate();
				p->position = vtx;
				p->vtx_from_plc = true;
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
				}
				if (!find_edge(f->p2, f->p3, e)) {
					e = (Edge*)edge_pool.allocate();
					e->end = f->p2;
					e->orig = f->p3;
				}
				if (!find_edge(f->p3, f->p1, e)) {
					e = (Edge*)edge_pool.allocate();
					e->end = f->p3;
					e->orig = f->p1;
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
			vulkan_data.numberOfTriangle = face_pool.size();
			vulkan_data.triangles = (int*)malloc(vulkan_data.numberOfTriangle * 3 * sizeof(int));
			//int min_index = 999;

			for (int i = 0; i < face_pool.size(); i++) {

				auto face = (Face*)face_pool[i];
				auto aaa = face_pool.get_index(face->p1);
				vulkan_data.triangles[i * 3] = vertex_pool.get_index(face->p1);
				vulkan_data.triangles[i * 3 + 1] = vertex_pool.get_index(face->p2);
				vulkan_data.triangles[i * 3 + 2] = vertex_pool.get_index(face->p3);

			}

			return vulkan_data;
		}
	};

}


void CDT_3D_01(CDT_3D_01_datastruct::PLC& plc, Delaunay3D_01_datastruct::BW_DT_struct& bw_dt_struct) {
	//Bowyer-Watson Algorithm 3d
	using namespace CDT_3D_01_datastruct;

	//Delaunay_3D_01(plc.vertex_array, bw_dt_struct);

	//edge protection algorithm


}