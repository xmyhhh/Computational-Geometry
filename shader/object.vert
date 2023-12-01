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
	mat4 lightSpaceMatrix;
	vec4 lightPos;

} ubo;

layout(push_constant) uniform PushConsts {
	mat4 model;
}pushConsts;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec3 outViewVec;
layout (location = 3) out vec3 outLightVec;
layout (location = 4) out vec4 outShadowCoord;

const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 );

void main() 
{
	outColor = inColor;

	gl_Position = ubo.projection * ubo.view * pushConsts.model * vec4(inPos.xyz, 1.0);
	
    vec4 pos = pushConsts.model * vec4(inPos, 1.0);  //world space pos
    outNormal = normalize(mat3(pushConsts.model) * inNormal);   //world space normal

    outLightVec = normalize(ubo.lightPos.xyz - pos.xyz);
    outViewVec = normalize(ubo.camPos.xyz - pos.xyz);			

	outShadowCoord = (ubo.lightSpaceMatrix * pushConsts.model ) * vec4(inPos, 1.0);	

}



