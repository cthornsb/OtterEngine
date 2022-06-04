#include <graphics/app/OTTApplication.hpp>
#include <graphics/app/OTTCharacterMap.hpp>
#include <graphics/app/OTTString.hpp>
#include <core/OTTSystem.hpp> // ott::getAssetsPath
#include <input/OTTKeyboard.hpp>
#include <math/OTTRandom.hpp>

#include <iostream>
#include <sstream>

namespace ott {

class AppTest : public Application {
public:
	AppTest() :
		Application(640, 480),
		nLoops(0),
		cmap(8, 16, 1),
		strings()
	{
		const std::string kBitmapPath = ott::AssetsPath("zap-vga16.psf");
		std::cout << kBitmapPath << std::endl;
		std::ifstream ifile(kBitmapPath.c_str(), std::ios::binary);
		ifile.seekg(4, std::ios::beg); // Skip the header
		if (cmap.LoadCharacterBitmaps(ifile) == false) {
			std::cout << " [error] Failed to load font file." << std::endl;
		}
	}

	~AppTest() override {
		// Nothing to clean up, window will be closed by OTTWindow class
	}

protected:

	bool OnUserCreateWindow() override {
		// Enable mouse and keyboard support
		this->EnableKeyboard();
		this->EnableMouse();
		this->EnableImageBuffer(false);

		// Set pixel color blend mode (required for opacity blending)
		buffer.SetBlendMode(BlendMode::Add);

		// Set the font foreground and background colors
		cmap.SetForegroundColor(colors::White);
		cmap.SetBackgroundColor(colors::Black);

		strings.SetCharacterMap(&cmap);
		strings.Add("Long living string", 0, 0, 8.f, 3.f);
		strings.Add("Shorter lived string", 100, 100, 5.f, 6.f);

		random.Seed();

		std::cout << " Press space to create new string" << std::endl;

		// Success
		return true;
	}

	bool OnUserLoop() override {
		buffer.Fill(0); // Clear output image buffer

		if (keys->Poll(0x20)) { // Spacebar			
			const uint32_t x = random(640);
			const uint32_t y = random(480);
			std::stringstream stream;
			stream << " Dynamic: x=" << x << ", y=" << y;
			strings.Add(stream.str(), x, y, 5.f, 2.f);
		}

		// Draw all strings to the image buffer
		strings.Update(&buffer);
		//strings.update(&buffer, (float)dTotalTime);

		if (nLoops++ % 120 == 0) { // Frame count (every 2 seconds by default)
			std::stringstream stream;
			stream << " console - " << this->Framerate() << " fps";
			this->SetWindowTitle(stream.str());
		}

		// Draw the screen
		this->RenderBuffer();

		return true;
	}

private:
	int32_t nLoops; ///< Total number of main loops executed

	CharacterMap cmap;

	StringManager strings;

	Random random;
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