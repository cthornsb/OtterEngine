#ifndef CORE_OTT_APPLICATION_HPP
#define CORE_OTT_APPLICATION_HPP

#include <core/OTTFrameTimer.hpp>
#include <graphics/core/OTTWindow.hpp>

namespace ott {

class Application : public Window, public FrameTimer {
public:
	/** Default constructor
	  */
	Application();

	/** Default 640 x 480 window size with arguments.
	  * Window is automatically initialized, but main loop is not started.
	  */
	Application(int argc, char* argv[]);

	/** Custom window size
	  */
	Application(const int& W, const int& H);

	/** Custom window size with arguments.
	  * Window is automatically initialized, but main loop is not started.
	  */
	Application(const int& W, const int& H, int argc, char* argv[]);

	/** Destructor
	  */
	virtual ~Application();

	/** Get the total time elapsed since the application was constructed (in seconds)
	  */
	double TotalElapsedTime() const {
		return dTotalTime;
	}

	/** Initialize application and open display window (not to be confused with execute())
	  */
	bool Start(int argc, char* argv[]);

	/** Execute main loop
	  * @return 0 if main loop exits gracefully
	  */
	int Execute();

protected:
	bool bInitSuccessful; ///< Set when application and window initialized successfully

	double dTotalTime; ///< Total time elapsed since application was constructed (seconds)

	/** Method called immediately after initializing output graphical window
	  */
	virtual bool OnUserCreateWindow() {
		return true;
	}

	/** Method called when input configuration file is loaded
	  */
	virtual bool OnUserLoadConfigFile() {
		return true;
	}

	/** Method called every iteration of the main loop.
	  * If method returns false, main loop will exit.
	  */
	virtual bool OnUserLoop() {
		return true;
	}
};

} /* namespace ott */

#endif // #ifndef CORE_OTT_APPLICATION_HPP
