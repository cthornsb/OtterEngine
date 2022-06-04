#ifndef CORE_OTT_HIGH_RES_TIMER_HPP
#define CORE_OTT_HIGH_RES_TIMER_HPP

#include <chrono>

// Make a typedef for clarity when working with chrono.
typedef std::chrono::high_resolution_clock hrclock;

namespace ott {

class HighResTimer{
public:
	HighResTimer();

	/** Get the average elapsed time for each start / stop cycle (in seconds)
	  */
	double operator () () {
		return (dTotalTime / nStops);
	}
	
	/** Get the number of start / stop cycles
	  */
	unsigned int Cycles() const {
		return nStops;
	}
	
	/** Start the timer
	  */
	void Start();
	
	/** Stop the timer and return the time elapsed since start() called (in seconds)
	  * Time elapsed is added to total time accumulator.
	  */
	double Stop();
	
	/** Get total time since timer initialization (in seconds)
	  */
	double Uptime() const;
	
	/** Reset time accumulator value
	  */
	void Reset();
	
private:
	double dTotalTime; ///< Total time between all starts and stops (in seconds)
	
	unsigned int nStops; ///< Number of times the timer has been started and stopped

	hrclock::time_point tInitialization; ///< The time that the timer was initialized
	
	hrclock::time_point tLastStart; ///< The most recent time that the timer was started
};

} /* namespace ott */

#endif // #ifndef CORE_OTT_HIGH_RES_TIMER_HPP
