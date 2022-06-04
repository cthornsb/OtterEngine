
#include "OTTHighResTimer.hpp"

ott::HighResTimer::HighResTimer() :
	dTotalTime(0),
	nStops(0),
	tInitialization(hrclock::now()),
	tLastStart()
{
}

void ott::HighResTimer::Start(){
	this->tLastStart = hrclock::now();
}

double ott::HighResTimer::Stop(){
	this->nStops++;
	const double kDeltaTime = std::chrono::duration<double>(hrclock::now() - this->tLastStart).count();
	this->dTotalTime += kDeltaTime;
	return kDeltaTime;
}

double ott::HighResTimer::Uptime() const {
	return std::chrono::duration<double>(hrclock::now() - this->tInitialization).count();
}

void ott::HighResTimer::Reset(){
	this->dTotalTime = 0;
	this->nStops = 0;
}

