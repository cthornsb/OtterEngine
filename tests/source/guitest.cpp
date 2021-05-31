#include <iostream>
#include <cmath>

// Core
#include "OTTApplication.hpp"

// Gui
#include "OTTGuiContainer.hpp"
#include "OTTGuiButton.hpp"
#include "OTTGuiRadioButton.hpp"
#include "OTTGuiCheckbox.hpp"
#include "OTTGuiSlider.hpp"

// Math
#include "WrappedValue.hpp"
#include "DecayEnvelope.hpp"

void buttonClicked(bool);

void stateChanged(bool);

void editingFinished(bool);

class SpriteApp : public OTTApplication {
public:
	SpriteApp() :
		OTTApplication(640, 480),
		gui(100, 100, 320, 240),
		button(),
		checkbox(),
		radio(),
		sliders{
			OTTGuiSlider(0.f, -1.f, 1.f),
			OTTGuiSlider(0.f, -1.f, 1.f),
			OTTGuiSlider(0.f,  0.f, 1.f),
			OTTGuiSlider(0.f,  0.f, 1.f)
		},
		theta(0.f, 0.f, 6.28318f),
		envelope1(),
		envelope2()
	{
	}

	~SpriteApp() override {
		// Nothing to clean up, window will be closed by OTTWindow class
	}

	void clicked() {
		// "Press" the buttons
		envelope1.trigger(1.f);
		envelope2.trigger(1.f);

		// Release them
		envelope1.release();
		envelope2.release();
	}

protected:
	bool onUserCreateWindow() override {
		// Enable mouse and keyboard support
		enableKeyboard();
		enableMouse();
		enableImageBuffer(false);

		button.setPosition(100, 100);
		button.setSize(50, 25);
		button.setLeftMouseButtonPressed(buttonClicked);

		checkbox.setPosition(175, 100);
		checkbox.setSize(50, 25);
		checkbox.setOnStateChanged(stateChanged);

		radio.setPosition(175, 200);
		radio.setSize(50, 50);
		radio.setOnStateChanged(stateChanged);

		gui.add(&button);
		gui.add(&checkbox);
		gui.add(&radio);

		for(int i = 0; i < 4; i++){
			sliders[i].setPosition(250, 100 + 25 * i);
			sliders[i].setSize(50, 25);
			sliders[i].setOnEditingFinished(editingFinished);
			gui.add(&sliders[i]);
		}

		// Select this window
		setCurrent();
		
		// Set decay acceleration for envelopes
		envelope1.setReleaseSlope(0.15f);
		envelope2.setReleaseSlope(0.125f);
		
		std::cout << " Press escape to quit" << std::endl;

		// Success
		return true;
	}

	bool onUserLoop() override {
		// Clear the image buffer
		buffer.fill(0);

		// Draw the gui
		gui.draw(&buffer);
		
		// Update window events
		processEvents();

		// Check for button events
		gui.handleMouseEvents(&mouse);
		
		theta += 1.f * dTotalFrameTime;
		sliders[0].setValue(std::sin(theta.get()));
		sliders[1].setValue(std::cos(theta.get()));
		sliders[2].setValue(envelope1.get());
		sliders[3].setValue(envelope2.get());
		
		envelope1.update(dTotalFrameTime);
		envelope2.update(dTotalFrameTime);
		
		// Draw the screen
		renderBuffer();

		return true;
	}

private:
	OTTGuiContainer gui;

	OTTGuiButton button;

	OTTGuiCheckbox checkbox;

	OTTGuiRadioButton radio;

	OTTGuiSlider sliders[4];
	
	WrappedValue theta;
	
	ott::DecayEnvelope envelope1;
	
	ott::DecayEnvelope envelope2;
};

// Declare a new 2d application
SpriteApp app;

void buttonClicked(bool state) {
	if(state)
		app.clicked();	
}

void stateChanged(bool) {
	std::cout << " TOGGLED\n";
}

void editingFinished(bool) {
	std::cout << " DONE\n";
}

int main(int argc, char* argv[]) {
	// Initialize application and open output window
	app.start(argc, argv);

	// Run the main loop
	return app.execute();
}
