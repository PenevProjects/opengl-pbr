#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Shader;

class FrameBuffer {
public:
	/** \brief Framebuffer ctor with default dimensions of 800x600.
	*
	* Constructs with a FrameBuffer object and creates its render texture and render buffer object.
	* @param _width Dimension of framebuffer width. Default 800.
	* @param _height Dimension of framebuffer height. Default 600.
	**/
	FrameBuffer::FrameBuffer(int _width = 800, int _height = 600);
	~FrameBuffer();
	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(const FrameBuffer&) = delete;

	void DrawRenderTexture(const Shader& _shader);
	unsigned int GetID() { return m_fbo; }
	unsigned int GetRenderTexture() { return m_textureId; }
	unsigned int GetRenderBufferObject() { return m_rbo; }
private:
	void CreateRenderTexture(int _width, int _height);
	void CreateRenderBuffer(int _width, int _height);
	void CreateRenderQuad();

	unsigned int m_screenQuadVAO;
	unsigned int m_fbo;
	unsigned int m_rbo;
	unsigned int m_textureId;
	int m_width;
	int m_height;
};