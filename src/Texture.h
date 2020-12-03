#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <string>
#include <vector>

/**
*\brief Controls texture objects.
*
*Can only be initialized through default constructor.
*OpenGL encapsulates glTexture objects within the API. Memory management for glTextures is not controlled by the user -
*this makes copy and copy assignment constructors dangerous to use, so they are deleted.
**/
struct Texture {
	///Texture from path.
	Texture(std::string _path, std::string _typeName, bool _gamma=false);
	///Texture from embedded texture.
	Texture(const aiTexture* _texture);

	~Texture();
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;


	unsigned int m_id;
	std::string m_typeName;
	std::string m_path;
};

