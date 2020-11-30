#include "Model.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


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

void Model::ProcessNode(aiNode *node, const aiScene *scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(ProcessMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
	// data to fill
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 tempVec; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// positions
		tempVec.x = mesh->mVertices[i].x;
		tempVec.y = mesh->mVertices[i].y;
		tempVec.z = mesh->mVertices[i].z;
		vertex.position = tempVec;
		// normals
		if (mesh->HasNormals())
		{
			tempVec.x = mesh->mNormals[i].x;
			tempVec.y = mesh->mNormals[i].y;
			tempVec.z = mesh->mNormals[i].z;
			vertex.normal = tempVec;
		}
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texUVs = vec;
		}
		else {
			std::cout << "No UVs detected in mesh." << std::endl;
			vertex.texUVs = glm::vec2(0.0f, 0.0f);
		}
		vertices.push_back(vertex);
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}


	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];


	//set colors of model in case it has no textures
	aiColor3D color(0.f, 0.f, 0.f);
	Colors tempColors;
	material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	tempColors.diffuse = glm::vec3(color.r, color.b, color.g);

	material->Get(AI_MATKEY_COLOR_AMBIENT, color);
	tempColors.ambient = glm::vec3(color.r, color.b, color.g);

	material->Get(AI_MATKEY_COLOR_SPECULAR, color);
	tempColors.specular = glm::vec3(color.r, color.b, color.g);
	

	// 1. diffuse maps
	std::vector<Texture> diffuseMaps = LoadMaterialTextures(scene, material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	std::vector<Texture> specularMaps = LoadMaterialTextures(scene, material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture> normalMaps = LoadMaterialTextures(scene, material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture> heightMaps = LoadMaterialTextures(scene, material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	std::cout << "\nfinal size of texture vec: " << textures.size();
	// return a mesh object created from the extracted mesh data
	return Mesh(vertices, indices, textures, tempColors);
}

std::vector<Texture> Model::LoadMaterialTextures(const aiScene* scene, aiMaterial *mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	
	std::cout << "\nTextures of type: " << type << ":" << mat->GetTextureCount(type) << std::endl;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString pathToTexture;
		mat->GetTexture(type, i, &pathToTexture);
		bool skip = false;
		for (unsigned int j = 0; j < m_texturesLoaded.size(); j++)
		{
			if (std::strcmp(m_texturesLoaded[j].path.data(), pathToTexture.C_Str()) == 0)
			{ //if the loaded texture's path(cstring) is the same as the imported model's string(str)
				textures.push_back(m_texturesLoaded[j]); //push the loaded texture 
				skip = true;
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;

			//check if there is an embedded texture

			const aiTexture* embedded = scene->GetEmbeddedTexture(pathToTexture.C_Str());
			if (embedded) {
				texture.id = TextureFromEmbedded(embedded);
			}
			else {
				std::string filename = pathToTexture.C_Str();
				filename = m_directory + '/' + filename;
				texture.id = TextureFromFile(filename);
			}
			texture.typeName = typeName;
			texture.path = pathToTexture.C_Str();
			textures.emplace_back(texture);
		}
	}
	return textures;
}

unsigned int Model::TextureFromFile(std::string _file, bool gamma)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	int width, height, nrComponents;
	//stbi_set_flip_vertically_on_load(1);
	unsigned char *data = stbi_load(_file.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
	
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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
		std::cout << "Texture failed to load at path: " << _file << std::endl;
		stbi_image_free(data);
	}
	return textureID;
}

unsigned int Model::TextureFromEmbedded(const aiTexture* texture)
{
	std::cout << "\nEmbedded texture detected!\n";
	unsigned int textureID;
	glGenTextures(1, &textureID);
	//stbi_set_flip_vertically_on_load(1);
	unsigned char *data = nullptr;
	int width, height, nrComponents;
	if (texture->mHeight == 0)
	{ //if texture is compressed into a continuous stream
		data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &nrComponents, 0);
	}
	else
	{ //if texture is raw image data
		data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth * texture->mHeight, &width, &height, &nrComponents, 0);
	}
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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
		stbi_image_free(data);
	}
	return textureID;
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