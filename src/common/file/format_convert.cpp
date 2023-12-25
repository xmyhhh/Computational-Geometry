#include "format_convert.h"
#include <string.h>
#include <vector>
#include <set>

struct Cell
{
	int numberOfPoints = 3; //3表示三角形
	int* pointList;
	int numberOfAttr = 0;
	int* attr;
};

struct FileData {

	int numberOfPoints = 0;
	double* pointList;

	int numberOfCell = 0;
	Cell* cellList;

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
			data.cellList = new Cell[ntetrahedras];
			data.numberOfCell = ntetrahedras;
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


			data.cellList[itetrahedras].pointList = new int[4];
			data.cellList[itetrahedras].numberOfPoints = 4;

			auto Cell_point = &data.cellList[itetrahedras].pointList[0];
			Cell_point[0] = p0;
			Cell_point[1] = p1;
			Cell_point[2] = p2;
			Cell_point[3] = p3;

			itetrahedras++;
		}
		else if (!readattr)
		{
			char s[20];
			sscanf(bufferp, "%s", &s);
			if (strcmp(s, "CELL_DATA") == 0) {
				readattr = true;
				read_line(buffer, fp, &line_count); //SCALARS cell_scalars int 1
				read_line(buffer, fp, &line_count); //LOOKUP_TABLE default
			}
		}
		else {
			int attr;
			sscanf(bufferp, "%d", &attr);
			data.cellList[itetrahedrasattr].attr = (int*)malloc(sizeof(int) * 1);
			data.cellList[itetrahedrasattr].numberOfAttr = 1;
			*data.cellList[itetrahedrasattr].attr = attr;
			itetrahedrasattr++;
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
	fprintf(fout, "%d  %d  %d\n", data.numberOfPoints, data.numberOfCell, 0);
	for (int i = 0; i < data.numberOfPoints; i++) {
		fprintf(fout, "%.16g %.16g %.16g\n", data.pointList[i * 3], data.pointList[i * 3 + 1], data.pointList[i * 3 + 2]);
	}
	for (int i = 0; i < data.numberOfCell; i++) {
		auto& face = data.cellList[i];
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
	for (int i = 0; i < data.numberOfCell; i++) {
		auto& face = data.cellList[i];
		assert(face.numberOfPoints == 3);

		fprintf(fout, "%c %d %d %d\n", 'f', face.pointList[0] + 1, face.pointList[1] + 1, face.pointList[2] + 1);
	}
	fclose(fout);
	return true;
}


bool save_f3grid(const char* out_file_path, const FileData& data) {

	FILE* fout = fopen(out_file_path, "w");

	if (fout == (FILE*)NULL)
	{
		//printf("File I/O Error:  Cannot create file %s.\n", vtk_file_path);

		return false;
	}

	fprintf(fout, "* GRIDPOINTS\n");
	for (int i = 0; i < data.numberOfPoints; i++) {
		fprintf(fout, "%c %d %.16g %.16g %.16g\n", 'G', i + 1, data.pointList[i * 3], data.pointList[i * 3 + 1], data.pointList[i * 3 + 2]);
	}
	fprintf(fout, "* ZONES\n");
	for (int i = 0; i < data.numberOfCell; i++) {
		auto& cell = data.cellList[i];
		assert(cell.numberOfPoints == 4);

		fprintf(fout, "%c T4 %d %d %d %d %d\n", 'Z', i + 1, cell.pointList[0] + 1, cell.pointList[1] + 1, cell.pointList[2] + 1, cell.pointList[3] + 1);
	}
	fprintf(fout, "* ZONE GROUPS\n");
	std::vector<std::vector<int>> all_zone_group(100);

	for (int i = 0; i < data.numberOfCell; i++) {
		auto& cell = data.cellList[i];
		int g_id = cell.attr[0] - 1;
		all_zone_group[g_id].push_back(i+1);
	}

	for (int i = 0; i < all_zone_group.size(); i++) {
		auto g = all_zone_group[i];
		if (g.size() != 0) {
			fprintf(fout, "ZGROUP \"ZG_00%d\" SLOT 1 \n", i);

			for (int j = 0; j < g.size(); j++) {
				fprintf(fout, "%d ", g[j]);
			}

			fprintf(fout, "\n");
		}
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

bool vtk_to_f3grid(const char* in_file_path, const char* out_file_path)
{
	FileData data;
	//load
	bool res = load_vtk(in_file_path, data);

	//save
	if (res)
		res = save_f3grid(out_file_path, data);

	return res;
}
