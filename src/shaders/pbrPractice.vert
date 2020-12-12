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
    vec3 TangentLightPos[4];
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

uniform vec3 lightPos[4];
uniform vec3 viewPos;

void main()
{
    vs_out.FragPos = vec3(u_Model * vec4(aPos, 1.0));   
    vs_out.TexCoords = aTexCoords;
	vs_out.Normal = mat3(u_Model) * aNormal;   
    
    mat3 normalMatrix = transpose(inverse(mat3(u_Model)));
    vec3 T = normalize(normalMatrix * aTangent);
	vec3 B = normalize(normalMatrix * aBitangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);

    mat3 TBNinverse = transpose(mat3(T, B, N));   //we need the inverse so that we can transform from world to tangent space
	for (int i=0; i < 4; i++)
	{
		vs_out.TangentLightPos[i] = TBNinverse * lightPos[i];
	}
    
    vs_out.TangentViewPos  = TBNinverse * viewPos;
    vs_out.TangentFragPos  = TBNinverse * vs_out.FragPos;
    gl_Position = u_Projection * u_View * u_Model * vec4(aPos, 1.0);
}