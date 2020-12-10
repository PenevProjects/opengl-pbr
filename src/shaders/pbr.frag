#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;


struct Material {
	vec3 ambient;
    vec3 diffuse;
    vec3 specular; 
    sampler2D texture_diffuse;
    sampler2D texture_normal;
    float shininess;
}; 

uniform Material material;



uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{

     // obtain normal from normal map in range [0,1]
    vec3 normal = texture(material.texture_normal, fs_in.TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
   
    // get diffuse color
    vec3 color = texture(material.texture_diffuse, fs_in.TexCoords).rgb;
    // ambient
    vec3 ambient = 0.2 * color;
    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = vec3(1.5) * diff * color;
    // specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.5) * spec;
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}