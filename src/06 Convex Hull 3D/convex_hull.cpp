
#include "common/typedef.h"
#include "convex_hull_01.h"
#include "common/vulkan/VulkanExampleBase.h"

class  ConvexHull3D_Vulkan :public VulkanExampleBase
{

public:
	ConvexHull3D_Vulkan() : VulkanExampleBase(true) {
		title = "convex";
		camera.type = Camera::CameraType::firstperson;
		camera.setPosition(glm::vec3(10.0f, 13.0f, 1.8f));
		camera.setRotation(glm::vec3(-62.5f, 90.0f, 0.0f));
		camera.movementSpeed = 4.0f;
		camera.setPerspective(60.0f, (float)width / (float)height, 0.1f, 256.0f);
		camera.rotationSpeed = 0.25f;
		paused = true;
		timerSpeed *= 0.25f;
	}

	~ConvexHull3D_Vulkan()
	{

	}
	void render() {}
};


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void ConvexHull3D(HINSTANCE hInstance) {


	int width = 1000;
	int height = 1000;
	int deepth = 1000;

	std::vector<cv::Point3d> all_dots;

	int size = 300;
	for (int i = 0; i < size; i++)
	{
		all_dots.push_back(cv::Point3d(rand() % width, rand() % height, rand() % deepth));

	}
	auto ConvexHull3D_Vulkan_app = new ConvexHull3D_Vulkan();
	ConvexHull3D_Vulkan_app->initVulkan();
	ConvexHull3D_Vulkan_app->setupWindow(hInstance, WndProc);
	ConvexHull3D_Vulkan_app->prepare();
	ConvexHull3D_Vulkan_app->renderLoop();
	delete(ConvexHull3D_Vulkan_app);
}