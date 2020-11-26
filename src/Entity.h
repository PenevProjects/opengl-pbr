#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>


#include "Model.h"
#include "bugl.h"


class Entity 
{
public:
	GLuint m_vao;
	size_t m_numVerts;
	Entity(std::string _path);
	~Entity();
	///Model matrix representing the object's position in world space.
	glm::mat4 modelMatrix;
	
};
