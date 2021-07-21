#include <thread>

#include "OTTFrameTimer.hpp"

#ifdef USE_GLFW_TIMER
#include <GLFW/glfw3.h>
#endif // ifdef USE_GLFW_TIMER

#ifdef WIN32
#include <Windows.h>
#endif // ifdef WIN32

OTTFrameTimer::OTTFrameTimer(const double& fps) :
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

OTTFrameTimer::~OTTFrameTimer() {
#ifdef WIN32
	timeEndPeriod(1);
#endif // ifdef WIN32
}

double OTTFrameTimer::getTimeElapsed() const { 
#ifndef USE_GLFW_TIMER
	return std::chrono::duration_cast<std::chrono::duration<double> >(hclock::now() - timeOfInitialization).count();
#else
	return (glfwGetTime() - dTimeOfInitialization); // in seconds
#endif // ifndef USE_GLFW_TIMER
}

void OTTFrameTimer::setFramerateCap(const double &fps){ 
	if (fps <= 0)
		return;
	dFramerateCap = fps; 
	dFramePeriod = 1 / fps;
	averageFrameTime.reset(); // Reset frame delta time since the frame period has changed
}

void OTTFrameTimer::setFrameratePeriod(const double& period) {
	dFramePeriod = period;
	dFramerateCap = 1 / dFramePeriod;
	averageFrameTime.reset(); // Reset frame delta time since the frame period has changed
}

void OTTFrameTimer::disableFramerateCap() {
	dFramePeriod = 0;
	dFramerateCap = -1;
}

void OTTFrameTimer::update(){
	// Update the timer
#ifndef USE_GLFW_TIMER
	timeOfLastUpdate = hclock::now();
#else
	dTimeOfLastUpdate = glfwGetTime(); // in seconds
#endif // ifndef USE_GLFW_TIMER
}

double OTTFrameTimer::sync() {
	if (!bEnabled)
		return 0.0;

	// Get the time since the frame started
#ifndef USE_GLFW_TIMER
	dTotalFrameTime = std::chrono::duration<double>(hclock::now() - timeOfLastUpdate).count(); // Total loop time, in seconds
#else
	dTotalFrameTime = glfwGetTime() - dTimeOfLastUpdate; // Total loop time, in seconds
#endif // ifndef USE_GLFW_TIMER

	// Cap the framerate by sleeping
	if (dFramerateCap > 0) {
		double timeToSleep = dFramePeriod - dTotalFrameTime - (dConstantOffset + averageDeltaTime()); // Amount of time remaining until end of frame period (seconds)
		if (timeToSleep > 0) { // Sleep until end of frame period
			std::this_thread::sleep_for(std::chrono::microseconds((long long)(1E6 * timeToSleep)));

			// Compute the time elapsed since last call to sync()
#ifndef USE_GLFW_TIMER
			double dExtraTime = std::chrono::duration<double>(hclock::now() - timeOfLastUpdate).count(); // Total frame time, in seconds
#else
			double dExtraTime = glfwGetTime() - dTimeOfLastUpdate; // Total frame time, in seconds
#endif // ifndef USE_GLFW_TIMER
			averageDeltaTime.add(dExtraTime - dTotalFrameTime - timeToSleep);
			dTotalFrameTime = dExtraTime;
		}
		// Update the time
#ifndef USE_GLFW_TIMER
		timeOfLastUpdate = hclock::now();
#else
		dTimeOfLastUpdate = glfwGetTime();
#endif // ifndef USE_GLFW_TIMER
	}

	// Update moving average frame time
	averageFrameTime.add(dTotalFrameTime);

	// Update the moving average framerate with the instantaneous framerate
	averageFramerate.add(1.0 / dTotalFrameTime);

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

void OTTFrameTimer::execute(){
	bQuitting = false;
	while(!bQuitting){
		update();
		this->userIdleTask();
		sync();
	}
}

void OTTFrameTimer::resetTimer() {
	bQuitting = false;
	dFramerate = 0;
	nFrameCount = 0;
	averageDeltaTime.reset();
	averageFramerate.reset();
	averageFrameTime.reset();
#ifndef USE_GLFW_TIMER
	timeOfInitialization = hclock::now();
	timeOfLastUpdate = hclock::now();
#else
	dTimeOfInitialization = 0;
	dTimeOfLastUpdate = 0;
	glfwSetTime(0); // Reset GLFW timer
#endif // ifndef USE_GLFW_TIMER
	dCycleTimer = 0;
}

void OTTFrameTimer::sleep(const long long& usec) {
	std::this_thread::sleep_for(std::chrono::microseconds((long long)usec));
}
