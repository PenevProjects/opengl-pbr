#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube u_environmentCubemap;

void main()
{		
    vec3 envColor = texture(u_environmentCubemap, WorldPos).rgb;
    //vec3 envColor = textureLod(environmentCubemap, WorldPos, 3).rgb; //FOR MIPMAP TESTING
    // HDR tonemap and gamma correct
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 
    
    FragColor = vec4(envColor, 1.0);
}