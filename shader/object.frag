#version 450



layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;



layout (location = 0) out vec4 outFragColor;

#define ambient 0.1


void main() 
{	

	outFragColor = vec4(1.0);

}
