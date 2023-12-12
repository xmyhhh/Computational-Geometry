#include "format_convert.h"
#include <cstdio>
#include <stdlib.h>
#include <cassert>


struct Polygon
{
	int numberOfPoints = 3; //3表示三角形
	int* pointList;
};

struct FileData {

	int numberOfPoints = 0;
	double* pointList;

	int numberOfFace = 0;
	Polygon* faceList;

};


char* read_line(char* string, FILE* infile, int* linenumber)
{
	char* result;

	// Search for a non-empty line.
	do
	{
		result = fgets(string, 2048, infile);
		if (linenumber) (*linenumber)++;
		if (result == (char*)NULL)
		{
			return (char*)NULL;
		}
		// Skip white spaces.
		while ((*result == ' ') || (*result == '\t')) result++;
		// If it's end of line, read another line and try again.
	} while ((*result == '\0') || (*result == '\r') || (*result == '\n'));
	return result;
}


bool load_vtk(const char* in_file_path, FileData& data) {
	FILE* fp = fopen(in_file_path, "r");
	if (fp == (FILE*)NULL)
	{
		//printf("File I/O Error:  Cannot create file %s.\n", vtk_file_path);
		return false;
	}


	char buffer[2048];
	char* bufferp;
	int line_count = 0;


	int nverts = 0, iverts = 0;
	int ntetrahedras = 0, itetrahedras = 0;
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
			data.numberOfPoints = nverts;
			data.pointList = new double[nverts * 3];
		}
		else if (nverts > iverts)
		{
			data.pointList[iverts * 3] = (double)strtod(bufferp, &bufferp);
			data.pointList[iverts * 3 + 1] = (double)strtod(bufferp, &bufferp);
			data.pointList[iverts * 3 + 2] = (double)strtod(bufferp, &bufferp);
			iverts++;
		}
		else if (ntetrahedras == 0)
		{
			//CELLS 35186 175930
			sscanf(bufferp, "%*s %d %*d", &ntetrahedras);
			data.faceList = new Polygon[ntetrahedras * 4];
			data.numberOfFace = ntetrahedras * 4;
		}
		else if (ntetrahedras > itetrahedras)
		{
			int p0, p1, p2, p3;
			sscanf(bufferp, "%*d %d %d %d %d",
				&p0,
				&p1,
				&p2,
				&p3
			);


			data.faceList[itetrahedras * 4 + 0].pointList = new int[4];
			data.faceList[itetrahedras * 4 + 1].pointList = new int[4];
			data.faceList[itetrahedras * 4 + 2].pointList = new int[4];
			data.faceList[itetrahedras * 4 + 3].pointList = new int[4];

			auto polygon_point = &data.faceList[itetrahedras * 4 + 0].pointList[0];
			polygon_point[0] = p0;
			polygon_point[1] = p1;
			polygon_point[2] = p2;

			polygon_point = &data.faceList[itetrahedras * 4 + 1].pointList[0];
			polygon_point[0] = p0;
			polygon_point[1] = p1;
			polygon_point[2] = p3;

			polygon_point = &data.faceList[itetrahedras * 4 + 2].pointList[0];
			polygon_point[0] = p0;
			polygon_point[1] = p2;
			polygon_point[2] = p3;

			polygon_point = &data.faceList[itetrahedras * 4 + 3].pointList[0];
			polygon_point[0] = p1;
			polygon_point[1] = p2;
			polygon_point[2] = p3;

			itetrahedras++;
		}
		else
		{
			break;
		}
	}
	fclose(fp);
	return true;
}

bool save_off(const char* out_file_path, const FileData& data) {

	FILE* fout = fopen(out_file_path, "w");

	if (fout == (FILE*)NULL)
	{
		//printf("File I/O Error:  Cannot create file %s.\n", vtk_file_path);

		return false;
	}
	fprintf(fout, "OFF\n");
	fprintf(fout, "%d  %d  %d\n", data.numberOfPoints, data.numberOfFace, 0);
	for (int i = 0; i < data.numberOfPoints; i++) {
		fprintf(fout, "%.16g %.16g %.16g\n", data.pointList[i * 3], data.pointList[i * 3 + 1], data.pointList[i * 3 + 2]);
	}
	for (int i = 0; i < data.numberOfFace; i++) {
		auto& face = data.faceList[i];
		assert(face.numberOfPoints == 3);

		fprintf(fout, "%d %d %d %d\n", 3, face.pointList[0], face.pointList[1], face.pointList[2]);
	}
	fclose(fout);
	return true;
}

bool save_obj(const char* out_file_path, const FileData& data) {

	FILE* fout = fopen(out_file_path, "w");

	if (fout == (FILE*)NULL)
	{
		//printf("File I/O Error:  Cannot create file %s.\n", vtk_file_path);

		return false;
	}


	for (int i = 0; i < data.numberOfPoints; i++) {
		fprintf(fout, "%c %.16g %.16g %.16g\n", 'v', data.pointList[i * 3], data.pointList[i * 3 + 1], data.pointList[i * 3 + 2]);
	}
	for (int i = 0; i < data.numberOfFace; i++) {
		auto& face = data.faceList[i];
		assert(face.numberOfPoints == 3);

		fprintf(fout, "%c %d %d %d\n", 'f', face.pointList[0]+1, face.pointList[1]+1, face.pointList[2]+1);
	}
	fclose(fout);
	return true;
}

//public function
bool vtk_to_off(const char* in_file_path, const char* out_file_path)
{
	FileData data;
	//load
	bool res = load_vtk(in_file_path, data);
	//save
	if (res)
		res = save_off(out_file_path, data);
	return res;
}

bool vtk_to_obj(const char* in_file_path, const char* out_file_path)
{
	FileData data;
	//load
	bool res = load_vtk(in_file_path, data);

	if (res)
		//save
		res = save_obj(out_file_path, data);

	return res;
}