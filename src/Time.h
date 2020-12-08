#ifndef _TIME_
#define _TIME_

#include <SDL2/SDL.h>



class Time
{
private:
	static Uint32 m_lastTick;
	static Uint32 m_deltaTicks;
	static double m_deltaTime;
	static double m_fps;
	static int fpsTimer;
	static int fps5Ticks;
public:
	Time();
	~Time();
	static double GetDeltaTime() { return m_deltaTime; }
	static double GetFps() { return m_fps; }
	static Uint32 GetCurrentTime() { return SDL_GetTicks(); }
	static void Update();
	static void Reset();
	static bool LimitFPS(double _fps=m_fps);
	/*****
	* Displays FPS over 10 ticks
	*****/
	static void DisplayFPSinWindowTitle(SDL_Window* _window);
};



#endif