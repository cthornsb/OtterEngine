#include <iostream>

#include "OTTWindow.hpp"
#include "OTTFrameTimer.hpp"
#include "OTTGuiContainer.hpp"
#include "OTTGuiButton.hpp"
#include "OTTGuiRadioButton.hpp"
#include "OTTGuiCheckbox.hpp"
#include "OTTGuiSlider.hpp"
#include "OTTMouse.hpp"
#include "OTTTexture.hpp"

void buttonClicked(bool) {
	std::cout << " CLICK\n";
}

void stateChanged(bool) {
	std::cout << " TOGGLED\n";
}

void editingFinished(bool) {
	std::cout << " DONE\n";
}

int main() {
	// Open a new graphical window and enable mouse and keyboard support
	OTTWindow window(640, 480);
	window.initialize();
	window.enableKeyboard();
	window.enableMouse();
	window.enableImageBuffer(false);

	OTTTexture normal("Untitled.png");
	OTTTexture clicked("Clicked.png");
	std::cout << normal.getNumChannels() << " " << clicked.getNumChannels() << std::endl;

	OTTGuiContainer gui(100, 100, 320, 240);

	OTTGuiButton* button = new OTTGuiButton();
	button->setPosition(100, 100);
	button->setSize(50, 25);
	button->setLeftMouseButtonPressed(buttonClicked);

	OTTGuiCheckbox* checkbox = new OTTGuiCheckbox();
	checkbox->setPosition(175, 100);
	checkbox->setSize(50, 25);
	checkbox->setOnStateChanged(stateChanged);

	OTTGuiRadioButton* radio = new OTTGuiRadioButton();
	radio->setPosition(175, 200);
	radio->setSize(50, 50);
	radio->setOnStateChanged(stateChanged);

	OTTGuiSlider* slider = new OTTGuiSlider();
	slider->setPosition(250, 100);
	slider->setSize(50, 25);
	slider->setOnEditingFinished(editingFinished);

	checkbox->setNormalStateImage(&normal);
	checkbox->setActiveStateImage(&clicked);

	gui.add(button);
	gui.add(checkbox);
	gui.add(radio);
	gui.add(slider);

	// Get a pointer to the output RAM image buffer
	OTTImageBuffer* buffer = window.getBuffer();

	OTTGuiFrame frame(320, 240, 50, 25);
	std::vector<std::pair<unsigned char*, unsigned char*> > targets;
	size_t nTargets = OTTImageBuffer::getImageTargets(320, 240, buffer, &clicked, targets);

	// Get a pointer to the mouse controller
	OTTMouse* mouse = window.getMouse();

	OTTFrameTimer timer(60.0); // Set framerate cap to 60 fps
	double dTotalTime = 0;
	while (window.status()) {
		// Start the frame timer
		timer.update();

		// Clear the screen
		//window.clear();
		buffer->fill(0);

		// Draw the gui
		gui.draw(buffer);
		
		// Draw a test image
		for (size_t i = 0; i < nTargets; i++) {
			memcpy(targets[i].first, targets[i].second, 150);
		}

		// Update window events
		window.processEvents();

		// Check for button events
		gui.handleMouseEvents(mouse);

		// Draw the screen
		window.renderBuffer();

		/*static int count = 0;
		if (count++ % 120 == 0) // Frame count (every 2 seconds by default)
			std::cout << timer.getFramerate() << " fps\r" << std::flush;*/

		// Maintain consistent framerate
		dTotalTime += timer.sync();
	}

	return 0;
}