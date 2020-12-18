#version 330 core
out vec4 FragColor;

//Reference: LearnOpenGL.com
//PBR Based on the reflectance equation with a Cook-Torrance BRDF.

in VERT_OUT {
    vec3 FragPos;
	vec3 Normal;
    vec2 TexCoords;
	vec3 Tangent;
	vec3 Bitangent;
} v;

uniform samplerCube u_irradianceMap;
uniform samplerCube u_prefilterMap;
uniform sampler2D u_brdfLUT;

struct Material {
    sampler2D texture_albedo;
    sampler2D texture_normal;
	sampler2D texture_roughness;
	sampler2D texture_metallic;
	sampler2D texture_ao;
}; 

uniform Material material;


// lights
uniform vec3 u_lightPos[3];
uniform vec3 u_lightColors[3];
uniform vec3 u_viewPos;

const float PI = 3.14159265359;

// ----------------------------------------------------------------------------
vec3 getNormalFromMap(sampler2D _normalMap)
{
    vec3 tangentNormal = texture(_normalMap, v.TexCoords).rgb * 2.0 - 1.0;
    vec3 N   = normalize(v.Normal);
	vec3 T = normalize(v.Tangent);
	vec3 B = normalize(v.Bitangent);
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
//F0 is specular reflectance at normal incidence, cosTheta is incident angle
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
// from: https://seblagarde.wordpress.com/2011/08/17/hello-world/
//F0 is specular reflectance at normal incidence, cosTheta is incident angle
vec3 fresnelSchlickRoughness(float _cosTheta, vec3 _F0, float _roughness)
{
    return _F0 + (max(vec3(1.0 - _roughness), _F0) - _F0) * pow(max(1.0 - _cosTheta, 0.0), 5.0);
} 
// ----------------------------------------------------------------------------
//Normal distribution function: approximates the relative surface area of microfacets exactly aligned to the (halfway) vector H.
float DistributionGGX(vec3 _N, vec3 _H, float _roughness)
{
	//square as per empirical observations of Disney
    float a = _roughness * _roughness;
    float a2     = a*a;
    float NdotH  = max(dot(_N, _H), 0.0);
	
    float num   = a2;
    float denom = (NdotH*NdotH * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}
// ----------------------------------------------------------------------------
//Geometry function: approximates the relative surface area where micro surface details overshadow each other
float GeometrySchlickGGX(float _NdotV, float _roughness)
{
	//as described in "Specular G" from Unreal's notes,
	//reduce hotness by remapping roughness
    float alpha = (_roughness + 1.0);
    float k = (alpha*alpha) / 8.0;
	
    return _NdotV / (_NdotV * (1.0 - k) + k);
}
// ----------------------------------------------------------------------------
//Geometry sum function: To account for both Viewing vector(geometry obstruction) and Light vector(geometry shadowing)
float GeometrySmith(vec3 _N, vec3 _V, vec3 _L, float _roughness)
{
    float NdotV = max(dot(_N, _V), 0.0);
    float NdotL = max(dot(_N, _L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, _roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, _roughness);
	
    return ggx1 * ggx2;
}


void main()
{
	vec3 albedo     = pow(texture(material.texture_albedo, v.TexCoords).rgb, vec3(2.2)); //gamma-correct sRGB to linear
    vec3 normals    = getNormalFromMap(material.texture_normal);  //have to be converted from tangent space to world space
	float roughness = texture(material.texture_roughness, v.TexCoords).r; //greyscale
	float metallic  = texture(material.texture_metallic, v.TexCoords).r; //greyscale
	float ao        = texture(material.texture_ao, v.TexCoords).r; //greyscale

    // transform normal vector to range [-1,1]
    vec3 N = normals;
    vec3 V = normalize(u_viewPos - v.FragPos); //viewing vector(tangent space)
	vec3 R = reflect(-V, N);

	vec3 F0 = vec3(0.04); //reflectance ratio at normal incidence
	F0 = mix(F0, albedo, metallic); //default value of dia-electrics is 0.04, for metallics lerp over to albedo using metallic map

	//for reflectance equation
	vec3 Lo = vec3(0.0);

	//---------------
	//Process direct lighting. Calculate for each light object in the scene. 
	for(int i = 0; i < u_lightPos.length(); i++) 
	{
		vec3 L = normalize(u_lightPos[i] - v.FragPos); //light vector
		vec3 H = normalize(V + L); //halfway vector
  
		float distanceLtoPos = length(u_lightPos[i] - v.FragPos);
		float attenuation = 1.0 / (distanceLtoPos * distanceLtoPos);
		vec3 radiance = u_lightColors[i] * attenuation; 


		//calculate values necessary for final reflectance equation
		float NDF = DistributionGGX(N, H, roughness); 
		float G   = GeometrySmith(N, V, L, roughness); 
		vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

		//calculate Cook-Torrance BRDF
		vec3 num    = NDF * G * F;
		float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		vec3 specular     = num / max(denom, 0.001);  

		vec3 kS = F; //reflection is the fresnel
		vec3 kD = vec3(1.0) - kS; //refraction is exclusive with reflectance
  
		kD *= 1.0 - metallic; //refraction value

		float NdotL = max(dot(N, L), 0.0);
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}

	// IBL Diffuse environment lighting
	// ----------------------------
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
	vec3 irradiance = texture(u_irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;
	// IBL Specular sample both the pre-filter map and the BRDF lut and combine them together - Split-Sum from Karis's notes to get the IBL specular part.
	// -----------------------
	//define max level of detail of the roughness map used. this is MIP COUNT when calling the prefiltering shader.
    const float maxReflectionLOD = 4.0;
    vec3 prefilteredColor = textureLod(u_prefilterMap, R,  roughness * maxReflectionLOD).rgb;    
    vec2 brdf  = texture(u_brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * ao;
	vec3 color = ambient + Lo; 

	// HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}

