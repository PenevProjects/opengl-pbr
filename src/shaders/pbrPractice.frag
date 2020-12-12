#version 330 core
out vec4 FragColor;

//A lot referenced from: LearnOpenGL.com

in VS_OUT {
    vec3 FragPos;
	vec3 Normal;
    vec2 TexCoords;
    vec3 TangentLightPos[4];
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform samplerCube irradianceMap;

struct Material {
	vec3 ambient;
    vec3 diffuse;
    vec3 specular; 
    sampler2D texture_albedo;
    sampler2D texture_normal;
	sampler2D texture_roughness;
	sampler2D texture_metallic;
	sampler2D texture_ao;
}; 

uniform Material material;


// lights
uniform vec3 lightPos[4];
uniform vec3 lightColors[4];
uniform vec3 viewPos;

const float PI = 3.14159265358979323846264338327950;


vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(material.texture_normal, fs_in.TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fs_in.FragPos);
    vec3 Q2  = dFdy(fs_in.FragPos);
    vec2 st1 = dFdx(fs_in.TexCoords);
    vec2 st2 = dFdy(fs_in.TexCoords);

    vec3 N   = normalize(fs_in.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

void main()
{
	//convert to linear space first
	vec3 albedo     = pow(texture(material.texture_albedo, fs_in.TexCoords).rgb, vec3(2.2));
    vec3 normals    = texture(material.texture_normal, fs_in.TexCoords).rgb; 
	float roughness = texture(material.texture_roughness, fs_in.TexCoords).r;
	float metallic  = texture(material.texture_metallic, fs_in.TexCoords).r;
	float ao        = texture(material.texture_ao, fs_in.TexCoords).r;

    // transform normal vector to range [-1,1]
    vec3 N = normalize(normals * 2.0 - 1.0);  //normal vector(tangent space)
    vec3 V = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos); //viewing vector(tangent space)


	vec3 F0 = vec3(0.04); //reflectance ratio at normal incidence
	F0 = mix(F0, albedo, metallic); //default value of dia-electrics is 0.04, for metallics lerp over to albedo using metallic map

	//for reflectance equation
	vec3 Lo = vec3(0.0);

	for(int i = 0; i < 4; i++) 
	{
		vec3 L = normalize(fs_in.TangentLightPos[i] - fs_in.TangentFragPos); //light vector (tangent space)
		vec3 H = normalize(V + L); //halfway vector(tangent space)
  
		float distanceLtoPos = length(lightPos[i] - fs_in.FragPos);
		float attenuation = 1.0 / (distanceLtoPos * distanceLtoPos);
		vec3 radiance = lightColors[i] * attenuation; 



		float NDF = DistributionGGX(N, H, roughness); //texture is greyscale so we can take .r     
		float G   = GeometrySmith(N, V, L, roughness); //texture is greyscale so we can take .r     
		vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

		
		vec3 numerator    = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		vec3 specular     = numerator / max(denominator, 0.001);  

		vec3 kS = F; //reflection is the fresnel
		vec3 kD = vec3(1.0) - kS; //refraction is exclusive with reflectance
  
		kD *= 1.0 - metallic; //refraction value

		float NdotL = max(dot(N, L), 0.0);        
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}

	//IBL environment lighting
    vec3 kS = fresnelSchlick(max(dot(N, V), 0.0), F0);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
	vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;
    vec3 ambient = (kD * diffuse) * ao;
	//vec3 ambient = vec3(0.05) * albedo * ao;
	//YOU ARE TRYING TO ACCESS TEXTURE UNIT 0 FOR BOTH A 2D TEXTURE AND 3D TEXTURE AT THE SAME TIME!
	vec3 color = ambient + Lo; 

	// HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}

