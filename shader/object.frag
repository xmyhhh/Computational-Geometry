#version 450

layout (binding = 1) uniform sampler2D shadowMap;

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inViewVec;
layout (location = 3) in vec3 inLightVec;
layout (location = 4) in vec4 inShadowCoord;

layout (constant_id = 0) const int enablePCF = 0;

layout (location = 0) out vec4 outFragColor;

#define ambient 0.1

float textureProj(vec4 shadowCoord, vec2 off)
{
	float shadow = 0.0;
	if ( shadowCoord.z > 0.0 && shadowCoord.z < 1.0 ) 
	{
		
		float dist = texture( shadowMap, shadowCoord.st + off ).r;
		if ( dist < shadowCoord.z ) 
		{
			shadow = 1.0;
		}
	}
	return shadow;
}

float filterPCF(vec4 sc)
{
	ivec2 texDim = textureSize(shadowMap, 0);
	float scale = 1.5;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 1;
	
	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += textureProj(sc, vec2(dx*x, dy*y));
			count++;
		}
	}
	return shadowFactor / count;
}

void main() 
{	
	vec4 inShadowCoord_n = inShadowCoord /inShadowCoord.w;
	inShadowCoord_n.xy = inShadowCoord_n.xy * 0.5 + 0.5;

	float shadow =  textureProj(inShadowCoord_n , vec2(0.0));

	vec3 N = (inNormal);
	vec3 L = (inLightVec);
	vec3 V = (inViewVec);
	vec3 H = normalize(L + V);

	vec3 diffuse = max(dot(N, L), ambient) * inColor;
	vec3 specular = max(pow(dot(H, N), 32), 0) * inColor * 0.25f;

	outFragColor = vec4((specular + diffuse) * (1.0 - shadow) + vec3(0.25) * inColor, 1.0);

}
