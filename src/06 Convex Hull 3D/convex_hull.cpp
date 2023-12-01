
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

	void prepare()override {
		VulkanExampleBase::prepare();
		loadAssets();
		buildCommandBuffers();
		prepared = true;
	}

	void loadAssets()
	{
		const uint32_t glTFLoadingFlags = vkglTF::FileLoadingFlags::PreTransformVertices | vkglTF::FileLoadingFlags::PreMultiplyVertexColors | vkglTF::FileLoadingFlags::FlipY;

		sphere.model.loadFromFile(getAssetPath() + "sphere.gltf", vulkanDevice, queue, glTFLoadingFlags);

	}

	void buildCommandBuffers()override {
		VkCommandBufferBeginInfo cmdBufInfo = vks::initializers::commandBufferBeginInfo();

		VkClearValue clearValues[2];
		clearValues[0].color = {0.025f, 0.025f, 0.025f, 1.0f};
		clearValues[1].color = { 1.0f, 0 };

		VkViewport viewport;
		VkRect2D scissor;

		for (int32_t i = 0; i < drawCmdBuffers.size(); ++i)
		{
			VK_CHECK_RESULT(vkBeginCommandBuffer(drawCmdBuffers[i], &cmdBufInfo));


			VkRenderPassBeginInfo renderPassBeginInfo = vks::initializers::renderPassBeginInfo();
			renderPassBeginInfo.renderPass = renderPass;
			renderPassBeginInfo.framebuffer = frameBuffers[i];
			renderPassBeginInfo.renderArea.extent.width = width;
			renderPassBeginInfo.renderArea.extent.height = height;
			renderPassBeginInfo.clearValueCount = 2;
			renderPassBeginInfo.pClearValues = clearValues;

			vkCmdBeginRenderPass(drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

			drawUI(drawCmdBuffers[i]);

			vkCmdEndRenderPass(drawCmdBuffers[i]);

			VK_CHECK_RESULT(vkEndCommandBuffer(drawCmdBuffers[i]));
		}
	}

	void render()override
	{
		if (!prepared)
			return;
		VulkanExampleBase::prepareFrame();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &drawCmdBuffers[currentBuffer];
		VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
		VulkanExampleBase::submitFrame();

	}
protected:
	SceneObject sphere;


};
ConvexHull3D_Vulkan* ConvexHull3D_Vulkan_app;
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ConvexHull3D_Vulkan_app != NULL)
	{																								
		ConvexHull3D_Vulkan_app->handleMessages(hWnd, uMsg, wParam, lParam);
	}
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
	 ConvexHull3D_Vulkan_app = new ConvexHull3D_Vulkan();
	ConvexHull3D_Vulkan_app->initVulkan();
	ConvexHull3D_Vulkan_app->setupWindow(hInstance, WndProc);
	ConvexHull3D_Vulkan_app->prepare();
	ConvexHull3D_Vulkan_app->renderLoop();
	delete(ConvexHull3D_Vulkan_app);
}