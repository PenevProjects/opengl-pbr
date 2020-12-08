#version 330 core
out vec4 FragColor;

struct Material {
	vec3 ambient;
    vec3 diffuse;
    vec3 specular; 
    sampler2D texture_diffuse;
    sampler2D texture_specular;    
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float constant;
    float linear;
    float quadratic;
};

in vec3 v_FragPos;  
in vec3 v_Normal;  
in vec2 v_TexCoords;
  
uniform vec3 viewPosition;
uniform Material material;
uniform Light light;
uniform bool hasTextures;

void main()
{
	float light_distance = length(light.position - v_FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * light_distance + 
    		    light.quadratic * (light_distance * light_distance));    

	vec3 N = normalize(v_Normal); //normal
    vec3 L = normalize(light.position - v_FragPos); //light
	vec3 E = normalize(viewPosition - v_FragPos); //eye(view)
    vec3 R = reflect(L, N); //reflection

    // ambient
    vec3 ambient;
	if (hasTextures) {
		ambient = light.ambient * material.ambient * texture(material.texture_diffuse, v_TexCoords).rgb * attenuation;
	}
	else {
		ambient = light.ambient * material.ambient * attenuation;
	}
  	
    // diffuse 

    float diff = max(dot(N, L), 0.0);
	vec3 diffuse;
	if (hasTextures) {
		diffuse = light.diffuse * diff * material.diffuse * texture(material.texture_diffuse, v_TexCoords).rgb * attenuation;  
	}
	else {
		diffuse = light.diffuse * diff * material.diffuse * attenuation;
	}
    // specular
	float spec = pow(max(dot(E, R), 0.0), material.shininess);
	vec3 specular;
	if (hasTextures) {
		specular = light.specular * spec * material.specular * texture(material.texture_specular, v_TexCoords).rgb * attenuation;
	}
	else {
		specular = light.specular * spec * material.specular * attenuation;
	}
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
} 