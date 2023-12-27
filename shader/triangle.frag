#version 450

layout(location = 0) out vec4 outColor;
layout (location = 0) in vec4 inColor; 




void main() 
{	
	if(inColor.x>1){
		outColor = vec4(inColor.x/255., inColor.y/255., inColor.z/255.,  1.);
	}
	else{
		outColor = vec4(inColor.x, inColor.y, inColor.z, 1.);
	}
}
