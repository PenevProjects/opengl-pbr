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
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

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
	glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.
	glEnable(GL_MULTISAMPLE);



	std::unique_ptr<Shader> stdShader = std::make_unique<Shader>("../src/shaders/blinn-lighting.vert", "../src/shaders/blinn-lighting.frag");
	std::unique_ptr<Shader> normalsShader = std::make_unique<Shader>("../src/shaders/pbr.vert", "../src/shaders/pbr.frag");
	std::unique_ptr<Shader> basicColorShader = std::make_unique<Shader>("../src/shaders/basic-color.vert", "../src/shaders/basic-color.frag");
	std::unique_ptr<Shader> lampShader = std::make_unique<Shader>("../src/shaders/pure-white.vert", "../src/shaders/pure-white.frag");
	std::unique_ptr<Shader> framebufShader = std::make_unique<Shader>("../src/shaders/glowing-edges.vert", "../src/shaders/glowing-edges.frag");
	std::unique_ptr<Shader> skyboxShader = std::make_unique<Shader>("../src/shaders/skybox.vert", "../src/shaders/skybox.frag");
	std::unique_ptr<Shader> pbrShader = std::make_unique<Shader>("../src/shaders/pbrPractice.vert", "../src/shaders/pbrPractice.frag");
	std::unique_ptr<Shader> equirectToCubemapShader = std::make_unique<Shader>("../src/shaders/HDR-skybox.vert", "../src/shaders/HDR-skybox.frag");

	std::shared_ptr<FrameBuffer> framebuf1 = std::make_shared<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);

	//floor normal shader
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



	std::vector<std::string> skyboxTextures{
		"../assets/skybox/right.jpg",
		"../assets/skybox/left.jpg",
		"../assets/skybox/top.jpg",
		"../assets/skybox/bottom.jpg",
		"../assets/skybox/front.jpg",
		"../assets/skybox/back.jpg"
	};
	std::shared_ptr<Skybox> skyboxFrom6Maps = std::make_shared<Skybox>(skyboxTextures);


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




	pbrShader->Use();
	for (unsigned int i = 0; i < sizeof(lightPos) / sizeof(lightPos[0]); i++)
	{
		pbrShader->setVec3("lightPos[" + std::to_string(i) + "]", lightPos[i]);
		pbrShader->setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
	}
	pbrShader->StopUsing();
	skyboxShader->Use();
	skyboxShader->setInt("environmentMap", 0);
	skyboxShader->StopUsing();

	unsigned int captureFBO;
	unsigned int captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	unsigned int hdrTexture;
	Texture::LoadHDR(hdrTexture, "../assets/Road_to_MonumentValley_Ref.hdr");
	// pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
	// ----------------------------------------------------------------------------------------------
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	// pbr: convert HDR equirectangular environment map to cubemap equivalent
	// ----------------------------------------------------------------------
	// pbr: setup cubemap to render to and attach to framebuffer
		// ---------------------------------------------------------
	unsigned int envCubemap;
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	equirectToCubemapShader->Use();
	equirectToCubemapShader->setInt("equirectangularMap", 0);
	equirectToCubemapShader->setMat4("u_Projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);


	glViewport(0, 0, 512,512); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		equirectToCubemapShader->setMat4("u_View", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	//FRAMEBUFFERS///////////////////////////////////////////
	//framebufShader->Use();
	//framebufShader->setInt("screenTexture", 0);
	//framebufShader->setFloat("screenWidth", WINDOW_WIDTH);
	//framebufShader->setFloat("screenHeight", WINDOW_HEIGHT);
	//framebufShader->StopUsing();
	//SKYBOX////////////////////////////////////////////////


	//MODEL SHADER/////////////////////////////////////////
	stdShader->Use();
	// light properties
	stdShader->setVec3("light.ambient", glm::vec3(2.0f));
	stdShader->setVec3("light.diffuse", glm::vec3(1.5f));
	stdShader->setVec3("light.specular", glm::vec3(1.9f));
	stdShader->setFloat("light.constant", 1.0f);
	stdShader->setFloat("light.linear", 0.014f);
	stdShader->setFloat("light.quadratic", 0.0007f);
	// material properties
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
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



			pbrShader->Use();
			pbrShader->setVec3("viewPos", cam1->getPosition());
			pbrShader->setViewAndProjectionMatrix(*cam1, true);

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
			glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
			renderCube();
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