
#include "common/typedef.h"
#include "unwrap_01.h"

#include "common/vulkan_app.h"

Delaunay3D_Vulkan* unwrap_Vulkan_app;

LRESULT CALLBACK Unwrap3D_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (unwrap_Vulkan_app != NULL) {
		unwrap_Vulkan_app->handleMessages(hWnd, uMsg, wParam, lParam);
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void Unwrap(HINSTANCE hInstance) {
	int width = 50;
	int height = 50;
	int deepth = 50;

    debug_cout("begin");

	unwrap_01_datastruct::Unwrap uw;
	ASSERT(uw.init_from_file(getAssetPath() + "23308.1.vtk"));

	Unwrap(uw);

	unwrap_Vulkan_app = new Delaunay3D_Vulkan();

	unwrap_Vulkan_app->SetData(uw.toVulkanDrawData_Face());
	unwrap_Vulkan_app->initVulkan();
	unwrap_Vulkan_app->setupWindow(hInstance, Unwrap3D_WndProc);
	unwrap_Vulkan_app->prepare();
	unwrap_Vulkan_app->renderLoop();

	delete (unwrap_Vulkan_app);
}