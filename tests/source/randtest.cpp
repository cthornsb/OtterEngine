#include <core/OTTHighResTimer.hpp>
#include <graphics/app/OTTApplication.hpp>
#include <input/OTTKeyboard.hpp>
#include <math/OTTRandom.hpp>

#include <iostream>

namespace ott {

class AppTest : public Application {
public:
	AppTest() :
		Application(512, 512),
		generator(),
		timer()
	{
	}

	~AppTest() override {
		// Nothing to clean up, window will be closed by OTTWindow class
	}

protected:
	void FillImageBuffer() {
		for (int32_t i = 0; i < 512; i++) { // Over columns
			for (int32_t j = 0; j < 512; j++) { // Over rows
				buffer.SetPixel(i, j, (generator(2) == 1 ? colors::White : colors::Black));
			}
		}
	}

	bool OnUserCreateWindow() override {
		// Enable keyboard support
		this->EnableKeyboard();
		this->EnableImageBuffer(false);

		std::cout << " Press 'g' to switch psuedo random number generator" << std::endl;
		std::cout << " Press 't' to generate one new noise map and print the time taken" << std::endl;
		std::cout << " Press 's' to re-seed generator" << std::endl;
		std::cout << " Hold space to generate new noise maps" << std::endl;

		// Success
		return true;
	}

	bool OnUserLoop() override {
		this->Clear(); // Clear the screen

		if (keys->Check(0x20)) { // Spacebar	
			this->FillImageBuffer();
		}
		if (keys->Poll('t')) { // Fill the entire frame with random noise and display the amount of time taken
			timer.Start();
			this->FillImageBuffer();
			double dElapsedTime = timer.Stop();
			std::cout << "  dT=" << dElapsedTime << ", " << 1E6 * dElapsedTime / 65536.0 << " us / pixel" << std::endl;
		}
		if (keys->Poll('g')) { // Cycle through all availables generators
			switch (generator.GetGenerator()) {
			case Random::Generator::Default:
				generator.SetGenerator(Random::Generator::XorShift);
				std::cout << " [randtest] Generator switched to XORSHIFT" << std::endl;
				break;
			case Random::Generator::XorShift:
				generator.SetGenerator(Random::Generator::Default);
				std::cout << " [randtest] Generator switched to DEFAULT" << std::endl;
				break;
			default:
				break;
			}
		}
		if (keys->Poll('s')) { // Seed RNG
			generator.Seed();
		}

		// Draw the screen
		this->RenderBuffer();

		return true;
	}

private:
	Random generator;

	HighResTimer timer;
};

} /* namespace ott */

int32_t main(int32_t argc, char* argv[]) {
	// Declare a new 2d application
	ott::AppTest app;

	// Initialize application and open output window
	app.Start(argc, argv);

	// Run the main loop
	return app.Execute();
}