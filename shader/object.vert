#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec3 inNormal;

layout (binding = 0) uniform UBO 
{
	mat4 projection;
	mat4 view;
	vec4 camPos;
} ubo;

layout(push_constant) uniform PushConsts {
	mat4 model;
	vec4 color;
}pushConsts;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec4 outColor;

void main() 
{

	gl_Position = ubo.projection * ubo.view * pushConsts.model * vec4(inPos.xyz, 1.0);
	
    vec4 pos = pushConsts.model * vec4(inPos, 1.0);  //world space pos

    outNormal = normalize(mat3(pushConsts.model) * inNormal);   //world space normal
	outColor = pushConsts.color;
}



