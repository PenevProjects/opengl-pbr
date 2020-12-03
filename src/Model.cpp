#include "Model.h"
#include <iostream>




void Model::ImportModel(std::string path)
{
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	m_directory = path.substr(0, path.find_last_of('/'));
	std::cout << "Directory: " << m_directory << std::endl;

	ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode *_node, const aiScene *_scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < _node->mNumMeshes; i++)	
	{
		aiMesh *mesh = _scene->mMeshes[_node->mMeshes[i]];
		aiMaterial* material = _scene->mMaterials[mesh->mMaterialIndex];
		std::vector<std::shared_ptr<Texture>> meshTextures = LoadMaterialTextures(_scene, material);

		std::shared_ptr<Mesh> b = std::make_shared<Mesh>(mesh, _scene, meshTextures);
		std::cout << "hit";
		m_meshes.push_back(b);
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < _node->mNumChildren; i++)
	{
		ProcessNode(_node->mChildren[i], _scene);
	}
}

std::vector<std::shared_ptr<Texture>> Model::LoadMaterialTextures(const aiScene* _scene, aiMaterial* _mat)
{
	std::vector<std::shared_ptr<Texture>> textures;
	// 1. diffuse maps
	std::vector<std::shared_ptr<Texture>> diffuseMaps = LoadTexturesOfType(_scene, _mat, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	std::vector<std::shared_ptr<Texture>> specularMaps = LoadTexturesOfType(_scene, _mat, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<std::shared_ptr<Texture>> normalMaps = LoadTexturesOfType(_scene, _mat, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<std::shared_ptr<Texture>> heightMaps = LoadTexturesOfType(_scene, _mat, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	// 5. to add more if supported

	return textures;
}

std::vector<std::shared_ptr<Texture>> Model::LoadTexturesOfType(const aiScene* scene, aiMaterial *mat, aiTextureType _type, std::string _typeName)
{
	std::vector<std::shared_ptr<Texture>> textures;

	if (mat->GetTextureCount(_type) > 1)
	{
		std::cout << "\nMODEL::LoadTexturesOfType::MORE THAN ONE " << _typeName << " DETECTED FOR A MESH!!! THIS IS NOT CURRENTLY SUPPORTED.\n";
	}
	for (unsigned int i = 0; i < mat->GetTextureCount(_type); i++)
	{
		aiString pathToTexture;
		mat->GetTexture(_type, i, &pathToTexture);
		bool loaded = false;
		for (unsigned int j = 0; j < m_texturesLoaded.size(); j++)
		{
			//if the loaded texture's path(cstring) is the same as the current material texture path
			if (std::strcmp(m_texturesLoaded[j]->m_path.data(), pathToTexture.C_Str()) == 0)
			{
				textures.push_back(m_texturesLoaded[j]); //push the loaded texture 
				loaded = true;
				break;
			}
		}
		if (!loaded)
		{   // if texture hasn't been loaded already, load it

			//check if there is an embedded texture
			const aiTexture* embedded = scene->GetEmbeddedTexture(pathToTexture.C_Str());

			if (embedded) {
				//construct from embedded
				std::shared_ptr<Texture> texture = std::make_shared<Texture>(embedded);
				//push to local texture vector(of uniform types)
				textures.push_back(texture);
				//push to member texture vector(of various types)
				m_texturesLoaded.push_back(texture);
			}
			else {
				//process path
				std::string filename = pathToTexture.C_Str();
				filename = m_directory + '/' + filename;
				//construct from file path
				std::shared_ptr<Texture> texture = std::make_shared<Texture>(filename, _typeName);
				//push to local texture vector(of uniform types)
				textures.push_back(texture);
				//push to member texture vector(of various types)
				m_texturesLoaded.push_back(texture);
			}
		}
	}
	return textures;
}


//need to do for ALL MESHES push back texture
//void Model::AddTexture(std::string _path, std::string _typeName)
//{
//	Texture texture;
//	texture.id = TextureFromFile(_path);
//	texture.typeName = _typeName;
//	texture.path = _path;
//	m_texturesLoaded.emplace_back(texture); // add to loaded textures
//}