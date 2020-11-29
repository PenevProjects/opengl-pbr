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
	if (glewInit() != GLEW_OK)
	{
		throw std::exception();
	}

	SDL_GL_SetSwapInterval(0);

	std::unique_ptr<Shader> stdShader = std::make_unique<Shader>("../src/basicColor.vert", "../src/basicColor.frag");
	//std::unique_ptr<Shader> basicColorShader = std::make_unique<Shader>("../src/basicColor.vert", "../src/basicColor.frag");
	std::unique_ptr<Shader> lampShader = std::make_unique<Shader>("../src/lightShader.vert", "../src/lightShader.frag");

	//std::shared_ptr<Texture> diffuseTexture = std::make_shared<Texture>("../assets/medea_diffuse.png");
	//std::shared_ptr<Texture> specularTexture = std::make_shared<Texture>("../assets/medea_specular.png");

	//std::shared_ptr<Entity> curuthers = std::make_shared<Entity>("../assets/medea.obj");
	Model ourModel("../assets/room/sofa.fbx");




	std::shared_ptr<Entity> lamp = std::make_shared<Entity>("../assets/cube.obj");

	//cube->modelMatrix = glm::translate(cube->modelMatrix, glm::vec3(1, 1, -3));

	glm::vec3 lightPos(0.0f, 3.0f, 6.0f);
	//lamp->modelMatrix = glm::translate(lamp->modelMatrix, lightPos);
	//lamp->modelMatrix = glm::scale(lamp->modelMatrix, glm::vec3(0.2f));

	std::shared_ptr<Camera> cam1 = std::make_shared<Camera>();


	//TIME
	std::unique_ptr<Time> time = std::make_unique<Time>();




	bool quit = false;
	float translation = 0.0f;
	bool enableCull = true;
	bool enableBlend = true;

	double lastTime = 0.0;
	glm::mat4 modelM(1.0f);


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
				float deltaX = e.motion.xrel;
				float deltaY = -e.motion.yrel;
				cam1->ProcessMouseInput(deltaX, deltaY);
			}

		}
		//time calculations
		Time::Update();

		if (Time::LimitFPS(144.0f))
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

			float speed = 0.001f;
			float range = 10.0f;
			translation = glm::sin(SDL_GetTicks()*speed)*range; //oscillate
			lightPos.x = translation;




			glClearColor(0.39f, 0.58f, 0.93f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			stdShader->Use();

			stdShader->setVec3("light.position", lightPos);
			stdShader->setVec3("viewPos", cam1->getPosition());

			// light properties
			stdShader->setVec3("light.ambient", glm::vec3(0.2f));
			stdShader->setVec3("light.diffuse", glm::vec3(0.6f));
			stdShader->setVec3("light.specular", glm::vec3(0.9f));

			//// material properties
			stdShader->setFloat("material.shininess", 8.0f);
			stdShader->setViewMatrix(*cam1, GL_TRUE);
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(1.0f));	// it's a bit too big for our scene, so scale it down
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			stdShader->setMat4("u_Model", model);
			ourModel.Draw(*stdShader);
			stdShader->StopUsing();

			//stdShader->RenderObject(*curuthers);

			//basicColorShader->Use();
			//glEnable(GL_DEPTH_TEST);
			//basicColorShader->setViewMatrix(*cam1, GL_TRUE);
			//// render the loaded model
			//glm::mat4 model = glm::mat4(1.0f);
			//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
			//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
			//basicColorShader->setMat4("u_Model", model);
			//ourModel.Draw(*basicColorShader);



			if (enableCull)
			{
				glEnable(GL_CULL_FACE);
			}

			if (enableBlend)
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}



			glDisable(GL_CULL_FACE);
			//stdShader->RenderObject(*cube);

			//basicColorShader->StopUsing();

			lampShader->Use();
			lampShader->setViewMatrix(*cam1, GL_TRUE);
			glm::mat4 lampModel = glm::mat4{ 1.0f };
			lampModel = glm::translate(lampModel, lightPos);
			lamp->modelMatrix = lampModel;
			lampShader->RenderObject(*lamp);
			lampShader->StopUsing();


			glDisable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);

			glBindVertexArray(0);
			glUseProgram(0);
			
			Time::Reset();
			SDL_GL_SwapWindow(window);
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}