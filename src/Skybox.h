#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <vector>

struct Texture;

class Skybox
{
	//friend class Texture;
public:
	Skybox(std::vector<std::string> _texturePaths);
	~Skybox();
	Skybox(const Skybox&) = delete;
	Skybox& operator=(const Skybox&) = delete;
	void DrawSkybox();
private:
	unsigned int m_id;
	unsigned int m_vao;
	void CreateVAO();
};