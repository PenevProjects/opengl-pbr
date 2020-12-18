#version 330 core
out vec2 FragColor;
in vec2 TexCoords;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
// Sourced from: http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// Efficient VanDerCorpus calculation(by reversing bits...Magic!)
float RadicalInverse_VdC(uint bits) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
// Quasi-Monte Carlo method
vec2 Hammersley(uint i, uint N)
{
	return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}
// ----------------------------------------------------------------------------
//Importance sampling: Generates a sample vector that's biased towards the preferred alignment direction N
vec3 ImportanceSampleGGX(vec2 _Xi, vec3 _N, float _roughness)
{
	float a = _roughness  *_roughness;
	
	float phi = 2.0 * PI * _Xi.x;
	float cosTheta = sqrt((1.0 - _Xi.y) / (1.0 + (a*a - 1.0) * _Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
	// from spherical coordinates to cartesian coordinates - halfway vector
	vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;
	
	// from tangent-space H vector to world-space sample vector
	vec3 up          = abs(_N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangentX   = normalize(cross(up, _N));
	vec3 tangentY = cross(_N, tangentX);
	
	return normalize(tangentX * H.x + tangentY * H.y + _N * H.z);
}
// ----------------------------------------------------------------------------
//Geometry function: approximates the relative surface area where micro surface details overshadow each other
float GeometrySchlickGGX(float _NdotV, float _roughness)
{
    // note that it's different k for IBL
    float a = _roughness;
    float k = (a * a) / 2.0;

    return _NdotV / (_NdotV * (1.0 - k) + k);
}
// ----------------------------------------------------------------------------
//Geometry sum function: To account for both Viewing vector(geometry obstruction) and Light vector(geometry shadowing)
float GeometrySmith(vec3 _N, vec3 _V, vec3 _L, float _roughness)
{
    float NdotV = max(dot(_N, _V), 0.0);
    float NdotL = max(dot(_N, _L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, _roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, _roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
//Make BRDF of specular reflection a Look-Up Texture
vec2 IntegrateBRDF(float _NdotV, float _roughness)
{
    vec3 V;
    V.x = sqrt(1.0 - _NdotV*_NdotV);
    V.y = 0.0;
    V.z = _NdotV;

    float A = 0.0;
    float B = 0.0; 

    vec3 N = vec3(0.0, 0.0, 1.0);
    
    const uint num_samples = 1024u;
    for(uint i = 0u; i < num_samples; ++i)
    {
        // generates a sample vector that's biased towards the
        // preferred alignment direction (importance sampling).
        vec2 Xi = Hammersley(i, num_samples);
        vec3 H = ImportanceSampleGGX(Xi, N, _roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if(NdotL > 0.0)
        {
            float G = GeometrySmith(N, V, L, _roughness);
            float G_Vis = (G * VdotH) / (NdotH * _NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    A /= float(num_samples);
    B /= float(num_samples);
    return vec2(A, B);
}
// ----------------------------------------------------------------------------
void main() 
{
    vec2 integratedBRDF = IntegrateBRDF(TexCoords.x, TexCoords.y);
    FragColor = integratedBRDF;
}