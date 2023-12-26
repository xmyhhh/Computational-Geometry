
#include "common/typedef.h"
#include "delaunay_01.h"
#include "common/vulkan_app.h"


Delaunay3D_Vulkan* dt_Vulkan_app;
LRESULT CALLBACK Delaunay3D_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (dt_Vulkan_app != NULL)
	{
        dt_Vulkan_app->handleMessages(hWnd, uMsg, wParam, lParam);
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void Delaunay3D(HINSTANCE hInstance) {
	debug_cout("Delaunay3D begin");
	int width = 50;
	int height = 50;
	int deepth = 50;

	std::vector<cv::Point3d> all_dots;


#define use_random_point 1

	if (use_random_point) {
		int size = 500;
		for (int i = 0; i < size; i++)
		{
			all_dots.push_back(cv::Point3d(rand() % width + 1, rand() % height + 1, rand() % deepth + 1));
		}
	}

	Delaunay3D_01_datastruct::BW_DT_struct res;

	Delaunay_3D_01(all_dots, res);

    dt_Vulkan_app = new Delaunay3D_Vulkan();
    dt_Vulkan_app->SetData(res.toVulkanDrawData());
    dt_Vulkan_app->initVulkan();
    dt_Vulkan_app->setupWindow(hInstance, Delaunay3D_WndProc);
    dt_Vulkan_app->prepare();
    dt_Vulkan_app->renderLoop();

	delete(dt_Vulkan_app);
}