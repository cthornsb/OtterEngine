#include "OTTApplication.hpp"

OTTApplication::OTTApplication() :
	OTTWindow(640, 480),
	OTTFrameTimer(),
	bInitSuccessful(false),
	dTotalTime(0)
{
}

OTTApplication::OTTApplication(int argc, char* argv[]) :
	OTTWindow(640, 480),
	OTTFrameTimer(),
	bInitSuccessful(false),
	dTotalTime(0)
{
	start(argc, argv);
}

OTTApplication::OTTApplication(const int& W, const int& H) :
	OTTWindow(W, H),
	OTTFrameTimer(),
	bInitSuccessful(false),
	dTotalTime(0)
{
}

OTTApplication::OTTApplication(const int& W, const int& H, int argc, char* argv[]) :
	OTTWindow(W, H),
	OTTFrameTimer(),
	bInitSuccessful(false),
	dTotalTime(0)
{
	start(argc, argv);
}

OTTApplication::~OTTApplication() {
}

bool OTTApplication::start(int argc, char* argv[]) {
	if (bInitSuccessful) // Already initialized
		return true;
	if (!initialize("OTTApplication"))
		return false;
	bInitSuccessful = this->onUserCreateWindow();
	return bInitSuccessful;
}

int OTTApplication::execute() {
	if (!bInitSuccessful)
		return -1;
	while (status()) {
		// Start the frame timer
		update();

		// Process OpenGL events
		processEvents();

		// Execute the main loop
		if (!this->onUserLoop())
			break;

		// Maintain consistent framerate
		dTotalTime += sync();
	}
	return 0;
}
