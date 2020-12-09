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
	glEnable(GL_MULTISAMPLE);



	std::unique_ptr<Shader> stdShader = std::make_unique<Shader>("../src/shaders/blinn-lighting.vert", "../src/shaders/blinn-lighting.frag");
	std::unique_ptr<Shader> basicColorShader = std::make_unique<Shader>("../src/shaders/basic-color.vert", "../src/shaders/basic-color.frag");
	std::unique_ptr<Shader> lampShader = std::make_unique<Shader>("../src/shaders/pure-white.vert", "../src/shaders/pure-white.frag");
	std::unique_ptr<Shader> framebufShader = std::make_unique<Shader>("../src/shaders/glowing-edges.vert", "../src/shaders/glowing-edges.frag");
	std::unique_ptr<Shader> skyboxShader = std::make_unique<Shader>("../src/shaders/skybox.vert", "../src/shaders/skybox.frag");


	std::shared_ptr<FrameBuffer> framebuf1 = std::make_shared<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);

	//floor normal shader
	std::shared_ptr<Model> floor = std::make_shared<Model>("../assets/skybox/untitled.obj");
	floor->m_modelMatrix = glm::translate(floor->m_modelMatrix, glm::vec3(0.0f, -3.0f, 0.0f));

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



	std::vector<std::string> skyboxTextures{
		"../assets/skybox/right.jpg",
		"../assets/skybox/left.jpg",
		"../assets/skybox/top.jpg",
		"../assets/skybox/bottom.jpg",
		"../assets/skybox/front.jpg",
		"../assets/skybox/back.jpg"
	};
	std::shared_ptr<Skybox> skybox = std::make_shared<Skybox>(skyboxTextures);


	std::shared_ptr<Entity> lamp = std::make_shared<Entity>("../assets/cube.obj");


	glm::vec3 lightPos(0.0f, 3.0f, 10.0f);
	std::shared_ptr<Camera> cam1 = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 15.0f));

	bool quit = false;
	float translation = 0.0f;
	bool enableCull = true;
	bool enableBlend = true;

	double lastTime = 0.0;

	//FRAMEBUFFERS///////////////////////////////////////////
	framebufShader->Use();
	framebufShader->setInt("screenTexture", 0);
	framebufShader->setFloat("screenWidth", WINDOW_WIDTH);
	framebufShader->setFloat("screenHeight", WINDOW_HEIGHT);
	framebufShader->StopUsing();
	//SKYBOX////////////////////////////////////////////////
	skyboxShader->Use();
	skyboxShader->setInt("u_Skybox", 0);
	skyboxShader->StopUsing();

	//MODEL SHADER/////////////////////////////////////////
	stdShader->Use();

	// light properties
	stdShader->setVec3("light.ambient", glm::vec3(2.0f));
	stdShader->setVec3("light.diffuse", glm::vec3(1.5f));
	stdShader->setVec3("light.specular", glm::vec3(1.9f));

	stdShader->setFloat("light.constant", 1.0f);
	stdShader->setFloat("light.linear", 0.014f);
	stdShader->setFloat("light.quadratic", 0.0007f);

	//// material properties
	stdShader->setFloat("material.shininess", 8.0f);

	stdShader->StopUsing();

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
			glClearColor(0.39f, 0.58f, 0.93f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);


			stdShader->Use();

			stdShader->setVec3("light.position", lightPos);
			//rendering properties
			stdShader->setVec3("viewPos", cam1->getPosition());
			stdShader->setViewAndProjectionMatrix(*cam1, true);

			//render sword
			sword->RenderMeshes(*stdShader);

			//render tv
			tv->RenderMeshes(*stdShader);

			//render oni mask
			oniMask->RenderMeshes(*stdShader);


			floor->RenderMeshes(*stdShader);
			stdShader->StopUsing();

			if (enableCull)
			{
				glEnable(GL_CULL_FACE);
			}

			if (enableBlend)
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}

			//stdShader->RenderObject(*cube);

			//basicColorShader->StopUsing();

			lampShader->Use();

			float speed = 0.001f;
			float range = 10.0f;
			translation = glm::sin(SDL_GetTicks()*speed)*range; //oscillate
			lightPos.x = translation;

			lampShader->setViewAndProjectionMatrix(*cam1, true);
			lamp->modelMatrix = glm::translate(glm::mat4{ 1.0f }, lightPos);
			lampShader->RenderObject(*lamp);
			lampShader->StopUsing();

			
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//framebufShader->Use();
			//framebuf1->DrawRenderTexture();
			//framebufShader->StopUsing();


			///////////////////////SKYBOX//////////////////////
			glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
			skyboxShader->Use();
			// remove translation from the view matrix
			skyboxShader->setMat4("u_View", glm::mat4(glm::mat3(cam1->generateViewMatrix())));
			skyboxShader->setMat4("u_Projection", cam1->generateProjMatrixPersp());
			skybox->DrawSkybox();
			glDepthFunc(GL_LESS); // set depth function back to default
			////////////////////////////////////////////////////


			glBindVertexArray(0);
			
			Time::Reset();
			SDL_GL_SwapWindow(window);
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}