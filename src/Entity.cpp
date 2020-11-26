#include "Entity.h"
#include "Texture.h"





Entity::Entity(std::string _path) :
	modelMatrix(glm::mat4(1.0))
{
	m_vao = buLoadModel(_path, &m_numVerts);
}

Entity::~Entity()
{
}





