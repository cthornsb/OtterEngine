#include <thread>

#include "OTTFrameTimer.hpp"

#ifdef USE_GLFW_TIMER
#include <GLFW/glfw3.h>
#endif // ifdef WIN32

#ifdef WIN32
#include <Windows.h>
#endif // ifdef WIN32

double MovingAverage::operator () (const double& value) {
	add(value);
	return (dTotal / nValues);
}

void MovingAverage::add(const double& value) {
	dTotal += (value - *iter);
	*iter = value;
	iter++;
	if (!bFull)
		nValues++;
	if (iter == dValues.end()) {
		iter = dValues.begin();
		bFull = true;
	}
}

void MovingAverage::reset() {
	std::fill(dValues.begin(), dValues.end(), 0);
	bFull = false;
	nValues = 0;
	dTotal = 0;
	iter = dValues.begin();
}

void MovingAverage::recount() {
	dTotal = 0;
	for (auto val = dValues.cbegin(); val != dValues.cend(); val++)
		dTotal += (*val);
}

OTTFrameTimer::OTTFrameTimer(const double& fps) :
	bQuitting(false),
	dFramerate(0),
	dFramerateCap(fps),
	dFramePeriod(1E6 / fps),
	nFrameCount(0),
#ifndef USE_GLFW_TIMER
	timeOfInitialization(hclock::now()),
	timeOfLastUpdate(hclock::now()),
#else
	dTimeOfInitialization(0),
	dTimeOfLastUpdate(0),
#endif // ifndef USE_GFLW_TIMER
	dCycleTimer(0),
	averageCycleTime(1000),
	averageFramerate((unsigned short)(2 * fps)),
	averageDeltaTime(1000)
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
	dFramePeriod = 1E6 / fps;
	averageDeltaTime.reset(); // Reset frame delta time since the frame period has changed
}

void OTTFrameTimer::setFrameratePeriod(const double& period) {
	dFramePeriod = period;
	dFramerateCap = 1E6 / dFramePeriod;
	averageDeltaTime.reset(); // Reset frame delta time since the frame period has changed
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
	// Get the time since the frame started
#ifndef USE_GLFW_TIMER
	double frameTime = std::chrono::duration<double, std::micro>(hclock::now() - timeOfLastUpdate).count(); // Total loop time, in microseconds
#else
	double frameTime = 1E6 * (glfwGetTime() - dTimeOfLastUpdate); // Total loop time, in microseconds
#endif // ifndef USE_GLFW_TIMER

	// Update the moving average loop time
	averageCycleTime.add(frameTime);

	// Cap the framerate by sleeping
	if (dFramerateCap > 0) {
		double timeToSleep = (dFramePeriod - averageDeltaTime()) - frameTime; // Amount of time remaining until end of frame period (microseconds)
		if (timeToSleep > 0) { // Sleep until end of frame period
			std::this_thread::sleep_for(std::chrono::microseconds((long long)timeToSleep));
		}
	}

	// Compute the time elapsed since last call to sync()
	double totalFrameTime = 0;
#ifndef USE_GLFW_TIMER
	totalFrameTime = std::chrono::duration<double>(hclock::now() - timeOfLastUpdate).count(); // Total frame time, in seconds
	timeOfLastUpdate = hclock::now(); // update()
#else
	totalFrameTime = glfwGetTime() - dTimeOfLastUpdate; // Total frame time, in seconds
	dTimeOfLastUpdate = glfwGetTime(); // update()
#endif // ifndef USE_GLFW_TIMER

	// Update the moving average framerate with the instantaneous framerate
	averageFramerate.add(1.0 / totalFrameTime);

	// Update moving average of difference between target frame period and actual frame period
	if (dFramerateCap > 0)
		averageDeltaTime.add(1E6 * totalFrameTime - dFramePeriod);

	// Update the one-second timer
	nFrameCount++;
	dCycleTimer += totalFrameTime;
	if (dCycleTimer >= 1.0) {
		dFramerate = nFrameCount / dCycleTimer;
		nFrameCount = 0;
		dCycleTimer = 0;
	}

	return totalFrameTime;
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
	averageCycleTime.reset();
	averageFramerate.reset();
	averageDeltaTime.reset();
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
