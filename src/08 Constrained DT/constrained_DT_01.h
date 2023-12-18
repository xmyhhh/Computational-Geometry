#pragma once

#include "common/typedef.h"
#include "common/helper.h"
#include "common/memory.h"
#include <common/file/format_convert.h>

namespace CDT_01_datastruct {

	//input
	struct Boundary
	{
		std::vector <int> point_array;
	};

	struct PSLG {
		int factor = 5;
		cv::Point2d offset;
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
			return true;
		}


		cv::Point2d get_position(int b_id, int v_id) {
			assert(b_id < boundary_array.size());
			assert(v_id < boundary_array[b_id].point_array.size());
			return vertex_array[boundary_array[b_id].point_array[v_id]] * factor + offset;
		}

		cv::Point2d get_position(int v_id) {
			assert(v_id < vertex_array.size());
			return vertex_array[v_id] * factor + offset;
		}
	};


	//run time

	struct Vertex
	{
		cv::Point2d position;
		bool is_infinity = false;
	};


	struct Edge {
		Vertex* orig;
		Vertex* end;
	};


	struct Strip_Region {
		bool empty = true;

		std::vector<Vertex*> vertex_array;
		std::vector<Edge> edge_array;

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


void CDT_01(CDT_01_datastruct::PSLG& plsg, CDT_01_datastruct::CDT& ctd) {
	//CDT from Chew's alg




}