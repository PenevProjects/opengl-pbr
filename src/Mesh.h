#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>
#include <vector>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Texture.h"

class Shader;

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texUVs;
	glm::vec3 tangents;
	glm::vec3 bitangents;
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
	//using shared ptrs to avoid copy ctor
	std::vector<std::shared_ptr<Texture>> m_textures;
	Colors m_colors;
	
	Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, std::vector<std::shared_ptr<Texture>> _textures, Colors _material);
	Mesh(aiMesh *_mesh, const aiScene *_scene, std::vector<std::shared_ptr<Texture>>& _textures);
	~Mesh();
	Mesh& operator=(const Mesh&) = delete;
	Mesh(const Mesh&) = delete;
	
	/**
	* @param _shader A shader object passed by const-ref, because the object needs to be only inspected.
	*/
	void Render(const Shader &_shader);

private:
	unsigned int m_vao, m_vbo, m_ebo;

	void setupMeshVAO();
};