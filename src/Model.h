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
		importModel(_path);
	}
	void Draw(Shader &shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i].Draw(shader);
		}
	}
private:
	// model data
	std::vector<Mesh> meshes;
	std::vector<Texture> textures_loaded;
	std::string directory;

	void importModel(std::string _path);
	void processNode(aiNode *_node, const aiScene *_scene);
	Mesh processMesh(aiMesh *_mesh, const aiScene *_scene);
	std::vector<Texture> loadMaterialTextures(const aiScene* _scene, aiMaterial *_mat, aiTextureType _type, std::string _typeName);
	unsigned int TextureFromFile(const char *_path, const std::string &_directory, bool _gamma=false);
	unsigned int TextureFromEmbedded(const aiTexture* texture);
};