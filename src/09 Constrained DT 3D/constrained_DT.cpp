
#include "common/typedef.h"
#include "constrained_DT_01.h"

#include "common/vulkan_app.h"

Delaunay3D_Vulkan* cdt_Vulkan_app;

LRESULT CALLBACK ConstrainedDelaunay3D_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (cdt_Vulkan_app != NULL) {
		cdt_Vulkan_app->handleMessages(hWnd, uMsg, wParam, lParam);
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void ConstrainedDelaunay3D(HINSTANCE hInstance) {
	int width = 50;
	int height = 50;
	int deepth = 50;

	std::vector<cv::Point3d> all_dots;

	CDT_3D_01_datastruct::PLC plc;
	//ASSERT(plc.init_from_file(getAssetPath() + "tet.obj"));
	//ASSERT(plc.init_from_file(getAssetPath() + "doughnut.obj"));
	ASSERT(plc.init_from_file(getAssetPath() + "doughnut2.obj"));
	//ASSERT(plc.init_from_file(getAssetPath() + "suzanne.obj"));

	auto res = CDT_3D_01(plc);

	cdt_Vulkan_app = new Delaunay3D_Vulkan();
	//cdt_Vulkan_app->SetData(plc.toVulkanDrawData());
	cdt_Vulkan_app->SetData(res.toVulkanDrawData());
	cdt_Vulkan_app->initVulkan();
	cdt_Vulkan_app->setupWindow(hInstance, ConstrainedDelaunay3D_WndProc);
	cdt_Vulkan_app->prepare();
	cdt_Vulkan_app->renderLoop();

	delete (cdt_Vulkan_app);
}