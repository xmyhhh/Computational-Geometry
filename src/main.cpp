


#pragma comment(linker, "/subsystem:windows")
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include <ShellScalingAPI.h>
#include <iostream>
#include "common/timer/timer.h"
#include "common/predicates/predicates.h"


extern void ShowTriangle();
extern void ShowDot();
extern void ShowLine();
extern void FileConvert();

extern void ConvexHull();
extern void Intersection();
extern void PolygonRandom();
extern void Monotonicity();
extern void Voronoi();
extern void Delaunay();
extern void Delaunay3D(HINSTANCE hInstance);
extern void Interpolation4D(HINSTANCE hInstance);
extern void ConstrainedDelaunay3D(HINSTANCE hInstance);
extern void constrained_DT();


bool RedirectConsoleIO()
{
	bool result = true;
	FILE* fp;

	// Redirect STDIN if the console has an input handle
	if (GetStdHandle(STD_INPUT_HANDLE) != INVALID_HANDLE_VALUE)
		if (freopen_s(&fp, "CONIN$", "r", stdin) != 0)
			result = false;
		else
			setvbuf(stdin, NULL, _IONBF, 0);

	// Redirect STDOUT if the console has an output handle
	if (GetStdHandle(STD_OUTPUT_HANDLE) != INVALID_HANDLE_VALUE)
		if (freopen_s(&fp, "CONOUT$", "w", stdout) != 0)
			result = false;
		else
			setvbuf(stdout, NULL, _IONBF, 0);

	// Redirect STDERR if the console has an error handle
	if (GetStdHandle(STD_ERROR_HANDLE) != INVALID_HANDLE_VALUE)
		if (freopen_s(&fp, "CONOUT$", "w", stderr) != 0)
			result = false;
		else
			setvbuf(stderr, NULL, _IONBF, 0);

	// Make C++ standard streams point to console as well.
	std::ios::sync_with_stdio(true);

	// Clear the error state for each of the C++ standard streams.
	std::wcout.clear();
	std::cout.clear();
	std::wcerr.clear();
	std::cerr.clear();
	std::wcin.clear();
	std::cin.clear();

	return result;
}
bool ReleaseConsole()
{
	bool result = true;
	FILE* fp;

	// Just to be safe, redirect standard IO to NUL before releasing.

	// Redirect STDIN to NUL
	if (freopen_s(&fp, "NUL:", "r", stdin) != 0)
		result = false;
	else
		setvbuf(stdin, NULL, _IONBF, 0);

	// Redirect STDOUT to NUL
	if (freopen_s(&fp, "NUL:", "w", stdout) != 0)
		result = false;
	else
		setvbuf(stdout, NULL, _IONBF, 0);

	// Redirect STDERR to NUL
	if (freopen_s(&fp, "NUL:", "w", stderr) != 0)
		result = false;
	else
		setvbuf(stderr, NULL, _IONBF, 0);

	// Detach from console
	if (!FreeConsole())
		result = false;

	return result;
}
void AdjustConsoleBuffer(int16_t minLength)
{
	// Set the screen buffer to be big enough to scroll some text
	CONSOLE_SCREEN_BUFFER_INFO conInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &conInfo);
	if (conInfo.dwSize.Y < minLength)
		conInfo.dwSize.Y = minLength;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), conInfo.dwSize);
}
bool CreateNewConsole(int16_t minLength)
{
	bool result = false;

	// Release any current console and redirect IO to NUL
	ReleaseConsole();

	// Attempt to create new console
	if (AllocConsole())
	{
		AdjustConsoleBuffer(minLength);
		result = RedirectConsoleIO();
	}

	return result;
}
bool AttachParentConsole(int16_t minLength)
{
	bool result = false;

	// Release any current console and redirect IO to NUL
	ReleaseConsole();

	// Attempt to attach to parent process's console
	if (AttachConsole(ATTACH_PARENT_PROCESS))
	{
		AdjustConsoleBuffer(minLength);
		result = RedirectConsoleIO();
	}

	return result;
}



int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
	//int main() {



	CreateNewConsole(1024);





	exactinit(0, 0, 0, 500, 500, 500);


	//opencv test
	//ShowTriangle();
	//ShowDot();
	//ShowLine();
	//FileConvert();


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

	//constrained_DT();

	ConstrainedDelaunay3D(hInstance);



	ReleaseConsole();


	return 0;
}

