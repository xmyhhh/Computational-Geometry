
#include "common/typedef.h"
#include "constrained_DT_01.h"

#include "common/vulkan_app.h"

Delaunay3D_Vulkan *cdt_Vulkan_app;

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
    ASSERT(plc.init_from_file(getAssetPath() + "suzanne.obj"));


    Delaunay3D_01_datastruct::BW_DT_struct bw_dt_struct;
    CDT_3D_01(plc, bw_dt_struct);

    cdt_Vulkan_app = new Delaunay3D_Vulkan();
    cdt_Vulkan_app->SetData(bw_dt_struct.toVulkanDrawData());
    cdt_Vulkan_app->initVulkan();
    cdt_Vulkan_app->setupWindow(hInstance, ConstrainedDelaunay3D_WndProc);
    cdt_Vulkan_app->prepare();
    cdt_Vulkan_app->renderLoop();

    delete (cdt_Vulkan_app);
}