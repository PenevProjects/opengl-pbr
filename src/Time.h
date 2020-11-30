#ifndef _TIME_
#define _TIME_

#include <SDL2/SDL.h>



class Time
{
private:
	static Uint32 m_lastTick;
	static Uint32 m_deltaTicks;
	static float m_deltaTime;
	static float m_fps;
	static int fpsTimer;
	static int fps5Ticks;
public:
	Time();
	~Time();
	static float GetDeltaTime() { return m_deltaTime; }
	static float GetFps() { return m_fps; }
	static void setFps(float _fps) { m_fps = _fps; }
	static Uint32 GetCurrentTime() { return SDL_GetTicks(); }
	static void Update();
	static void Reset();
	static bool LimitFPS(float _fps=m_fps);
	/*****
	* Displays FPS over 10 ticks
	*****/
	static void DisplayFPSinWindowTitle(SDL_Window* _window);
};



#endif