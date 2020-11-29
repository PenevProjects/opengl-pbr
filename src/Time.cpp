#include "Time.h"
#include <sstream>
#include <iostream>

Time::Time()
{}
Time::~Time() 
{}


Uint32 Time::m_lastTick = 0;
float Time::m_deltaTime = 0.1f;
Uint32 Time::m_deltaTicks = 0;
float Time::m_fps = 0;
int Time::fpsTimer = 0;
int Time::fps5Ticks = 0;


void Time::Reset()
{
	m_lastTick = SDL_GetTicks();
	m_deltaTicks = 0;
	m_deltaTime = 0;
}
void Time::Update()
{
	m_deltaTicks = SDL_GetTicks() - m_lastTick;
	m_deltaTime = m_deltaTicks / 1000.0f;
}

bool Time::LimitFPS(float _fps)
{
	if (_fps != m_fps)
	{
		m_fps = _fps;
	}
	if (m_deltaTime >= 1.0f / _fps) {	
		return true;
	}
	return false;
}

void Time::DisplayFPSinWindowTitle(SDL_Window* _window)
{
	std::stringstream ss;
	ss << "OPENGL, FPS: " << 1.0f/((SDL_GetTicks() - m_lastTick)/1000.0f);
	SDL_SetWindowTitle(_window, ss.str().c_str() );
}