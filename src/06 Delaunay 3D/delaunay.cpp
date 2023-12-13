
#include "common/typedef.h"
#include "delaunay_01.h"
#include "common/vulkan/VulkanExampleBase.h"

class  Delaunay3D_Vulkan :public VulkanExampleBase
{
	struct PushBlock_Point {
		glm::mat4 mvp;
		glm::vec4 color = glm::vec4(1);
	};

	struct PushBlock_Triangle {
		glm::vec4 p1;
		glm::vec4 p2;
		glm::vec4 p3;
	};

	struct UBOMatrices {
		glm::mat4 projection;
		glm::mat4 view;
		glm::vec4 camPos;
	};



public:
	struct DrawData {
		int numberOfPoint;
		double* points; //location xyz(3 double) * numberOfPoint
		int numberOfTriangle;
		int* triangles;//index of triangle (3 int) * numberOfTriangle
	};


	Delaunay3D_Vulkan() : VulkanExampleBase(true) {
		title = "convex";
		camera.type = Camera::CameraType::firstperson;
		camera.setPosition(glm::vec3(10.0f, 13.0f, 1.8f));
		camera.setRotation(glm::vec3(-62.5f, 90.0f, 0.0f));
		camera.setMovementSpeed(20);

		camera.setPerspective(60.0f, (float)width / (float)height, 0.1f, 256.0f);
		camera.rotationSpeed = 0.55f;
		paused = true;
		timerSpeed *= 0.25f;
	}

	~Delaunay3D_Vulkan()
	{

	}

	void SetData(VulkanDrawData _data) {
		data = _data;
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
	void getEnabledFeatures() {
		enabledFeatures.fillModeNonSolid = true;
		enabledFeatures.wideLines = true;
		enabledFeatures.samplerAnisotropy = true;
	}
	void loadAssets()
	{
		const uint32_t glTFLoadingFlags = vkglTF::FileLoadingFlags::PreTransformVertices | vkglTF::FileLoadingFlags::PreMultiplyVertexColors | vkglTF::FileLoadingFlags::FlipY;

		sphere.model.loadFromFile(getAssetPath() + "sphere.gltf", vulkanDevice, queue, glTFLoadingFlags);

	}

	void preparePipelines() {
		std::vector<VkPushConstantRange> pushConstantRanges = {
			vks::initializers::pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushBlock_Point), 0),
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


		//begin draw triangle pipline create
		pushConstantRanges = {
			vks::initializers::pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushBlock_Triangle), 0),
		};
		VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutCI, nullptr, &pipelineLayout_triangle));
		rasterizationState = vks::initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_LINE, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.vertexBindingDescriptionCount = 0,
			.vertexAttributeDescriptionCount = 0,
		};

		pipelineCI.pVertexInputState = &vertexInputInfo;
		pipelineCI.layout = pipelineLayout_triangle;
		shaderStages[0] = loadShader(getShadersPath() + "/triangle_vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
		shaderStages[1] = loadShader(getShadersPath() + "/triangle_frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

		VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCI, nullptr, &pipline_triangle));

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

			for (int j = 0; j < data.numberOfPoint; j++) {
				auto pos = glm::vec3(data.points[j * (3)], data.points[j * (3) + 1], data.points[j * (3) + 2]);
				auto size = glm::vec3(1, 1, 1);
				PushBlock_Point pushblock;
				pushblock.mvp = glm::translate(glm::mat4(1.0f), pos);
				pushblock.mvp = glm::scale(pushblock.mvp, size);
				if (data.numberOfPointAttr == 4) {
					pushblock.color = glm::vec4(
						data.attr[j * (data.numberOfPointAttr)],
						data.attr[j * (data.numberOfPointAttr) + 1],
						data.attr[j * (data.numberOfPointAttr) + 2],
						data.attr[j * (data.numberOfPointAttr) + 3]
					);
				}
				else {
					pushblock.color = glm::vec4(1);
				}
				vkCmdPushConstants(drawCmdBuffers[i], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushBlock_Point), (void*)&pushblock);
				sphere.model.draw(drawCmdBuffers[i]);
			}

			drawUI(drawCmdBuffers[i]);

			vkCmdBindPipeline(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipline_triangle);


			scissor = vks::initializers::rect2D(width, height, 0, 0);
			vkCmdSetScissor(drawCmdBuffers[i], 0, 1, &scissor);

			vkCmdBindDescriptorSets(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout_triangle, 0, 1, &descriptorSet, 0, NULL);

			for (int j = 0; j < data.numberOfTriangle; j++) {
				auto tr = &data.triangles[j * 3];

				glm::vec4 p1 = glm::vec4(data.points[tr[0] * 3], data.points[tr[0] * 3 + 1], data.points[tr[0] * 3 + 2], 1);
				glm::vec4 p2 = glm::vec4(data.points[tr[1] * 3], data.points[tr[1] * 3 + 1], data.points[tr[1] * 3 + 2], 1);
				glm::vec4 p3 = glm::vec4(data.points[tr[2] * 3], data.points[tr[2] * 3 + 1], data.points[tr[2] * 3 + 2], 1);
				vkCmdPushConstants(drawCmdBuffers[i], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::vec4), &p1);
				vkCmdPushConstants(drawCmdBuffers[i], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::vec4), sizeof(glm::vec4), &p2);
				vkCmdPushConstants(drawCmdBuffers[i], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::vec4) * 2, sizeof(glm::vec4), &p3);
				vkCmdDraw(drawCmdBuffers[i], 3, 1, 0, 0);
			}



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
		uboMatrices.camPos = glm::vec4(camera.position * -1.0f, 1.f);

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
	VulkanDrawData data;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSet descriptorSet;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipline;


	VkPipelineLayout pipelineLayout_triangle;
	VkPipeline pipline_triangle;

	vks::Buffer UBO;
	UBOMatrices uboMatrices;
};

Delaunay3D_Vulkan* ConvexHull3D_Vulkan_app;
LRESULT CALLBACK Delaunay3D_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ConvexHull3D_Vulkan_app != NULL)
	{
		ConvexHull3D_Vulkan_app->handleMessages(hWnd, uMsg, wParam, lParam);
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void Delaunay3D(HINSTANCE hInstance) {
	int width = 100;
	int height = 100;
	int deepth = 100;

	std::vector<cv::Point3d> all_dots;

	int size = 1;
	for (int i = 0; i < size; i++)
	{
		all_dots.push_back(cv::Point3d(rand() % width, rand() % height, rand() % deepth));

	}
	Delaunay3D_01_datastruct::BW_DT_struct res;

	Delaunay_3D_01(all_dots, res);

	ConvexHull3D_Vulkan_app = new Delaunay3D_Vulkan();
	ConvexHull3D_Vulkan_app->SetData(res.toVulkanDrawData());
	ConvexHull3D_Vulkan_app->initVulkan();
	ConvexHull3D_Vulkan_app->setupWindow(hInstance, Delaunay3D_WndProc);
	ConvexHull3D_Vulkan_app->prepare();
	ConvexHull3D_Vulkan_app->renderLoop();


	delete(ConvexHull3D_Vulkan_app);
}