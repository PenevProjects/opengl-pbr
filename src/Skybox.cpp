#include "Skybox.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "Shader.h"

#include <iostream>


Skybox::Skybox(std::string _path, unsigned int _size) :
	m_path{ _path },
	m_size{ _size },
	m_captureProjection{ glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f) },
	m_captureView{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)) }
{
	//setup skybox vao
	CreateCubeVAO();	

	// setup member vars
	// ---------------------------------
	m_environmentMap = std::make_shared<Texture>(m_size, m_size, "cubemap");

	//for the current demo, this is a good trade-off between performance and visuals.
	unsigned int reflectionSize = 256;
	m_prefilterMap = std::make_shared<Texture>(reflectionSize, reflectionSize, "cubemapPrefilter");


	//setup vars
	std::unique_ptr<Shader> equirectToCubemapShader = std::make_unique<Shader>("../src/shaders/cubemap.vert", "../src/shaders/equirectangular-to-cubemap.frag");
	std::unique_ptr<Shader> irradianceConvShader = std::make_unique<Shader>("../src/shaders/cubemap.vert", "../src/shaders/cubemap-conv.frag");
	std::unique_ptr<Shader> prefilterConvShader = std::make_unique<Shader>("../src/shaders/cubemap.vert", "../src/shaders/prefilter-conv.frag");
	std::unique_ptr<Shader> brdfLutShader = std::make_unique<Shader>("../src/shaders/framebuf-quad.vert", "../src/shaders/brdf-lut.frag");
	std::unique_ptr<FrameBuffer> environmentFramebuffer = std::make_unique<FrameBuffer>(m_size, m_size);
	std::shared_ptr<Texture> hdrTexture = std::make_shared<Texture>(m_path, "HDR");

	// set up projection and view matrices for capturing data onto the 6 cubemap face directions
	// ----------------------------------------------------------------------------------------------

	//set static shader uniform variables
	equirectToCubemapShader->Use();
	equirectToCubemapShader->setInt("equirectangularMap", 0);
	equirectToCubemapShader->setMat4("u_Projection", m_captureProjection);

	//convert HDR equirectangular map to cubemap
	// ----------------------------------------------------------------------
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture->m_id);

	// configure the viewport to the capture dimensions.
	glViewport(0, 0, m_size, m_size);

	glBindFramebuffer(GL_FRAMEBUFFER, environmentFramebuffer->GetFrameBufferObject());
	for (unsigned int i = 0; i < 6; ++i)
	{ //render all 6 faces of cube going through the view matrices and store in member texture id
		equirectToCubemapShader->setMat4("u_View", m_captureView[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_environmentMap->m_id, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//generate mipmap levels for base environment map after it has been generated for prefilter convolution
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentMap->m_id);
	//enable trilinear filtering
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);


	// create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
	// ----------------------------------------------------------------------
	unsigned int irradianceSize = 32;
	m_irradiance = std::make_shared<Texture>(irradianceSize, irradianceSize, "cubemap");
	glBindFramebuffer(GL_FRAMEBUFFER, environmentFramebuffer->GetFrameBufferObject());
	glBindRenderbuffer(GL_RENDERBUFFER, environmentFramebuffer->GetRenderBufferObject());
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, irradianceSize, irradianceSize);

	// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
	// -----------------------------------------------------------------------------
	irradianceConvShader->Use();
	irradianceConvShader->setInt("u_environmentCubemap", 0);
	irradianceConvShader->setMat4("u_Projection", m_captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentMap->m_id);
	glViewport(0, 0, irradianceSize, irradianceSize); // configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, environmentFramebuffer->GetFrameBufferObject());
	for (unsigned int i = 0; i < 6; ++i)
	{
		irradianceConvShader->setMat4("u_View", m_captureView[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_irradiance->m_id, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
	// ----------------------------------------------------------------------------------------------------

	prefilterConvShader->Use();
	prefilterConvShader->setInt("u_environmentCubemap", 0);
	prefilterConvShader->setMat4("u_Projection", m_captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentMap->m_id);
	//turn on trilinear filtering for fixing of dots
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, environmentFramebuffer->GetFrameBufferObject());
	//256 * pow(0.5, 5) = 8x8 is the smallest mipmap we can sample
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; mip++)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = reflectionSize * std::pow(0.5, mip);
		unsigned int mipHeight = reflectionSize * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, environmentFramebuffer->GetRenderBufferObject());
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterConvShader->setFloat("u_roughness", roughness);
		prefilterConvShader->setFloat("u_resolution", reflectionSize);
		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterConvShader->setMat4("u_View", m_captureView[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_prefilterMap->m_id, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			RenderCube();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//brdf look-up texture setup
	// --------------------------
	m_brdfLookUpTexture = std::make_shared<Texture>(m_size, m_size, "brdfLUT");
	glBindTexture(GL_TEXTURE_2D, m_brdfLookUpTexture->m_id);
	// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
	glBindFramebuffer(GL_FRAMEBUFFER, environmentFramebuffer->GetFrameBufferObject());
	glBindRenderbuffer(GL_RENDERBUFFER, environmentFramebuffer->GetRenderBufferObject());
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_size, m_size);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_brdfLookUpTexture->m_id, 0);

	glViewport(0, 0, m_size, m_size);
	brdfLutShader->Use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	environmentFramebuffer->DrawRenderTextureQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

Skybox::~Skybox()
{
	glDeleteVertexArrays(1, &m_vao);
}

void Skybox::CreateCubeVAO()
{
	float cubeVertexData[] = {
		// positions, don't need texture coordinates because cube is always at origin
		// which makes position coordinates also direction vectors(which can be used to sample a cubemap texture)         
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	unsigned int cubeVBO;
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(m_vao);

	//setup quad vao
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexData), &cubeVertexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0);
	glDeleteBuffers(1, &cubeVBO);
}

void Skybox::RenderCube()
{
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}



