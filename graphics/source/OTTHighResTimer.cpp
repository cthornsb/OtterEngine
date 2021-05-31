
#include "OTTHighResTimer.hpp"

OTTHighResTimer::OTTHighResTimer() :
	dTotalTime(0),
	nStops(0),
	tInitialization(hrclock::now()),
	tLastStart()
{
}

void OTTHighResTimer::start(){
	tLastStart = hrclock::now();
}

double OTTHighResTimer::stop(){
	nStops++;
	double retval = std::chrono::duration<double>(hrclock::now() - tLastStart).count();
	dTotalTime += retval;
	return retval;
}

double OTTHighResTimer::uptime() const {
	return std::chrono::duration<double>(hrclock::now() - tInitialization).count();
}

void OTTHighResTimer::reset(){
	dTotalTime = 0;
	nStops = 0;
}

