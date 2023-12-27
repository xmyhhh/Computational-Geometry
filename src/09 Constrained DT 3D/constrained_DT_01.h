#pragma once
#include "common/typedef.h"
#include "common/helper.h"
#include "common/file/format_convert.h"


extern void Delaunay_3D_01(std::vector<cv::Point3d>& all_dots, Delaunay3D_01_datastruct::BW_DT_struct& bw_dt_struct);

namespace CDT_3D_01_datastruct {
	struct PLC {
		std::vector<cv::Point3d> vertex_array;
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

					face_index_array.push_back({ x , y, z });

				}

			}
			fclose(fp);

			return true;
		}

		VulkanDrawData toVulkanDrawData() {
			VulkanDrawData vulkan_data;

			vulkan_data.numberOfPoint = vertex_array.size();
			vulkan_data.points = (double*)malloc(vulkan_data.numberOfPoint * 3 * sizeof(double));

			//point
			for (int i = 0; i < vertex_array.size(); i++) {
				vulkan_data.points[i * 3] = vertex_array[i].x;
				vulkan_data.points[i * 3 + 1] = -vertex_array[i].y;
				vulkan_data.points[i * 3 + 2] = vertex_array[i].z;
			}

			//edge
			vulkan_data.numberOfTriangle = face_index_array.size();
			vulkan_data.triangles = (int*)malloc(vulkan_data.numberOfTriangle * 3 * sizeof(int));
			//int min_index = 999;

			for (int i = 0; i < face_index_array.size(); i++) {
				auto& face = face_index_array[i];
				vulkan_data.triangles[i * 3] = face.p1-1;
				vulkan_data.triangles[i * 3 + 1] = face.p2-1;
				vulkan_data.triangles[i * 3 + 2] = face.p3-1;
				
			}
			
			return vulkan_data;
		}
	};

}


void CDT_3D_01(CDT_3D_01_datastruct::PLC& plc, Delaunay3D_01_datastruct::BW_DT_struct& bw_dt_struct) {
	//Bowyer-Watson Algorithm 3d
	using namespace CDT_3D_01_datastruct;

	Delaunay_3D_01(plc.vertex_array, bw_dt_struct);

}