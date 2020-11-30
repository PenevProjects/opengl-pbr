#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"


class Model
{
public:
	Model(char *_path)
	{
		ImportModel(_path);
	}
	void RenderMeshes(Shader &_shader)
	{
		for (auto& mesh : m_meshes)
		{
			mesh.Render(_shader);
		}
	}
	void AddTexture(std::string _path, std::string _typeName);
	std::vector<Texture> GetLoadedTextures() { return m_texturesLoaded; }
private:
	// model data
	std::vector<Mesh> m_meshes;
	std::vector<Texture> m_texturesLoaded;
	std::string m_directory;

	void ImportModel(std::string _path);
	void ProcessNode(aiNode* _node, const aiScene* _scene);
	Mesh ProcessMesh(aiMesh* _mesh, const aiScene* _scene);
	std::vector<Texture> LoadMaterialTextures(const aiScene* _scene, aiMaterial* _mat, aiTextureType _type, std::string _typeName);
	unsigned int TextureFromFile(std::string _file, bool _gamma=false);
	unsigned int TextureFromEmbedded(const aiTexture* _texture);
};