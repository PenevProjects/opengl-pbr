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
	* @param _width Dimension of framebuffer width.
	* @param _height Dimension of framebuffer height.
	**/
	FrameBuffer::FrameBuffer(int _width, int _height);
	~FrameBuffer();
	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(const FrameBuffer&) = delete;

	/**\brief Function for rendering a quad with post-processing effects.
	*
	*Sets uniforms of "screenWidth" and "screenHeight" to member variables m_width and m_height.
	*Renders the m_textureId applied on a screen quad.
	*/
	void DrawRenderTexture(const Shader& _shader);
	unsigned int GetFrameBufferObject() { return m_fbo; }
	unsigned int GetRenderTexture() { return m_textureId; }
	unsigned int GetRenderBufferObject() { return m_rbo; }
private:
	void CreateRenderTexture();
	void CreateRenderBuffer();
	void CreateRenderQuad();

	unsigned int m_screenQuadVAO;
	unsigned int m_fbo;
	unsigned int m_rbo;
	unsigned int m_textureId;
	int m_width;
	int m_height;
};