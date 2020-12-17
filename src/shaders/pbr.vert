#version 330 core
layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_TexCoords;
layout (location = 3) in vec3 in_Tangent;
layout (location = 4) in vec3 in_Bitangent;

out VERT_OUT {
    vec3 FragPos;
	vec3 Normal;
    vec2 TexCoords;
	vec3 Tangent;
	vec3 Bitangent;
} v;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{
    v.FragPos = vec3(u_Model * vec4(in_Position, 1.0));   
    v.TexCoords = in_TexCoords;
	v.Normal = mat3(u_Model) * in_Normal;
	v.Tangent = mat3(u_Model) * in_Tangent;   
	v.Bitangent = mat3(u_Model) * in_Bitangent;
	

    gl_Position = u_Projection * u_View * u_Model * vec4(in_Position, 1.0);
}