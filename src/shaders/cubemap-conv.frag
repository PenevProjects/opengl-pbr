#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube u_environmentCubemap;

const float PI = 3.14159265359;

void main()
{
	//This shader convolves over a hemisphere's lighting information and output an irradiance(total radiance over an area).
    vec3 N = normalize(WorldPos); //basically used as Wi as the vector around which we integrate for a hemisphere

    vec3 irradiance = vec3(0.0);   
    
    // tangent space calculation from origin point, or in other words:
	// calculate directions in relation to a "normal"
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, N);
    up = cross(N, right);
       
	// step is 1/50
    float sampleDelta = 0.02;
    float nrSamples = 0.0;

	//Spherical coordinates are used for the integration over the directions in the hemisphere
	//this is because using solid angles in a Cartesian system would be too complex.
	//Spherical coordinates are natively in tangent space, so we need to account for this.

	//phi is polar azimuth(measures parallels of sphere, like the equator)
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
		//theta is the inclination zenith (measures meridians of sphere, 
		//goes only to 0.5PI because we are sampling a hemisphere in the horizontal direction)
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // spherical coords to cartesian coords (in tangent space, or "in relation to a normal")
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world (from "in relation to a normal" to "in relation to vertex positions"
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; 
			//sample environment cube at the given coordinate
            irradiance += texture(u_environmentCubemap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
	//we need the average irradiance in the hemisphere, so take its mean average
    irradiance = PI * irradiance * (1.0 / float(nrSamples));
    
    FragColor = vec4(irradiance, 1.0);
}