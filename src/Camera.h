#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <SDL2/SDL.h>
#include <memory>


#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720



/**
* \brief Camera class which handles camera movement and generates view and projection matrices
*
* Stores values for position and orientation of camera.
* Speed and sensitivity of camera are also configurable as public properties(unnecessary to write getters and setters - safe variables).
*/
class Camera
{
private:
	float m_Yaw;
	float m_Pitch;
	float m_FOV;
	glm::vec3 m_Position;
	glm::vec3 m_Facing;
	glm::vec3 m_Right;
	glm::vec3 m_Up;
	glm::vec3 m_WorldUp;

	int static m_ScreenWidth;
	int static m_ScreenHeight;
public:
	float m_CameraMovementSpeed;
	float m_CameraSensitivity;
	///Creates Camera object with default values
	Camera(glm::vec3 _position = glm::vec3(0.0f, 0.0f, 5.0f),
		glm::vec3 _facing = glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f));
	///Creates Camera object with set position, facing direction, and up direction.
	void ProcessKeyboardInput();
	void ProcessMouseInput(float xoffset, float yoffset);
	void ProcessWindowResizing(int _screenWidth, int _screenHeight);
	void ProcessZoom();
	void UpdateCameraVectors();
	/// Generates and returns a view matrix of the current camera.
	glm::mat4 generateViewMatrix() const;
	/// Generates and returns a perspective projection matrix of the current camera.
	glm::mat4 generateProjMatrixPersp() const;
	/// Generates and returns an ortographic projection matrix of the current camera.
	glm::mat4 generateProjMatrixOrtho() const;
	glm::vec3 getPosition() const { return m_Position; }
	glm::vec3 getFacing() const { return m_Facing; }


};
