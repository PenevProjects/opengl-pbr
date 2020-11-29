#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;    
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 v_FragPos;  
in vec3 v_Normal;  
in vec2 v_TexCoords;
  
uniform vec3 viewPosition;
uniform Material material;
uniform Light light;

void main()
{
	vec3 N = normalize(v_Normal); //normal
    vec3 L = normalize(light.position - v_FragPos); //light
	vec3 E = normalize(viewPosition - v_FragPos); //eye(view)
    vec3 R = reflect(L, N); //reflection

    // ambient
    vec3 ambient = light.ambient * texture(material.texture_diffuse1, v_TexCoords).rgb;
  	
    // diffuse 

    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, v_TexCoords).rgb;  
    // specular
	float spec = pow(max(dot(E, R), 0.0), material.shininess);
	vec3 specular = light.specular * spec * texture(material.texture_specular1, v_TexCoords).rgb;
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
} 