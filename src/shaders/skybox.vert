#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 u_Projection;
uniform mat4 u_View;

out vec3 WorldPos;

void main()
{
    WorldPos = aPos;
	//remove translation part of view matrix (w, which is the term used as an "axis" around we transform the 3d local space)
	mat4 rotView = mat4(mat3(u_View)); 
	vec4 clipPos = u_Projection * rotView * vec4(WorldPos, 1.0);

	gl_Position = clipPos.xyww;
}