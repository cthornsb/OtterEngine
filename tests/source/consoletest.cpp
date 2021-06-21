#include <iostream>
#include <cmath>

// Core
#include "OTTApplication.hpp"
#include "OTTCharacterMap.hpp"
#include "OTTConsole.hpp"
#include "OTTSystem.hpp" // ott::getAssetsPath

class AppTestConsole : public OTTConsole {
public:
	AppTestConsole() = delete;

	AppTestConsole(const unsigned short& W, const unsigned short& H, OTTCharacterMap* map) :
		OTTConsole(W, H, map),
		bInverted(false)
	{
		initialize();
	}

protected:
	bool bInverted;

	void onUserAddCommands() override {
		addConsoleCommand("cmd1", 1, 0, "<arg> [arg2]...]", "Command requiring at least one argument");
		addConsoleCommand("cmd2", 0, 1, "[args]...]", "Command with no required arguments");
		addConsoleCommand("invert", 0, 2, "", "Invert console font colors");
	}

	void onUserPrompt() override {
		put("console-> ");
	}

	bool onUserUnknownCommand(const std::vector<std::string>& args) override {
		std::cout << " Unknown command string: ";
		for (auto arg = args.begin(); arg != args.end(); arg++) {
			std::cout << *arg << " ";
		}
		std::cout << std::endl;
		return true;
	}

	void onUserHandleInput(ConsoleCommand* cmd, const std::vector<std::string>& args) override {
		std::cout << "cmd=" << cmd->getName() << ", args=" << args.size() << std::endl;
		for (auto arg = args.begin(); arg != args.end(); arg++) // Print all arguments
			std::cout << " arg=" << *arg << std::endl;
		if (cmd->getName() == "cmd1") {
			// cmd1 requires at least one argument
			// The OTTConsole class guarantees that if we got this far, we have at least one argument
			(*this) << " you entered \"" << args[1] << "\"\n";
		}
		else if (cmd->getName() == "cmd2") {
		}
		else if (cmd->getName() == "invert") { // Set the font foreground and background colors
			bInverted = !bInverted;
			if (bInverted) {
				cmap->setForegroundColor(Colors::BLACK);
				cmap->setBackgroundColor(Colors::WHITE);
			}
			else {
				cmap->setForegroundColor(Colors::WHITE);
				cmap->setBackgroundColor(Colors::BLACK);
			}
		}
	}
};

class AppTest : public OTTApplication {
public:
	AppTest() :
		OTTApplication(640, 480),
		nLoops(0),
		cmap(8, 16, 1),
		console(80, 30, &cmap)
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

		// Set the font foreground and background colors
		cmap.setForegroundColor(Colors::WHITE);
		cmap.setBackgroundColor(Colors::BLACK);

		// Set the output graphical window
		console.setOutputWindow(this);

		// Set keyboard stream buffer mode (text input)
		setKeyboardStreamMode();

		// Success
		return true;
	}

	bool onUserLoop() override {
		clear(); // Clear the screen

		// Update console text input
		console.update();

		// Draw the console to the image buffer
		console.draw(&buffer);

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

	AppTestConsole console;
};

int main(int argc, char* argv[]) {
	// Declare a new 2d application
	AppTest app;

	// Initialize application and open output window
	app.start(argc, argv);

	// Run the main loop
	return app.execute();
}