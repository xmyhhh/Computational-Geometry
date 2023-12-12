


#pragma comment(linker, "/subsystem:windows")
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include <ShellScalingAPI.h>



extern void ShowTriangle();
extern void ShowDot();
extern void ShowLine();
extern void FileConvert();

extern void ConvexHull();
extern void Intersection();
extern void Polygon();
extern void Monotonicity();
extern void Voronoi();
extern void Delaunay();
extern void Delaunay3D(HINSTANCE hInstance);
extern void Interpolation4D(HINSTANCE hInstance);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
//int main() {
	//opencv test
	//ShowTriangle();
	//ShowDot();
	//ShowLine();
	FileConvert();


	// 2D
	//ConvexHull();
	//Intersection();
	//Polygon();
	//Monotonicity();
	//Voronoi();
	//Delaunay();

	// 3D
	//Delaunay3D(hInstance);

	//Interpolation4D(hInstance);

	return 0;
}

