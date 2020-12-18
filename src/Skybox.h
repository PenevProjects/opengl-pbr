#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <vector>
#include <memory>

#include "Texture.h"
#include "FrameBuffer.h"

class Skybox
{
	//friend class Texture;
public:
	/**\brief Creates an environment skybox from an HDR equirectangular image.
	*
	* @param _pathToHDR Path to .hdr file.
	* @param _size Specified width and height of render texture for the cubemap dimensions.
	*/
	Skybox(std::string _pathToHDR, unsigned int _size);
	~Skybox();
	Skybox(const Skybox&) = delete;
	Skybox& operator=(const Skybox&) = delete;
	void RenderCube();
	std::weak_ptr<Texture> GetSkyboxMap() { return m_environmentMap; }
	std::weak_ptr<Texture> GetIrradianceMap() { return m_irradiance; }
	std::weak_ptr<Texture> GetPrefilterMap() { return m_prefilterMap; }
	std::weak_ptr<Texture> GetBrdfLUT() { return m_brdfLookUpTexture; }

private:
	std::shared_ptr<Texture> m_environmentMap;
	std::shared_ptr<Texture> m_irradiance;
	std::shared_ptr<Texture> m_prefilterMap;
	std::shared_ptr<Texture> m_brdfLookUpTexture;

	std::shared_ptr<FrameBuffer> m_framebuffer;
	
	//todo:
	//void TransformHDRtoCubemap();	
	//void GenerateEnvironmentMap();
	//void GenerateIrradianceMap();
	//void GeneratePrefilterMap();
	//void GenerateBrdfLUT();
	
	void CreateCubeVAO();

	unsigned int m_vao;
	unsigned int m_size;
	std::string m_path;

	glm::mat4 m_captureProjection;
	glm::mat4 m_captureView[6];
};