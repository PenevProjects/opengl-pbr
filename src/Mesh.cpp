#include "Mesh.h"
#include "Shader.h"


Mesh::Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, std::vector<Texture> _textures, Colors _material)
{
	this->m_vertices = _vertices;
	this->m_indices = _indices;
	this->m_textures = _textures;
	this->m_colors = _material;

	setupMesh();
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int),
		&m_indices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texUVs));

	glBindVertexArray(0);
}

void Mesh::Render(Shader &_shader)
{
	_shader.setBool("hasTextures", false);

	_shader.setVec3("material.diffuse", this->m_colors.diffuse);
	_shader.setVec3("material.specular", this->m_colors.specular);
	_shader.setVec3("material.ambient", this->m_colors.ambient);
	if (!m_textures.empty())
	{
		_shader.setBool("hasTextures", true);
		for (unsigned int i = 0; i < m_textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
			// retrieve texture name
			std::string name = m_textures[i].typeName;

			//set the id of the sampler
			_shader.setInt(("material." + name).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, m_textures.at(i).id);
		}
	}
	glActiveTexture(GL_TEXTURE0);

	// draw mesh
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}