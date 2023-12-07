


#pragma comment(linker, "/subsystem:windows")
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include <ShellScalingAPI.h>



extern void ShowTriangle();
extern void ShowDot();
extern void ShowLine();

extern void ConvexHull();
extern void Intersection();
extern void Polygon();
extern void Monotonicity();
extern void Voronoi();
extern void Delaunay();
extern void ConvexHull3D(HINSTANCE hInstance);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

	//opencv 左上角是（0，0）

	ShowTriangle();
	//ShowDot();
	//ShowLine();
	//ConvexHull();
	//Intersection();

	//Polygon();
	//Monotonicity();
	//Voronoi();
	Delaunay();
	return 0;
}

