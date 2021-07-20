#ifndef OTT_FRAME_TIMER_HPP
#define OTT_FRAME_TIMER_HPP

#include <vector>

#include "MovingAverage.hpp"

#ifndef USE_GLFW_TIMER

#include <chrono>

// Make a typedef for clarity when working with chrono.
typedef std::chrono::high_resolution_clock hclock;

#endif // ifndef USE_GFLW_TIMER

/// <summary>
/// High resolution frame render timer which will attempt to maintain a target 
/// number of cycles per second. Uses the chrono library on Unix systems, and 
/// uses the high precision GLFW timer on Windows systems (since calls to chrono
/// may take on the order of milliseconds).
/// </summary>
class OTTFrameTimer{
public:
	/** Default constructor (60 fps cap)
	  */
	OTTFrameTimer() :
		OTTFrameTimer(60.0)
	{
	}

	/** Target framerate constructor
	  * @param fps Target framerate (Hz), or uncapped for fps < 0
	  */
	OTTFrameTimer(const double& fps);

	/** Destructor.
	  * Does nothing on Unix platforms. On Windows, calls timeEndPeriod(1).
	  */
	~OTTFrameTimer();

	/** Get the total time elapsed since the scene was initialized (in seconds)
	  */
	double getTimeElapsed() const;

	/** Get the average amount of time for each render (in seconds)
	  */
	double getAverageRenderTime() const { 
		return averageDeltaTime();
	}

	/** Get the instantaneous framerate (in frames per second) of the most recent frame update
	  */
	double getFramerate() const { 
		return dFramerate; 
	}

	/** Get the moving average framerate over the last 1000 frames
	  */
	double getAverageFramerate() const {
		return averageFramerate();
	}

	/** Get the current framerate cap (in frames per second, fps)
	  */
	double getFramerateCap() const {
		return dFramerateCap;
	}

	/** Get the current frame period (in seconds)
	  */
	double getFramePeriod() const {
		return dFramePeriod;
	}

	/** Get the current frame period time offset (in seconds)
	  */
	double getFramePeriodOffet() const {
		return dConstantOffset;
	}

	/** Get the total number of frames processed so far
	  */
	unsigned long long getFrameCount() const {
		return nFrameCount;
	}

	/** Set the constant frame period time offset to fine tune frame timing (in microseconds).
	  * Setting the frame period offset will disable time offset averaging. 
	  * Set the period to <= 0 to re-enable default time averaging behavior.
	  */
	void setFramePeriodOffset(const double& offset) {
		dConstantOffset = offset / 1E6;
	}

	/** Set the target maximum framerate for rendering (in Hz)
	  * @param fps Target number of frames per second (fps > 0)
	  */
	void setFramerateCap(const double &fps);

	/** Set the period for each successive frame (in seconds)
	  */
	void setFrameratePeriod(const double& period);

	/** Disable frame timer, frames will be rendered as fast as possible
	  */
	void disableFramerateCap();

	/** Exit the main loop started by execute()
	  */
	void quit(){
		bQuitting = true;
	}

	/** Restart frame timer
	  */
	void enable() {
		bEnabled = true;
		update(); // Reset timer
	}

	/** Stop frame timer
	  */
	void disable() {
		bEnabled = false;
	}

	/** Update the time elapsed since last loop.
	  * Intended to be used in conjunction with sync() to control framerate.
	  */
	void update();

	/** Sync the frame timer to the requested framerate.
	  * Must be used in conjunction with update() in order to control framerate.
	  * @return The time elapsed since last update() was called (in seconds)
	  */
	double sync();
	
	/** Start main loop.
	  * Execution will continue until quit() is called.
	  */
	void execute();

	/** Reset the timer
	  */
	void resetTimer();

	/** Sleep for a specified number of microseconds.
	  * Note that high-precision timing resolution is platform dependent and sleeping for very short
	  * periods of time may not be possible on some systems.
	  */
	static void sleep(const long long& usec);

protected:
	bool bEnabled; ///< Set if frame timer is running

	bool bQuitting; ///< Set if the main execution loop should exit

	double dFramerate; ///< The instantaneous framerate of the last render

	double dFramerateCap; ///< The target render framerate (in Hz)

	double dFramePeriod; ///< Target frame period (seconds)

	double dConstantOffset; ///< Constant frame period time offset (seconds)

	unsigned long long nFrameCount; ///< The number of frames which have been rendered

#ifndef USE_GLFW_TIMER
	hclock::time_point timeOfInitialization; ///< The time that the scene was initialized

	hclock::time_point timeOfLastUpdate; ///< The last time that update() was called by the user
#else
	double dTimeOfInitialization; ///< The time that the scene was initialized

	double dTimeOfLastUpdate; ///< The last time that update() was called by the user
#endif // ifndef USE_GFLW_TIMER

	double dCycleTimer; ///< The time taken to execute nCycleTimerPeriod update() cycles

	double dTotalFrameTime; ///< The total time taken for the most recent frame (i.e. between successive sync() calls, in seconds)

	ott::MovingAverage averageDeltaTime; ///< Average extra time due to high-res timing and sleep calls (in seconds)

	ott::MovingAverage averageFramerate; ///< Average framerate

	ott::MovingAverage averageFrameTime; ///< Average frame period (in seconds)

	/** Function to be executed once per main loop when execute() is called
	  */
	virtual void userIdleTask() { 
	}
};

#endif

