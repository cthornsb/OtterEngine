#ifndef OTT_FRAME_TIMER_HPP
#define OTT_FRAME_TIMER_HPP

#include <vector>

#ifdef WIN32
#define USE_GLFW_TIMER
#endif

#ifndef USE_GLFW_TIMER

#include <chrono>

// Make a typedef for clarity when working with chrono.
typedef std::chrono::high_resolution_clock hclock;

#endif // ifndef USE_GFLW_TIMER

class MovingAverage {
public:
	/** Default constructor (100 values)
	  */
	MovingAverage() :
		bFull(false),
		nValues(0),
		dTotal(0),
		dValues(100, 0),
		iter(dValues.begin())
	{
	}

	/** N value constructor
	  */
	MovingAverage(const unsigned short& N) :
		bFull(false),
		nValues(0),
		dTotal(0),
		dValues(N, 0),
		iter(dValues.begin())
	{
	}

	/** Get the current running average
	  */
	double operator () () const {
		return (nValues > 0 ? (dTotal / nValues) : 0);
	}

	/** Add a new value to the running average and subtract the oldest value
	  * @return The current running average
	  */
	double operator () (const double& value);

	/** Add a new value to the running average and subtract the oldest value
	  */
	void add(const double& value);

	/** Reset the current running average and all saved values
	  */
	void reset();

private:
	bool bFull; ///< Set when the moving average is full of values

	unsigned short nValues; ///< Total number of values to average

	double dTotal; ///< Current running average

	std::vector<double> dValues; ///< Vector of running average values

	std::vector<double>::iterator iter; ///< The next value which will be modified

	/** Recompute the total sum to remove any rounding errors which may have been introduced
	  */
	void recount();
};

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

	/** Get the average amount of time for each render (in microseconds)
	  */
	double getAverageRenderTime() const { 
		return averageCycleTime();
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

	/** Set the target maximum framerate for rendering (in Hz)
	  */
	void setFramerateCap(const unsigned short &fps){ 
		dFramerateCap = fps; 
		dFramePeriod = 1E6 / fps;
	}

	/** Set the period for each successive frame (in microseconds)
	  */
	void setFrameratePeriod(const double& period) {
		dFramePeriod = period;
		dFramerateCap = 1E6 / dFramePeriod;
	}

	/** Exit the main loop started by execute()
	  */
	void quit(){
		bQuitting = true;
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

protected:
	bool bQuitting; ///< Set if the main execution loop should exit

	double dFramerate; ///< The instantaneous framerate of the last render

	double dFramerateCap; ///< The target render framerate (in Hz)

	double dFramePeriod; ///< Target frame period (microseconds)

	unsigned long long nFrameCount; ///< The number of frames which have been rendered

#ifndef USE_GLFW_TIMER
	hclock::time_point timeOfInitialization; ///< The time that the scene was initialized

	hclock::time_point timeOfLastUpdate; ///< The last time that update() was called by the user

	hclock::time_point cycleTimer; ///< The time taken to execute nCycleTimerPeriod update() cycles
#else
	double dTimeOfInitialization; ///< The time that the scene was initialized

	double dTimeOfLastUpdate; ///< The last time that update() was called by the user

	double dCycleTimer; ///< The time taken to execute nCycleTimerPeriod update() cycles
#endif // ifndef USE_GFLW_TIMER

	MovingAverage averageCycleTime; ///< Moving average cycle time

	MovingAverage averageFramerate; ///< Moving average framerate

	MovingAverage averageDeltaTime; ///< Moving average difference between target frame period and actual frame period

	/** Function to be executed once per main loop when execute() is called
	  */
	virtual void userIdleTask() { }
};

#endif

