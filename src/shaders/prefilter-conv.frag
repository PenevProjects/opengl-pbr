#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube u_environmentCubemap;
uniform float u_roughness;
uniform float u_resolution;

const float PI = 3.14159265359;

//prefiltering sourced from: https://developer.nvidia.com/gpugems/gpugems3/part-iii-rendering/chapter-20-gpu-based-importance-sampling


float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
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
	float a = _roughness * _roughness;
	
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
// Doing this for 5 different resolutions!
void main()
{		
    vec3 N = normalize(WorldPos);
    
    // N = V = R
    vec3 R = N;
    vec3 V = R;

    const uint num_samples = 1024u;
    vec3 prefilteredColor = vec3(0.0);
    float totalWeight = 0.0;
    
    for(uint i = 0u; i < num_samples; ++i)
    {
        // generates a sample vector that's biased towards the preferred alignment direction (importance sampling).
        vec2 Xi = Hammersley(i, num_samples);
        vec3 H = ImportanceSampleGGX(Xi, N, u_roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if(NdotL > 0.0)
        {
            // sample from the environment's mip level based on roughness/pdf
            float D   = DistributionGGX(N, H, u_roughness);
            float NdotH = max(dot(N, H), 0.0);
            float HdotV = max(dot(H, V), 0.0);
            float pdf = max(D * NdotH / (4.0 * HdotV), 0.01); 

			// Solid angle of a sample is 1 of the total number of samples in the probability density function
            float omegaS = 1.0 / (float(num_samples) * pdf);

			// the solid angle of a texel in relation to the hemisphere can be found as:
			// distortion factor = the rate of change from a unit of area on the hemisphere / a unit of area on the texture.
			// 4.0 * PI = Total area of Sphere(ds)
			// 6.0 * res * res = Total area of Cubemap(dc)
			// Solid angle of a texel = ds / dc
            float omegaP  = 4.0 * PI / (6.0 * u_resolution * u_resolution);


			// if roughness is other than 0, use pre-calculated mip level.
			// nvidia's formula uses 0.5 rather than 2, but dots still persist in my specular reflections using that.
			// I think specular reflections that are more sensitive to roughness are better than the opposite, as
			// a blurry reflection looks more "real-life" than a noisy one.
            float mipLevel = u_roughness == 0.0 ? 0.0 : (2 * log2(omegaS / omegaP));
			

			//sample environment cubemap with miplevel as lod(higher is more distorted).
            prefilteredColor += textureLod(u_environmentCubemap, L, mipLevel).rgb * NdotL;
            totalWeight      += NdotL;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;

    FragColor = vec4(prefilteredColor, 1.0);
}