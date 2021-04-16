#ifndef OTT_APPLICATION_HPP
#define OTT_APPLICATION_HPP

#include "OTTWindow.hpp"
#include "OTTFrameTimer.hpp"

class OTTApplication : public OTTWindow, public OTTFrameTimer {
public:
	/** Default constructor
	  */
	OTTApplication();

	/** Default 640 x 480 window size with arguments.
	  * Window is automatically initialized, but main loop is not started.
	  */
	OTTApplication(int argc, char* argv[]);

	/** Custom window size
	  */
	OTTApplication(const int& W, const int& H);

	/** Custom window size with arguments.
	  * Window is automatically initialized, but main loop is not started.
	  */
	OTTApplication(const int& W, const int& H, int argc, char* argv[]);

	/** Destructor
	  */
	virtual ~OTTApplication();

	/** Get the total time elapsed since the application was constructed (in seconds)
	  */
	double getTotalElapsedTime() const {
		return dTotalTime;
	}

	/** Initialize application and open display window (not to be confused with execute())
	  */
	bool start(int argc, char* argv[]);

	/** Execute main loop
	  * @return 0 if main loop exits gracefully
	  */
	int execute();

protected:
	bool bInitSuccessful; ///< Set when application and window initialized successfully

	double dTotalTime; ///< Total time elapsed since application was constructed (seconds)

	/** Method called immediately after initializing output graphical window
	  */
	virtual bool onUserCreateWindow() {
		return true;
	}

	/** Method called when input configuration file is loaded
	  */
	virtual bool onUserLoadConfigFile() {
		return true;
	}

	/** Method called every iteration of the main loop.
	  * If method returns false, main loop will exit.
	  */
	virtual bool onUserLoop() {
		return true;
	}
};

#endif