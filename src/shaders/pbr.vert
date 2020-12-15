#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 FragPos;
	vec3 Normal;
    vec2 TexCoords;
	vec3 Tangent;
	vec3 Bitangent;
} vs_out;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

uniform vec3 viewPos;

void main()
{
    vs_out.FragPos = vec3(u_Model * vec4(aPos, 1.0));   
    vs_out.TexCoords = aTexCoords;
	vs_out.Normal = mat3(u_Model) * aNormal;
	vs_out.Tangent = mat3(u_Model) * aTangent;   
	vs_out.Bitangent = mat3(u_Model) * aBitangent;   
	

    gl_Position = u_Projection * u_View * u_Model * vec4(aPos, 1.0);
}