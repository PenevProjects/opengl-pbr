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
#include "Texture.h"


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
			mesh->Render(_shader);
		}
	}
	std::vector<std::shared_ptr<Texture>> GetLoadedTextures() { return m_texturesLoaded; }
private:
	// model data, using shared ptrs to avoid copy ctor
	std::vector<std::shared_ptr<Mesh>> m_meshes;
	//using shared ptrs to avoid copy ctor
	std::vector<std::shared_ptr<Texture>> m_texturesLoaded;
	std::string m_directory;

	void ImportModel(std::string _path);
	void ProcessNode(aiNode* _node, const aiScene* _scene);
	std::vector<std::shared_ptr<Texture>> LoadMaterialTextures(const aiScene* _scene, aiMaterial* _mat);
	std::vector<std::shared_ptr<Texture>> LoadTexturesOfType(const aiScene* scene, aiMaterial *mat, aiTextureType type, std::string typeName);

};