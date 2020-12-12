#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <exception>
#include <vector>





#include "Shader.h"
#include "Camera.h"
#include "Entity.h"
#include "Time.h"
#include "Texture.h"
#include "Model.h"
#include "Framebuffer.h"
#include "Skybox.h"



int main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		throw std::exception();
	}


	SDL_Window *window = SDL_CreateWindow("OpenGL labs",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	
	//LOCK IN MOUSE SO U CAN LOOK AROUND
	SDL_SetRelativeMouseMode(SDL_TRUE);


	if (!SDL_GL_CreateContext(window))
	{
		throw std::exception();
	}


	SDL_GL_SetSwapInterval(0); //disable vsync evil
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);


	if (glewInit() != GLEW_OK)
	{
		throw std::exception();
	}
	//global opengl state
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); // for skybox rendering
	glEnable(GL_MULTISAMPLE);



	std::unique_ptr<Shader> stdShader = std::make_unique<Shader>("../src/shaders/blinn-lighting.vert", "../src/shaders/blinn-lighting.frag");
	std::unique_ptr<Shader> normalsShader = std::make_unique<Shader>("../src/shaders/pbr.vert", "../src/shaders/pbr.frag");
	std::unique_ptr<Shader> basicColorShader = std::make_unique<Shader>("../src/shaders/basic-color.vert", "../src/shaders/basic-color.frag");
	std::unique_ptr<Shader> lampShader = std::make_unique<Shader>("../src/shaders/pure-white.vert", "../src/shaders/pure-white.frag");
	std::unique_ptr<Shader> framebufShader = std::make_unique<Shader>("../src/shaders/glowing-edges.vert", "../src/shaders/glowing-edges.frag");
	std::unique_ptr<Shader> skyboxShader = std::make_unique<Shader>("../src/shaders/skybox.vert", "../src/shaders/skybox.frag");
	std::unique_ptr<Shader> pbrShader = std::make_unique<Shader>("../src/shaders/pbrPractice.vert", "../src/shaders/pbrPractice.frag");


	//floor shader
	std::shared_ptr<Model> floor = std::make_shared<Model>("../assets/floor/floor.fbx");
	floor->m_modelMatrix = glm::translate(floor->m_modelMatrix, glm::vec3(0.0f, -3.0f, 0.0f));
	floor->m_modelMatrix = glm::scale(floor->m_modelMatrix, glm::vec3(50.0f, 0.0f, 50.0f));


	//tv pbr
	std::shared_ptr<Model> tv = std::make_shared<Model>("../assets/tv/tv.fbx");
	tv->m_modelMatrix = glm::scale(tv->m_modelMatrix, glm::vec3(0.3f));	
	tv->m_modelMatrix = glm::rotate(tv->m_modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	tv->m_modelMatrix = glm::translate(tv->m_modelMatrix, glm::vec3(-40.0f, 0.0f, -10.0f));

	//sword pbr
	std::shared_ptr<Model> sword = std::make_shared<Model>("../assets/sword/sword.fbx");
	sword->m_modelMatrix = glm::scale(sword->m_modelMatrix, glm::vec3(10.0f));
	sword->m_modelMatrix = glm::rotate(sword->m_modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	//mask pbr
	std::shared_ptr<Model> oniMask = std::make_shared<Model>("../assets/oni/onito.fbx");
	oniMask->m_modelMatrix = glm::scale(oniMask->m_modelMatrix, glm::vec3(1.0f));
	oniMask->m_modelMatrix = glm::translate(oniMask->m_modelMatrix, glm::vec3(15.0f, -5.0f, 0.0f));


	std::shared_ptr<Entity> lamp0 = std::make_shared<Entity>("../assets/cube.obj");
	std::shared_ptr<Entity> lamp1 = std::make_shared<Entity>("../assets/cube.obj");
	std::shared_ptr<Entity> lamp2 = std::make_shared<Entity>("../assets/cube.obj");
	std::shared_ptr<Entity> lamp3 = std::make_shared<Entity>("../assets/cube.obj");


	glm::vec3 lightPos[4] = {
		glm::vec3(0.0f, 1.0f, 10.0f),
		glm::vec3(0.0f, 10.0f, -10.0f),
		glm::vec3(-25.0f, 3.0f, 10.0f),
		glm::vec3(25.0f, 3.0f, 10.0f)
	};
	glm::vec3 lightColors[4] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};

	std::shared_ptr<Camera> cam1 = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 15.0f));

	bool quit = false;
	float translation = 0.0f;
	bool enableCull = true;
	bool enableBlend = true;
	double lastTime = 0.0;


	std::shared_ptr<Skybox> skybox = std::make_shared<Skybox>("../assets/hdr/Factory_Catwalk_2k.hdr", 2048);
	//max amount of texture units that can be used concurrently from a model file is currently 9. Skybox uses the 10th texture unit. 
	int skyboxSampler = 10;


	pbrShader->Use();
	//set irradiance map to texture unit 12, to avoid trying to access the same sampler position(0 for albedo, etc)
	pbrShader->setInt("irradianceMap", skyboxSampler);
	for (unsigned int i = 0; i < sizeof(lightPos) / sizeof(lightPos[0]); i++)
	{
		pbrShader->setVec3("lightPos[" + std::to_string(i) + "]", lightPos[i]);
		pbrShader->setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
	}
	pbrShader->StopUsing();



	//SKYBOX////////////////////////////////////////////////



	skyboxShader->Use();
	skyboxShader->setInt("environmentCubemap", 0);
	skyboxShader->StopUsing();

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!quit)
	{
		SDL_Event e = { 0 };
		while (SDL_PollEvent(&e))
		{

			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_b)
				{
					enableBlend = !enableBlend;
				}
				else if (e.key.keysym.sym == SDLK_c)
				{
					enableCull = !enableCull;
				}
				else if (e.key.keysym.sym == SDLK_f)
				{
					//locking the cursor
					if ((bool)SDL_GetRelativeMouseMode())
						SDL_SetRelativeMouseMode(SDL_FALSE);
					else
						SDL_SetRelativeMouseMode(SDL_TRUE);
				}
			}
			else if (e.type == SDL_MOUSEMOTION)
			{
				float deltaX = static_cast<float>(e.motion.xrel);
				float deltaY = static_cast<float>(-e.motion.yrel);
				cam1->ProcessMouseInput(deltaX, deltaY);
			}

		}
		//time calculations
		Time::Update();

		if (Time::LimitFPS(60.0f))
		{
			Time::DisplayFPSinWindowTitle(window);

			//window resizing calculationg
			int width = 0;
			int height = 0;
			SDL_GetWindowSize(window, &width, &height);
			glViewport(0, 0, width, height);



			//camera updates
			cam1->ProcessKeyboardInput();
			cam1->ProcessZoom();
			cam1->ProcessWindowResizing(width, height);

	


			//glBindFramebuffer(GL_FRAMEBUFFER, framebuf1->GetID());
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



			pbrShader->Use();
			pbrShader->setVec3("viewPos", cam1->getPosition());
			pbrShader->setViewAndProjectionMatrix(*cam1, true);
			// bind pre-computed IBL data
			glActiveTexture(GL_TEXTURE0 + skyboxSampler);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->GetIrradianceMap().lock()->m_id);


			//render sword
			sword->RenderMeshes(*pbrShader);

			//render tv
			tv->RenderMeshes(*pbrShader);

			floor->RenderMeshes(*pbrShader);

			//render oni mask
			oniMask->RenderMeshes(*pbrShader);

			
			
			pbrShader->StopUsing();

			////stdShader->Use();
			////stdShader->setVec3("lightPos", lightPos);
			////stdShader->setVec3("viewPos", cam1->getPosition());
			////stdShader->setViewAndProjectionMatrix(*cam1, true);
			////floor->RenderMeshes(*stdShader);
			////stdShader->StopUsing();


			////stdShader->RenderObject(*cube);


			lampShader->Use();

			float speed = 0.001f;
			float range = 10.0f;
			translation = glm::sin(SDL_GetTicks()*speed)*range; //oscillate
			//lightPos.x = translation;

			lampShader->setViewAndProjectionMatrix(*cam1, true);
			lamp0->modelMatrix = glm::translate(glm::mat4{ 1.0f }, lightPos[0]);
			lampShader->RenderObject(*lamp0);
			lamp1->modelMatrix = glm::translate(glm::mat4{ 1.0f }, lightPos[1]);
			lampShader->RenderObject(*lamp1);
			lamp2->modelMatrix = glm::translate(glm::mat4{ 1.0f }, lightPos[2]);
			lampShader->RenderObject(*lamp2);
			lamp3->modelMatrix = glm::translate(glm::mat4{ 1.0f }, lightPos[3]);
			lampShader->RenderObject(*lamp3);

			lampShader->StopUsing();

			
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//framebufShader->Use();
			//framebuf1->DrawRenderTexture();
			//framebufShader->StopUsing();

			///////////////////////SKYBOX//////////////////////
			skyboxShader->Use();
			skyboxShader->setViewAndProjectionMatrix(*cam1, true);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->GetSkyboxMap().lock()->m_id);
			skybox->RenderCube();
			skyboxShader->StopUsing();
			glBindVertexArray(0);
			
			Time::Reset();
			SDL_GL_SwapWindow(window);
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}