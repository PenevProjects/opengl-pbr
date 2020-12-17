#include "Time.h"
#include <sstream>
#include <iostream>

LARGE_INTEGER Time::StartingTime{ 0 };
LARGE_INTEGER Time::EndingTime{ 0 };
LARGE_INTEGER Time::ElapsedMicroseconds{ 0 };
LARGE_INTEGER Time::Frequency{ 0 };
double Time::deltaTime{ 0.007 };
int Time::frames{ 0 };
float Time::avgFPS{ 0.0f };

void Time::Update()
{
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&StartingTime);
}

void Time::Reset()
{
	// We now have the elapsed number of ticks, along with the
	// number of ticks-per-second. We use these values
	// to convert to the number of elapsed microseconds.
	// To guard against loss-of-precision, we convert
	// to microseconds *before* dividing by ticks-per-second.
	//
	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
	ElapsedMicroseconds.QuadPart *= 1000000;
	ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
	deltaTime = (double)ElapsedMicroseconds.QuadPart / 1000000.0;
	frames++;
	avgFPS += 1.0f / (float)deltaTime;
}
void Time::DisplayFPSinWindowTitle(SDL_Window* _window)
{
	//use average fps over 10 frames
	if (frames > 10)
	{
		std::stringstream ss;
		ss << "OpenGL PBR. FPS: " << avgFPS/frames;
		SDL_SetWindowTitle(_window, ss.str().c_str());
		avgFPS = 0;
		frames = 0;
	}
}


