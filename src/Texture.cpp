#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::~Texture()
{
	glDeleteTextures(1, &m_id);
}

Texture::Texture(std::string _path, std::string _typeName, bool _gamma) :
	m_typeName(_typeName),
	m_path(_path)
{
	if (m_typeName != "hdr" && m_typeName != "HDR")
	{
		glGenTextures(1, &m_id);

		//stbi implementation
		int width, height, components;
		unsigned char *data = stbi_load(m_path.c_str(), &width, &height, &components, 0);
		if (data)
		{
			GLenum format;
			GLenum internalFormat;
			if (components == 1)
			{
				format = GL_RED;
				internalFormat = GL_RED;
			}
			else if (components == 3)
			{
				internalFormat = _gamma ? GL_SRGB : GL_RGB;
				format = GL_RGB;
			}
			else if (components == 4)
			{
				internalFormat = _gamma ? GL_SRGB_ALPHA : GL_RGB;
				format = GL_RGBA;
			}

			glBindTexture(GL_TEXTURE_2D, m_id);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

			//gl parameters
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
		{
			std::cout << "Texture data failed to load at path: " << _path << std::endl;
		}
	}
	else
	{// hdr texture
		glGenTextures(1, &m_id);
		//stbi implementation
		stbi_set_flip_vertically_on_load(true);
		int width, height, components;
		float *data = stbi_loadf(m_path.c_str(), &width, &height, &components, 0);
		if (data)
		{
			glBindTexture(GL_TEXTURE_2D, m_id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			stbi_image_free(data);
			stbi_set_flip_vertically_on_load(false);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
		{
			std::cout << "Texture data failed to load at path: " << m_path << std::endl;
		}
	}

}

Texture::Texture(const aiTexture* texture, std::string _typeName, bool _gamma)
{
	m_typeName = _typeName;
	glGenTextures(1, &m_id);
	//stbi_set_flip_vertically_on_load(1);
	unsigned char *data = nullptr;
	int width, height, components;
	if (texture->mHeight == 0)
	{ //if texture is compressed into a continuous stream
		data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &components, 0);
	}
	else
	{ //if texture is raw image data
		data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth * texture->mHeight, &width, &height, &components, 0);
	}
	if (data)
	{
		GLenum format;
		GLenum internalFormat;
		if (components == 1)
		{
			format = GL_RED;
			internalFormat = GL_RED;
		}
		else if (components == 3)
		{
			internalFormat = _gamma ? GL_SRGB : GL_RGB;
			format = GL_RGB;
		}
		else if (components == 4)
		{
			internalFormat = _gamma ? GL_SRGB_ALPHA : GL_RGB;
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, m_id);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		std::cout << "Embedded texture failed to load." << std::endl;
	}

}

unsigned int Texture::LoadCubemap(std::vector<std::string> _textureFaces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < _textureFaces.size(); i++)
	{
		unsigned char *data = stbi_load(_textureFaces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << _textureFaces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

Texture::Texture(int _width, int _height, std::string _mode)
{
	if (_mode == "cubemap")
	{
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, _width, _height, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	}
	else if (_mode == "cubemapTrilinear")
	{
		//enable trilinear filtering to use for prefiltering.
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, _width, _height, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else if (_mode == "cubemapPrefilter")
	{
		//generate mipmaps for different levels of roughness.
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, _width, _height, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}
	else if (_mode == "brdfLUT")
	{
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_2D, m_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, _width, _height, 0, GL_RG, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "\n This texture ctor can only create cubemaps for now. Use with last arg as 'cube' or 'cubemap'\n";
		//todo if empty 2d maps are needed.
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}
