#version 450



layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec4 inColor;



layout (location = 0) out vec4 outFragColor;

#define ambient 0.1


void main() 
{	
	if(inColor.x>1){
		outFragColor = vec4(inColor.x/255., inColor.y/255., inColor.z/255.,  0.9);
	}
	else{
		outFragColor =  vec4(inColor.x., inColor.y., inColor.z., 0.9);;
	}
	
}
