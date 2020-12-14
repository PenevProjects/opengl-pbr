#include "FrameBuffer.h"
#include "Shader.h"
#include <iostream>


FrameBuffer::FrameBuffer(int _width, int _height) :
	m_width(_width),
	m_height(_height)
{
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	CreateRenderQuad();
	CreateRenderTexture();
	CreateRenderBuffer();
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	
}

FrameBuffer::~FrameBuffer()
{
	if (m_textureId) { glDeleteTextures(1, &m_textureId); }
	if (m_screenQuadVAO) { glDeleteVertexArrays(1, &m_screenQuadVAO); }
	if (m_rbo) { glDeleteRenderbuffers(1, &m_rbo); }
	if (m_fbo) { glDeleteFramebuffers(1, &m_fbo); }
}

void FrameBuffer::CreateRenderTexture()
{
	glGenTextures(1, &m_textureId);
	glBindTexture(GL_TEXTURE_2D, m_textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureId, 0);
}
void FrameBuffer::CreateRenderBuffer()
{
	glGenRenderbuffers(1, &m_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
}

void FrameBuffer::CreateRenderQuad()
{
	//screen quad vertices
	float quadVertexData[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
	//// screen quad VAO
	unsigned int quadVBO;
	glGenVertexArrays(1, &m_screenQuadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(m_screenQuadVAO);

	//setup quad vao
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertexData), &quadVertexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);
	glDeleteBuffers(1, &quadVBO);
}

void FrameBuffer::DrawRenderTextureQuad()
{
	glBindVertexArray(m_screenQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}