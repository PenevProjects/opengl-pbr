#include "Skybox.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "Shader.h"

#include <iostream>


Skybox::Skybox(std::string _pathToHDR, int _size, unsigned int _samplerID)
{
	//setup skybox vao
	CreateCubeVAO();

	// setup member vars
	// ---------------------------------
	m_environmentMap = std::make_shared<Texture>(_size, _size, "cubemap");

	unsigned int irradianceSize = 32;
	m_irradiance = std::make_shared<Texture>(irradianceSize, irradianceSize, "cubemap");

	unsigned int mipSize = 128;
	m_prefilterMap = std::make_shared<Texture>(mipSize, mipSize, "cubemapMip");




	//setup vars
	std::unique_ptr<Shader> equirectToCubemapShader = std::make_unique<Shader>("../src/shaders/cubemap.vert", "../src/shaders/equirectangular-to-cubemap.frag");
	std::unique_ptr<Shader> irradianceConvShader = std::make_unique<Shader>("../src/shaders/cubemap.vert", "../src/shaders/cubemap-conv.frag");
	std::unique_ptr<Shader> prefilterConvShader = std::make_unique<Shader>("../src/shaders/cubemap.vert", "../src/shaders/prefilter-conv.frag");
	std::unique_ptr<FrameBuffer> environmentFramebuffer = std::make_unique<FrameBuffer>(_size, _size);
	std::shared_ptr<Texture> hdrTexture = std::make_shared<Texture>(_pathToHDR, "HDR");

	// set up projection and view matrices for capturing data onto the 6 cubemap face directions
	// ----------------------------------------------------------------------------------------------
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};
	//set static shader uniform variables
	equirectToCubemapShader->Use();
	equirectToCubemapShader->setInt("equirectangularMap", _samplerID);
	equirectToCubemapShader->setMat4("u_Projection", captureProjection);

	//convert HDR equirectangular map to cubemap
	// ----------------------------------------------------------------------
	glActiveTexture(GL_TEXTURE0 + _samplerID);
	glBindTexture(GL_TEXTURE_2D, hdrTexture->m_id);

	// configure the viewport to the capture dimensions.
	glViewport(0, 0, _size, _size);

	glBindFramebuffer(GL_FRAMEBUFFER, environmentFramebuffer->GetFrameBufferObject());
	for (unsigned int i = 0; i < 6; ++i)
	{ //render all 6 faces of cube going through the view matrices and store in member texture id
		equirectToCubemapShader->setMat4("u_View", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_environmentMap->m_id, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
	// ----------------------------------------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, environmentFramebuffer->GetFrameBufferObject());
	glBindRenderbuffer(GL_RENDERBUFFER, environmentFramebuffer->GetRenderBufferObject());
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, irradianceSize, irradianceSize);

	// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
	// -----------------------------------------------------------------------------
	irradianceConvShader->Use();
	irradianceConvShader->setInt("environmentCubemap", _samplerID);
	irradianceConvShader->setMat4("u_Projection", captureProjection);
	glActiveTexture(GL_TEXTURE0 + _samplerID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentMap->m_id);

	glViewport(0, 0, irradianceSize, irradianceSize); // configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, environmentFramebuffer->GetFrameBufferObject());
	for (unsigned int i = 0; i < 6; ++i)
	{
		irradianceConvShader->setMat4("u_View", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_irradiance->m_id, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
	// ----------------------------------------------------------------------------------------------------
	prefilterConvShader->Use();
	prefilterConvShader->setInt("environmentCubemap", _samplerID);
	prefilterConvShader->setMat4("u_Projection", captureProjection);
	glActiveTexture(GL_TEXTURE0 + _samplerID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentMap->m_id);

	glBindFramebuffer(GL_FRAMEBUFFER, environmentFramebuffer->GetFrameBufferObject());
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; mip++)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = mipSize * std::pow(0.5, mip);
		unsigned int mipHeight = mipSize * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, environmentFramebuffer->GetRenderBufferObject());
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterConvShader->setFloat("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterConvShader->setMat4("u_View", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_prefilterMap->m_id, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			RenderCube();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Skybox::~Skybox()
{
	glDeleteVertexArrays(1, &m_vao);
}

void Skybox::CreateCubeVAO()
{
	float skyboxVertexData[] = {
		// positions, don't need texture coordinates because cube is always at origin, position coordinates are also direction vectors(which can be used to sample the texture)         
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

	unsigned int skyboxVBO;
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(m_vao);

	//setup quad vao
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertexData), &skyboxVertexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0);
	glDeleteBuffers(1, &skyboxVBO);
}

void Skybox::RenderCube()
{
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}