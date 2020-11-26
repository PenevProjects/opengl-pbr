#include "Time.h"
#include <sstream>

Time::Time() : 
	fpsTimer(0),
	fps10Ticks(0)
{}
Time::~Time() 
{}

float Time::m_DeltaTime = 0.1f;
int Time::m_LastTime = 0;
float Time::m_Fps = 0;
int Time::m_Now = 0;


void Time::updateTime(int _currentTime)
{
	m_Now = _currentTime;
	m_DeltaTime = (m_Now - m_LastTime) / 1000.0f;
	m_Fps = 1.0 / m_DeltaTime;
	m_LastTime = m_Now;
}

void Time::DisplayFPSinWindowTitle(SDL_Window* _window)
{
	fpsTimer++;
	fps10Ticks += 1.0 / m_DeltaTime;
	if (fpsTimer > 9)
	{
		fps10Ticks /= 10;
		std::stringstream x;
		x << "OPENGL, FPS: " << fps10Ticks;
		SDL_SetWindowTitle(_window, x.str().c_str());
		fpsTimer = 0;
		fps10Ticks = 0;
	}
}