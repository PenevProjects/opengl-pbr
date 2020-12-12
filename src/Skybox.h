#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <vector>
#include <memory>

#include "Texture.h"

class Skybox
{
	//friend class Texture;
public:
	/**\brief Creates an environment skybox from an HDR equirectangular image.
	*
	* @param _pathToHDR Path to .hdr file.
	* @param _width Specified width of render texture for the cubemap dimensions.
	* @param _height Specified height of render texture for the cubemap dimensions.
	*/
	Skybox(std::string _pathToHDR, int _size);
	~Skybox();
	Skybox(const Skybox&) = delete;
	Skybox& operator=(const Skybox&) = delete;
	void RenderCube();
	std::weak_ptr<Texture> GetSkyboxMap() { return m_texture; }
	std::weak_ptr<Texture> GetIrradianceMap() { return m_irradiance; }

private:
	std::shared_ptr<Texture> m_texture;
	std::shared_ptr<Texture> m_irradiance;
	unsigned int m_vao;
	void CreateCubeVAO();
};