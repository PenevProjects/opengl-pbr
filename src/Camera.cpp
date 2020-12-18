#include "Camera.h"
#include "Time.h"

extern const float defaultYAW = -90.0f;
extern const float defaultPITCH = 0.0f;
extern const float defaultSPEED = 10.0f;
extern const float defaultSENSITIVITY = 0.1f;
extern const float defaultZOOM = 45.0f;


int Camera::m_ScreenWidth = WINDOW_WIDTH;
int Camera::m_ScreenHeight = WINDOW_HEIGHT;

Camera::Camera(glm::vec3 _position, glm::vec3 _facing, glm::vec3 _up) :
	m_Yaw(defaultYAW),
	m_Pitch(defaultPITCH),
	m_CameraSensitivity(defaultSENSITIVITY),
	m_CameraMovementSpeed(defaultSPEED),
	m_FOV(defaultZOOM),
	m_Position(_position),
	m_Facing(_facing),
	m_Up(_up),
	m_WorldUp(glm::vec3(0, 1.0f, 0))
{
}



/**
*\brief Handles input for translation and updates position.
*
* Processes current keystroke and transforms the camera's position using current orientation of camera.
*/
void Camera::ProcessKeyboardInput()
{
	const Uint8* key = SDL_GetKeyboardState(NULL);
	if (key[SDL_SCANCODE_W]) //moving left
	{
		m_Position += m_CameraMovementSpeed * m_Facing * (float)Time::GetDeltaTime();
	}
	if (key[SDL_SCANCODE_S])
	{
		m_Position -= m_CameraMovementSpeed * m_Facing * (float)Time::GetDeltaTime();
	}
	if (key[SDL_SCANCODE_D])
	{
		m_Position += glm::normalize(glm::cross(m_Facing, m_Up)) * m_CameraMovementSpeed * (float)Time::GetDeltaTime();
	}
	if (key[SDL_SCANCODE_A])
	{
		m_Position -= glm::normalize(glm::cross(m_Facing, m_Up)) * m_CameraMovementSpeed * (float)Time::GetDeltaTime();
	}
}

/**
*\brief Handles input for mouse orientation.
*
* Processes current keystroke and transforms the camera's position using current orientation of camera.
*/
void Camera::ProcessMouseInput(float xoffset, float yoffset)
{
	m_Yaw += (xoffset * m_CameraSensitivity ); //* Time::GetDeltaTime()
	m_Pitch += (yoffset * m_CameraSensitivity ); //* Time::GetDeltaTime()

	if (m_Pitch > 89.0f)
	{
		m_Pitch = 89.0f;
	}
	if (m_Pitch < -89.0f)
	{
		m_Pitch = -89.0f;
	}

	UpdateCameraVectors();



}

/**
* \brief Updates camera orientation according to mouse input.
*
* considering pitch, directions in 3D can be presented as
* x = cos(yaw) * cos(pitch)
* z = sin(yaw) * cos(pitch)
* y = sin(yaw)
* deduct other orientation vectors from these.
*/
void Camera::UpdateCameraVectors()
{
	glm::vec3 direction;
	direction.x = glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
	direction.z = glm::sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
	direction.y = glm::sin(glm::radians(m_Pitch));
	m_Facing = glm::normalize(direction);
	m_Right = glm::normalize(glm::cross(m_Facing, m_WorldUp));
	m_Up = glm::normalize(glm::cross(m_Right, m_Facing));
}

/// In case of window resize, updates width and height for perspective projection calculation.
void Camera::ProcessWindowResizing(int _screenWidth, int _screenHeight)
{
	m_ScreenWidth = _screenWidth;
	m_ScreenHeight = _screenHeight;
}

/// Handles keyboard input for zooming with Q and E (works with Field of View).
void Camera::ProcessZoom()
{
	const Uint8* key = SDL_GetKeyboardState(NULL);
	if (key[SDL_SCANCODE_Q])
	{
		m_FOV += 1;
	}
	if (key[SDL_SCANCODE_E])
	{
		m_FOV -= 1;
	}

}

glm::mat4 Camera::generateViewMatrix() const
{
	return glm::lookAt(m_Position, m_Position + m_Facing, m_Up);
}

glm::mat4 Camera::generateProjMatrixPersp() const
{
	return glm::perspective(glm::radians(m_FOV), (float)m_ScreenWidth / (float)m_ScreenHeight, 0.1f, 100.0f);
}
glm::mat4 Camera::generateProjMatrixOrtho() const
{
	return glm::ortho((float)-m_ScreenWidth/2, (float)m_ScreenWidth/2, (float)m_ScreenHeight/2, (float)-m_ScreenHeight/2, 0.1f, 100.0f);
}