
#include "common/typedef.h"
#include "convex_hull_01.h"
#include "common/vulkan/VulkanExampleBase.h"

class  ConvexHull3D_Vulkan :public VulkanExampleBase
{
	struct PushBlock {
		glm::mat4 mvp;
		float roughness;
		uint32_t numSamples = 32u;
	} pushBlock;

	struct UBOMatrices {
		glm::mat4 projection;
		glm::mat4 view;
		glm::vec3 camPos;
	};

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
		prepareUniformBuffers();
		setupDescriptorSetLayout();
		setupDescriptorSets();
		preparePipelines();
		buildCommandBuffers();
		prepared = true;
	}

	void loadAssets()
	{
		const uint32_t glTFLoadingFlags = vkglTF::FileLoadingFlags::PreTransformVertices | vkglTF::FileLoadingFlags::PreMultiplyVertexColors | vkglTF::FileLoadingFlags::FlipY;

		sphere.model.loadFromFile(getAssetPath() + "sphere.gltf", vulkanDevice, queue, glTFLoadingFlags);

	}

	void preparePipelines() {
		std::vector<VkPushConstantRange> pushConstantRanges = {
			vks::initializers::pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushBlock), 0),
		};
		VkPipelineLayoutCreateInfo pipelineLayoutCI = vks::initializers::pipelineLayoutCreateInfo(&descriptorSetLayout, 1);;
		pipelineLayoutCI.pushConstantRangeCount = 1;
		pipelineLayoutCI.pPushConstantRanges = pushConstantRanges.data();
		VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutCI, nullptr, &pipelineLayout));

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = vks::initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
		VkPipelineRasterizationStateCreateInfo rasterizationState = vks::initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
		VkPipelineColorBlendAttachmentState blendAttachmentState = vks::initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
		VkPipelineColorBlendStateCreateInfo colorBlendState = vks::initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);
		VkPipelineDepthStencilStateCreateInfo depthStencilState = vks::initializers::pipelineDepthStencilStateCreateInfo(VK_FALSE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL);
		VkPipelineViewportStateCreateInfo viewportState = vks::initializers::pipelineViewportStateCreateInfo(1, 1);
		VkPipelineMultisampleStateCreateInfo multisampleState = vks::initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);
		std::vector<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamicState = vks::initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables);
		VkGraphicsPipelineCreateInfo pipelineCI = vks::initializers::pipelineCreateInfo(pipelineLayout, renderPass);

		std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages;
		pipelineCI.pInputAssemblyState = &inputAssemblyState;
		pipelineCI.pRasterizationState = &rasterizationState;
		pipelineCI.pColorBlendState = &colorBlendState;
		pipelineCI.pMultisampleState = &multisampleState;
		pipelineCI.pViewportState = &viewportState;
		pipelineCI.pDepthStencilState = &depthStencilState;
		pipelineCI.pDynamicState = &dynamicState;
		pipelineCI.stageCount = static_cast<uint32_t>(shaderStages.size());
		pipelineCI.pStages = shaderStages.data();
		pipelineCI.pVertexInputState = vkglTF::Vertex::getPipelineVertexInputState({ vkglTF::VertexComponent::Position, vkglTF::VertexComponent::Normal, vkglTF::VertexComponent::UV, vkglTF::VertexComponent::Tangent });

		rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
		shaderStages[0] = loadShader(getShadersPath() + "/object_vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
		shaderStages[1] = loadShader(getShadersPath() + "/object_frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

		// Enable depth test and write
		depthStencilState.depthWriteEnable = VK_TRUE;
		depthStencilState.depthTestEnable = VK_TRUE;
		VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCI, nullptr, &pipline));

	}

	void buildCommandBuffers()override {
		VkCommandBufferBeginInfo cmdBufInfo = vks::initializers::commandBufferBeginInfo();

		VkClearValue clearValues[2];
		clearValues[0].color = { 0.025f, 0.025f, 0.025f, 1.0f };
		clearValues[1].color = { 1.0f, 0 };

		VkViewport viewport;
		VkRect2D scissor;

		for (int32_t i = 0; i < drawCmdBuffers.size(); ++i)
		{
			VK_CHECK_RESULT(vkBeginCommandBuffer(drawCmdBuffers[i], &cmdBufInfo));


			VkRenderPassBeginInfo renderPassBeginInfo = vks::initializers::renderPassBeginInfo();
			{
				renderPassBeginInfo.renderPass = renderPass;
				renderPassBeginInfo.framebuffer = frameBuffers[i];
				renderPassBeginInfo.renderArea.extent.width = width;
				renderPassBeginInfo.renderArea.extent.height = height;
				renderPassBeginInfo.clearValueCount = 2;
				renderPassBeginInfo.pClearValues = clearValues;
			}


			vkCmdBeginRenderPass(drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

			viewport = vks::initializers::viewport(width, height, 0.0f, 1.0f);
			vkCmdSetViewport(drawCmdBuffers[i], 0, 1, &viewport);

			scissor = vks::initializers::rect2D(width, height, 0, 0);
			vkCmdSetScissor(drawCmdBuffers[i], 0, 1, &scissor);


			vkCmdBindPipeline(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipline);
			vkCmdBindDescriptorSets(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);

			sphere.pos = glm::vec3(0, 0, 0);
			sphere.size = glm::vec3(1, 1, 1);
			glm::mat4 model = glm::translate(glm::mat4(1.0f), sphere.pos);
			model = glm::scale(model, sphere.size);
			vkCmdPushConstants(drawCmdBuffers[i], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushBlock), &model);
			sphere.model.draw(drawCmdBuffers[i]);

			/*		for (auto& scene_object : scene.objects) {
						glm::mat4 model = glm::translate(glm::mat4(1.0f), scene_object.pos);
						model = glm::scale(model, scene_object.size);
						vkCmdPushConstants(drawCmdBuffers[i], offscreenPass.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushBlock), &model);
						scene_object.model.draw(drawCmdBuffers[i]);
					}*/


			drawUI(drawCmdBuffers[i]);

			vkCmdEndRenderPass(drawCmdBuffers[i]);

			VK_CHECK_RESULT(vkEndCommandBuffer(drawCmdBuffers[i]));
		}
	}

	void prepareUniformBuffers()
	{
		// Object vertex shader uniform buffer
		VK_CHECK_RESULT(vulkanDevice->createBuffer(
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&UBO,
			sizeof(UBOMatrices)));

		// Map persistent
		VK_CHECK_RESULT(UBO.map());

		updateUniformBuffers();

	}


	void setupDescriptorSetLayout() {
		std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
			vks::initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT , 0)
		};

		VkDescriptorSetLayoutCreateInfo descriptorLayout = vks::initializers::descriptorSetLayoutCreateInfo(setLayoutBindings);
		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &descriptorLayout, nullptr, &descriptorSetLayout));

		std::vector<VkDescriptorPoolSize> poolSizes = {
			vks::initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2)
		};

		VkDescriptorPoolCreateInfo descriptorPoolInfo = vks::initializers::descriptorPoolCreateInfo(poolSizes, 2);

		VK_CHECK_RESULT(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &descriptorPool));
	}

	void setupDescriptorSets() {
		VkDescriptorSetAllocateInfo allocInfo =
			vks::initializers::descriptorSetAllocateInfo(descriptorPool, &descriptorSetLayout, 1);

		// 3D object descriptor set
		VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet));

		std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
			vks::initializers::writeDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &UBO.descriptor)

		};
		vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, NULL);
	}

	void updateUniformBuffers()
	{
		// 3D object
		uboMatrices.projection = camera.matrices.perspective;
		uboMatrices.view = camera.matrices.view;
		uboMatrices.camPos = camera.position * -1.0f;

		memcpy(UBO.mapped, &uboMatrices, sizeof(uboMatrices));
	}

	virtual void viewChanged()
	{
		updateUniformBuffers();
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

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSet descriptorSet;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipline;

	vks::Buffer UBO;
	UBOMatrices uboMatrices;
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