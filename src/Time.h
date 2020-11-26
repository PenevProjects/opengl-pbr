#ifndef _TIME_
#define _TIME_

#include <SDL2/SDL.h>

class Time
{
private:
	float static m_DeltaTime;
	int static m_LastTime;
	int static m_Now;
	float static m_Fps;
	int fpsTimer;
	int fps10Ticks;
public:
	Time();
	~Time();
	float static getDeltaTime() { return m_DeltaTime; }
	float static getFps() { return m_Fps; }
	void updateTime(int _currentTime);
	/*****
	* Displays FPS over 10 ticks
	*****/
	void DisplayFPSinWindowTitle(SDL_Window* _window);
};



#endif