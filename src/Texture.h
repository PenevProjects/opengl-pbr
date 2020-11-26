//#pragma once
//#include <GL/glew.h>
//#include <glm/glm.hpp>
//#include <glm/ext.hpp>
//#include <iostream>
//#include <string>
//
//
///**
//*\brief Controls texture objects.
//*
//*Can only be initialized through default constructor.
//*OpenGL encapsulates glTexture objects within the API. Memory management for glTextures is not controlled by the user -
//*this makes copy and copy assignment constructors dangerous to use, so both are private, to comply with the Rule of Three.
//*/
//class Texture
//{
//private:
//	///Copy constructor is private to avoid resource mismanagement.
//	//Texture(const Texture& other) {};
//	///// Copy assignment constructor is private to avoid resource mismanagement.
//	//Texture& operator=(const Texture& other) {};
//public:
//	GLuint m_id;
//	std::string m_type;
//	std::string m_path;
//	/** 
//	*\brief Initializes texture object with path to an image or bitmap.
//	*
//	*@param path Specifies the path to the texture file.
//	*/
//	Texture(char const * path);
//	/// Custom destructor to clear texture data.
//	~Texture();
//
//};
//
