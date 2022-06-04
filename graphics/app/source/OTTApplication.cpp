#include "OTTApplication.hpp"

ott::Application::Application() :
	Window(640, 480),
	FrameTimer(),
	bInitSuccessful(false),
	dTotalTime(0)
{
}

ott::Application::Application(int argc, char* argv[]) :
	Window(640, 480),
	FrameTimer(),
	bInitSuccessful(false),
	dTotalTime(0)
{
	this->Start(argc, argv);
}

ott::Application::Application(const int& W, const int& H) :
	Window(W, H),
	FrameTimer(),
	bInitSuccessful(false),
	dTotalTime(0)
{
}

ott::Application::Application(const int& W, const int& H, int argc, char* argv[]) :
	Window(W, H),
	FrameTimer(),
	bInitSuccessful(false),
	dTotalTime(0)
{
	this->Start(argc, argv);
}

ott::Application::~Application() {
}

bool ott::Application::Start(int argc, char* argv[]) {
	if (bInitSuccessful) { // Already initialized
		return true;
	}
	if (this->Initialize("ott::Application") == false) {
		return false;
	}
	bInitSuccessful = this->OnUserCreateWindow();
	return bInitSuccessful;
}

int ott::Application::Execute() {
	if (bInitSuccessful == false) {
		return -1;
	}
	while (this->Status() == true) {
		// Start the frame timer
		this->Update();

		// Process OpenGL events
		this->ProcessEvents();

		// Execute the main loop
		if (!this->OnUserLoop())
			break;

		// Maintain consistent framerate
		dTotalTime += this->Sync();
	}
	return 0;
}
