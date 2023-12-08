#version 450

layout (binding = 0) uniform UBO 
{
	mat4 projection;
	mat4 view;
	vec4 camPos;
} ubo;

layout(push_constant) uniform PushConsts {
	vec4 p1;
    vec4 p2;
    vec4 p3;
}pushConsts;


void main() {

    if(gl_VertexIndex==0){
        gl_Position = ubo.projection * ubo.view *  pushConsts.p1;
    }
    else if(gl_VertexIndex==1){
        gl_Position = ubo.projection * ubo.view * pushConsts.p2;
    }
    else{
        gl_Position = ubo.projection * ubo.view * pushConsts.p3;
    }

}