#pragma once
#include <cstdio>
#include <stdlib.h>
#include <cassert>
char* read_line(char* string, FILE* infile, int* linenumber);

bool vtk_to_off(const char * in_file_path, const char* out_file_path);
bool vtk_to_obj(const char* in_file_path, const char* out_file_path);