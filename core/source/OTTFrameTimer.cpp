#include <thread>

#include "OTTFrameTimer.hpp"

#ifdef USE_GLFW_TIMER
#include <GLFW/glfw3.h>
#endif // ifdef USE_GLFW_TIMER

#ifdef WIN32
#include <Windows.h>
#endif // ifdef WIN32

ott::FrameTimer::FrameTimer(const double& fps) :
	bEnabled(true),
	bQuitting(false),
	dFramerate(0),
	dFramerateCap(fps),
	dFramePeriod(1 / fps),
	dConstantOffset(0),
	nFrameCount(0),
#ifndef USE_GLFW_TIMER
	timeOfInitialization(hclock::now()),
	timeOfLastUpdate(hclock::now()),
#else
	dTimeOfInitialization(0),
	dTimeOfLastUpdate(0),
#endif // ifndef USE_GFLW_TIMER
	dCycleTimer(0),
	dTotalFrameTime(0),
	averageDeltaTime(1000),
	averageFramerate((unsigned short)(2 * fps)),
	averageFrameTime(1000)
{
#ifdef WIN32
	timeBeginPeriod(1);
#endif // ifdef WIN32
}

ott::FrameTimer::~FrameTimer() {
#ifdef WIN32
	timeEndPeriod(1);
#endif // ifdef WIN32
}

double ott::FrameTimer::TimeElapsed() const { 
#ifndef USE_GLFW_TIMER
	return std::chrono::duration_cast<std::chrono::duration<double> >(hclock::now() - this->timeOfInitialization).count();
#else
	return (glfwGetTime() - this->timeOfInitialization); // in seconds
#endif // ifndef USE_GLFW_TIMER
}

void ott::FrameTimer::SetFramerateCap(const double &fps){ 
	if (fps <= 0) {
		return;
	}
	this->dFramerateCap = fps;
	this->dFramePeriod = 1.0 / fps;
	this->averageFrameTime.Reset(); // Reset frame delta time since the frame period has changed
}

void ott::FrameTimer::SetFrameratePeriod(const double& period) {
	this->dFramePeriod = period;
	this->dFramerateCap = 1.0 / this->dFramePeriod;
	this->averageFrameTime.Reset(); // Reset frame delta time since the frame period has changed
}

void ott::FrameTimer::DisableFramerateCap() {
	this->dFramePeriod = 0;
	this->dFramerateCap = -1;
}

void ott::FrameTimer::Update(){
	// Update the timer
#ifndef USE_GLFW_TIMER
	this->timeOfLastUpdate = hclock::now();
#else
	this->timeOfLastUpdate = glfwGetTime(); // in seconds
#endif // ifndef USE_GLFW_TIMER
}

double ott::FrameTimer::Sync() {
	if (this->bEnabled == false) {
		return 0.0;
	}

	// Get the time since the frame started
#ifndef USE_GLFW_TIMER
	dTotalFrameTime = std::chrono::duration<double>(hclock::now() - timeOfLastUpdate).count(); // Total loop time, in seconds
#else
	dTotalFrameTime = glfwGetTime() - dTimeOfLastUpdate; // Total loop time, in seconds
#endif // ifndef USE_GLFW_TIMER

	// Cap the framerate by sleeping
	if (dFramerateCap > 0) {
		const double kTimeToSleep = dFramePeriod - dTotalFrameTime - (dConstantOffset + averageDeltaTime()); // Amount of time remaining until end of frame period (seconds)
		if (kTimeToSleep > 0) { // Sleep until end of frame period
			std::this_thread::sleep_for(std::chrono::microseconds((long long)(1E6 * kTimeToSleep)));

			// Compute the time elapsed since last call to sync()
#ifndef USE_GLFW_TIMER
			const double kExtraTime = std::chrono::duration<double>(hclock::now() - timeOfLastUpdate).count(); // Total frame time, in seconds
#else
			const double kExtraTime = glfwGetTime() - timeOfLastUpdate; // Total frame time, in seconds
#endif // ifndef USE_GLFW_TIMER
			averageDeltaTime.Add(kExtraTime - dTotalFrameTime - kTimeToSleep);
			dTotalFrameTime = kExtraTime;
		}
		// Update the time
#ifndef USE_GLFW_TIMER
		timeOfLastUpdate = hclock::now();
#else
		timeOfLastUpdate = glfwGetTime();
#endif // ifndef USE_GLFW_TIMER
	}

	// Update moving average frame time
	averageFrameTime.Add(dTotalFrameTime);

	// Update the moving average framerate with the instantaneous framerate
	averageFramerate.Add(1.0 / dTotalFrameTime);

	// Update the one-second timer
	nFrameCount++;
	dCycleTimer += dTotalFrameTime;
	if (dCycleTimer >= 1.0) {
		dFramerate = nFrameCount / dCycleTimer;
		nFrameCount = 0;
		dCycleTimer = 0;
	}

	return dTotalFrameTime;
}

void ott::FrameTimer::Execute(){
	this->bQuitting = false;
	while(this->bQuitting == false){
		this->Update();
		this->OnUserIdleTask();
		this->Sync();
	}
}

void ott::FrameTimer::ResetTimer() {
	this->bQuitting = false;
	this->dFramerate = 0;
	this->nFrameCount = 0;
	this->averageDeltaTime.Reset();
	this->averageFramerate.Reset();
	this->averageFrameTime.Reset();
#ifndef USE_GLFW_TIMER
	this->timeOfInitialization = hclock::now();
	this->timeOfLastUpdate = hclock::now();
#else
	this->dTimeOfInitialization = 0;
	this->dTimeOfLastUpdate = 0;
	this->glfwSetTime(0); // Reset GLFW timer
#endif // ifndef USE_GLFW_TIMER
	this->dCycleTimer = 0;
}

void ott::FrameTimer::Sleep(const long long& usec) {
	std::this_thread::sleep_for(std::chrono::microseconds((long long)usec));
}
