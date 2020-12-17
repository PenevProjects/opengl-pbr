#ifndef _TIME_
#define _TIME_

#include <windows.h>
#include <SDL2/SDL.h>



class Time
{
private:
	static LARGE_INTEGER StartingTime;
	static LARGE_INTEGER EndingTime;
	static LARGE_INTEGER ElapsedMicroseconds;
	static LARGE_INTEGER Frequency;
	static double deltaTime;
	static int frames;
	static float avgFPS;
public:
	static double GetDeltaTime() { return deltaTime; }
	static double GetFPS() { return 1.0 / deltaTime; }
	static void Update();
	static void Reset();
	static void DisplayFPSinWindowTitle(SDL_Window* _window);
};



#endif