#ifndef OTT_FRAME_TIMER_HPP
#define OTT_FRAME_TIMER_HPP

#include <chrono>

// Make a typedef for clarity when working with chrono.
typedef std::chrono::high_resolution_clock hclock;

class OTTFrameTimer{
public:
	/** Default constructor (60 fps cap)
	  */
	OTTFrameTimer() :
		bQuitting(false),
		dFramerate(0), 
		dFramerateCap(60.0),
		dTotalRenderTime(0),
		dFramePeriod(1E6 / 60.0),
		nFrameCount(0),
		timeOfInitialization(hclock::now()),
		timeOfLastUpdate(hclock::now())
	{
	}

	/** Target framerate constructor
	  * @param fps Target framerate (Hz), or uncapped for fps < 0
	  */
	OTTFrameTimer(const double& fps) :
		bQuitting(false),
		dFramerate(0), 
		dFramerateCap(fps),
		dTotalRenderTime(0),
		dFramePeriod(1E6 / fps),
		nFrameCount(0),
		timeOfInitialization(hclock::now()),
		timeOfLastUpdate(hclock::now())	
	{
	}

	/** Get the total time elapsed since the scene was initialized (in seconds)
	  */
	double getTimeElapsed() const;

	/** Get the average amount of time for each render (in microseconds)
	  */
	double getAverageRenderTime() const { 
		return (dTotalRenderTime / nFrameCount); 
	}

	/** Get the instantaneous framerate of the most recent render (in Hz)
	  */
	double getFramerate() const { 
		return dFramerate; 
	}

	/** Set the target maximum framerate for rendering (in Hz)
	  */
	void setFramerateCap(const unsigned short &fps){ 
		dFramerateCap = fps; 
		dFramePeriod = 1E6 / fps;
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

protected:
	bool bQuitting;

	double dFramerate; ///< The instantaneous framerate of the last render

	double dFramerateCap; ///< The target render framerate (in Hz)

	double dTotalRenderTime; ///< The running total time of all render events (microseconds)

	double dFramePeriod; ///< Target frame period (microseconds)

	unsigned long long nFrameCount; ///< The number of frames which have been rendered

	hclock::time_point timeOfInitialization; ///< The time that the scene was initialized

	hclock::time_point timeOfLastUpdate; ///< The last time that update() was called by the user
	
	/** Function to be executed once per main loop when execute() is called
	  */
	virtual void userIdleTask() { }
};

#endif

