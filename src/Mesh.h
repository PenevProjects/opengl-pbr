#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>
#include <vector>

class Shader;

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texUVs;
};

struct Texture {
	unsigned int id;
	std::string typeName;
	std::string path; 
};

struct Colors {
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 ambient;
	float shininess;
};

class Mesh {
public:
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
	std::vector<Texture> m_textures;
	Colors m_colors;
	
	Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, std::vector<Texture> _textures, Colors _material);
	~Mesh()
	{
		glDeleteVertexArrays(1, &m_vao);
		glDeleteVertexArrays(1, &m_vbo);
		glDeleteVertexArrays(1, &m_ebo);
	}
	Mesh& operator=(const Mesh&) = delete;
	Mesh(const Mesh&) = delete;

	void Render(Shader &_shader);
private:
	unsigned int m_vao, m_vbo, m_ebo;

	void setupMesh();
};