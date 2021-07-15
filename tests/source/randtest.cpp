#include <iostream>

// Core
#include "OTTApplication.hpp"

// Graphics
#include "OTTHighResTimer.hpp"

// Math
#include "OTTRandom.hpp"

class AppTest : public OTTApplication {
public:
	AppTest() :
		OTTApplication(512, 512),
		generator(),
		timer()
	{
	}

	~AppTest() override {
		// Nothing to clean up, window will be closed by OTTWindow class
	}

protected:
	void fillImageBuffer() {
		for (int i = 0; i < 512; i++) { // Over columns
			for (int j = 0; j < 512; j++) { // Over rows
				buffer.setPixel(i, j, (generator(2) == 1 ? Colors::WHITE : Colors::BLACK));
			}
		}
	}

	bool onUserCreateWindow() override {
		// Enable keyboard support
		enableKeyboard();
		enableImageBuffer(false);

		std::cout << " Press 'g' to switch psuedo random number generator" << std::endl;
		std::cout << " Press 't' to generate one new noise map and print the time taken" << std::endl;
		std::cout << " Press 's' to re-seed generator" << std::endl;
		std::cout << " Hold space to generate new noise maps" << std::endl;

		// Success
		return true;
	}

	bool onUserLoop() override {
		clear(); // Clear the screen

		if (keys.check(0x20)) { // Spacebar	
			fillImageBuffer();
		}
		if (keys.poll('t')) { // Fill the entire frame with random noise and display the amount of time taken
			timer.start();
			fillImageBuffer();
			double dElapsedTime = timer.stop();
			std::cout << "  dT=" << dElapsedTime << ", " << 1E6 * dElapsedTime / 65536.0 << " us / pixel" << std::endl;
		}
		if (keys.poll('g')) { // Cycle through all availables generators
			switch (generator.getGenerator()) {
			case OTTRandom::Generator::DEFAULT:
				generator.setGenerator(OTTRandom::Generator::XORSHIFT);
				std::cout << " [randtest] Generator switched to XORSHIFT" << std::endl;
				break;
			case OTTRandom::Generator::XORSHIFT:
				generator.setGenerator(OTTRandom::Generator::DEFAULT);
				std::cout << " [randtest] Generator switched to DEFAULT" << std::endl;
				break;
			default:
				break;
			}
		}
		if (keys.poll('s')) { // Seed RNG
			generator.seed();
		}

		// Draw the screen
		renderBuffer();

		return true;
	}

private:
	OTTRandom generator;

	OTTHighResTimer timer;
};

int main(int argc, char* argv[]) {
	// Declare a new 2d application
	AppTest app;

	// Initialize application and open output window
	app.start(argc, argv);

	// Run the main loop
	return app.execute();
}