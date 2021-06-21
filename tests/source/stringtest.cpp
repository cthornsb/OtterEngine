#include <iostream>
#include <sstream>
#include <random>

// Core
#include "OTTApplication.hpp"
#include "OTTCharacterMap.hpp"
#include "OTTString.hpp"
#include "OTTSystem.hpp" // ott::getAssetsPath

class AppTest : public OTTApplication {
public:
	AppTest() :
		OTTApplication(640, 480),
		nLoops(0),
		cmap(8, 16, 1),
		strings()
	{
		std::ifstream ifile(ott::getAssetsPath("zap-vga16.psf").c_str(), std::ios::binary);
		ifile.seekg(4, std::ios::beg); // Skip the header
		if (!cmap.loadCharacterBitmaps(ifile)) {
			std::cout << " [error] Failed to load font file." << std::endl;
		}
	}

	~AppTest() override {
		// Nothing to clean up, window will be closed by OTTWindow class
	}

protected:
	bool onUserCreateWindow() override {
		// Enable mouse and keyboard support
		enableKeyboard();
		enableMouse();
		enableImageBuffer(false);

		// Set pixel color blend mode (required for opacity blending)
		buffer.setBlendMode(BlendMode::ADD);

		// Set the font foreground and background colors
		cmap.setForegroundColor(Colors::WHITE);
		cmap.setBackgroundColor(Colors::BLACK);

		strings.setCharacterMap(&cmap);
		strings.add("Long living string", 0, 0, 8.f, 3.f);
		strings.add("Shorter lived string", 100, 100, 5.f, 6.f);

		std::cout << " Press space to create new string" << std::endl;

		// Success
		return true;
	}

	bool onUserLoop() override {
		buffer.fill(0); // Clear output image buffer

		if (keys.poll(0x20)) { // Spacebar			
			int x = (1.f * rand() / RAND_MAX) * 640;
			int y = (1.f * rand() / RAND_MAX) * 480;
			std::stringstream stream;
			stream << " Dynamic: x=" << x << ", y=" << y;
			strings.add(stream.str(), x, y, 5.f, 2.f);
		}

		// Draw all strings to the image buffer
		strings.update(&buffer);
		//strings.update(&buffer, (float)dTotalTime);

		if (nLoops++ % 120 == 0) { // Frame count (every 2 seconds by default)
			std::stringstream stream;
			stream << " console - " << getFramerate() << " fps";
			setWindowTitle(stream.str());
		}

		// Draw the screen
		renderBuffer();

		return true;
	}

private:
	int nLoops; ///< Total number of main loops executed

	OTTCharacterMap cmap;

	OTTStringManager strings;
};

int main(int argc, char* argv[]) {
	// Declare a new 2d application
	AppTest app;

	// Initialize application and open output window
	app.start(argc, argv);

	// Run the main loop
	return app.execute();
}