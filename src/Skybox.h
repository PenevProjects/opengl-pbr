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
	Skybox();
	Skybox(std::vector<std::string> _texturePaths);
	~Skybox();
	Skybox(const Skybox&) = delete;
	Skybox& operator=(const Skybox&) = delete;
	void DrawSkyboxCube();
	unsigned int m_id;
private:
	
	unsigned int m_vao;
	void CreateVAO();
};