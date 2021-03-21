#include <iostream>
#include <thread>

#include "OTTFrameTimer.hpp"

double OTTFrameTimer::getTimeElapsed() const { 
	return std::chrono::duration_cast<std::chrono::duration<double> >(hclock::now() - timeOfInitialization).count();
}

void OTTFrameTimer::update(){
	// Update the timer
	timeOfLastUpdate = hclock::now();
}

double OTTFrameTimer::sync() {
	// Get the time since the frame started
	double frameTime = std::chrono::duration<double, std::micro>(hclock::now() - timeOfLastUpdate).count(); // in microseconds
	dTotalRenderTime += frameTime;

	// Update the total render time and the instantaneous framerate
	if ((++nFrameCount % (long long)dFramerateCap) == 0) { // Compute framerate (roughly once per second)
		dFramerate = nFrameCount / (dTotalRenderTime * 1.0E-6);
	}

	// Cap the framerate by sleeping
	if (dFramerateCap > 0) {
		long long timeToSleep = (long long)(dFramePeriod - frameTime); // microseconds
		if (timeToSleep > 0) {
			std::this_thread::sleep_for(std::chrono::microseconds(timeToSleep));
			dTotalRenderTime += timeToSleep;
		}
	}

	// Return the time elapsed since last call to sync()
	return std::chrono::duration<double>(hclock::now() - timeOfLastUpdate).count(); // in seconds
}

void OTTFrameTimer::execute(){
	bQuitting = false;
	while(!bQuitting){
		update();
		this->userIdleTask();
		sync();
	}
}

