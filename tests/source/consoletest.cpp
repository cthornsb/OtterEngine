#include <graphics/app/OTTApplication.hpp>
#include <graphics/app/OTTCharacterMap.hpp>
#include <graphics/app/OTTConsole.hpp>
#include <core/OTTSystem.hpp> // ott::getAssetsPath

#include <iostream>
#include <cmath>

namespace ott {

class AppTestConsole : public Console {
public:
	AppTestConsole() = delete;

	AppTestConsole(const uint16_t& W, const uint16_t& H, CharacterMap* map) :
		Console(W, H, map),
		bInverted(false)
	{
		this->Initialize();
	}

protected:

	bool bInverted;

	void OnUserAddCommands() override {
		this->AddConsoleCommand("cmd1", 1, 0, "<arg> [arg2]...]", "Command requiring at least one argument");
		this->AddConsoleCommand("cmd2", 0, 1, "[args]...]", "Command with no required arguments");
		this->AddConsoleCommand("invert", 0, 2, "", "Invert console font colors");
	}

	void OnUserPrompt() override {
		this->Put("console-> ");
	}

	bool OnUserUnknownCommand(const std::vector<std::string>& args) override {
		std::cout << " Unknown command string: ";
		for (auto arg = args.begin(); arg != args.end(); arg++) {
			std::cout << *arg << " ";
		}
		std::cout << std::endl;
		return true;
	}

	void OnUserHandleInput(ConsoleCommand* cmd, const std::vector<std::string>& args) override {
		std::cout << "cmd=" << cmd->Name() << ", args=" << args.size() << std::endl;
		for (auto arg = args.begin(); arg != args.end(); arg++) // Print all arguments
			std::cout << " arg=" << *arg << std::endl;
		if (cmd->Name() == "cmd1") {
			// cmd1 requires at least one argument
			// The OTTConsole class guarantees that if we got this far, we have at least one argument
			(*this) << " you entered \"" << args[1] << "\"\n";
		}
		else if (cmd->Name() == "cmd2") {
		}
		else if (cmd->Name() == "invert") { // Set the font foreground and background colors
			bInverted = !bInverted;
			if (bInverted) {
				cmap->SetForegroundColor(colors::Black);
				cmap->SetBackgroundColor(colors::White);
			}
			else {
				cmap->SetForegroundColor(colors::White);
				cmap->SetBackgroundColor(colors::Black);
			}
		}
	}
};

class AppTest : public Application {
public:

	AppTest() :
		Application(640, 480),
		nLoops(0),
		cmap(8, 16, 1),
		console(80, 30, &cmap)
	{
		std::ifstream ifile(ott::AssetsPath("zap-vga16.psf").c_str(), std::ios::binary);
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

		// Set the font foreground and background colors
		cmap.SetForegroundColor(colors::White);
		cmap.SetBackgroundColor(colors::Black);

		// Set the output graphical window
		console.SetOutputWindow(this);

		// Set keyboard stream buffer mode (text input)
		this->SetKeyboardStreamMode();

		// Success
		return true;
	}

	bool OnUserLoop() override {
		this->Clear(); // Clear the screen

		// Update console text input
		console.Update();

		// Draw the console to the image buffer
		console.Draw(&buffer);

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

	AppTestConsole console;
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