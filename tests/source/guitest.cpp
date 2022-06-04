#include <iostream>
#include <cmath>

// Core
#include <graphics/app/OTTApplication.hpp>

// Gui
#include <gui/OTTGuiButton.hpp>
#include <gui/OTTGuiCheckbox.hpp>
#include <gui/OTTGuiContainer.hpp>
#include <gui/OTTGuiRadioButton.hpp>
#include <gui/OTTGuiSlider.hpp>

// Math
#include <math/OTTWrappedValue.hpp>
//#include "DecayEnvelope.hpp"

namespace ott {

void ButtonClicked(GuiElement*, bool);
void StateChanged(GuiElement*, bool);
void EditingFinished(GuiElement*, bool);

class SpriteApp : public Application {
public:

	SpriteApp() :
		Application(640, 480),
		gui(100, 100, 320, 240),
		button(),
		checkbox(),
		radio(),
		sliders{
			GuiSlider(0.f, -1.f, 1.f),
			GuiSlider(0.f, -1.f, 1.f),
			GuiSlider(0.f,  0.f, 1.f),
			GuiSlider(0.f,  0.f, 1.f)
		},
		theta(0.f, 0.f, 6.28318f)
		//envelope1(),
		//envelope2()
	{
	}

	~SpriteApp() override {
		// Nothing to clean up, window will be closed by OTTWindow class
	}

	void Clicked() {
		// "Press" the buttons
		//envelope1.trigger(1.f);
		//envelope2.trigger(1.f);

		// Release them
		//envelope1.release();
		//envelope2.release();
	}

protected:

	bool OnUserCreateWindow() override {
		// Enable mouse and keyboard support
		this->EnableKeyboard();
		this->EnableMouse();
		this->EnableImageBuffer(false);

		button.SetPosition(100, 100);
		button.SetSize(50, 25);
		button.SetLeftMouseButtonPressed(ButtonClicked);

		checkbox.SetPosition(175, 100);
		checkbox.SetSize(50, 25);
		checkbox.SetOnStateChanged(StateChanged);

		radio.SetPosition(175, 200);
		radio.SetSize(50, 50);
		radio.SetOnStateChanged(StateChanged);

		gui.Add(&button);
		gui.Add(&checkbox);
		gui.Add(&radio);

		for(int32_t i = 0; i < 4; i++){
			sliders[i].SetPosition(250, 100 + 25 * i);
			sliders[i].SetSize(50, 25);
			sliders[i].SetOnEditingFinished(EditingFinished);
			gui.Add(&sliders[i]);
		}

		// Select this window
		this->SetCurrent();
		
		// Set decay acceleration for envelopes
		//envelope1.setReleaseSlope(0.15f);
		//envelope2.setReleaseSlope(0.125f);
		
		std::cout << " Press escape to quit" << std::endl;

		// Success
		return true;
	}

	bool OnUserLoop() override {
		// Clear the image buffer
		buffer.Fill(0);

		// Draw the gui
		gui.Draw(&buffer);
		
		// Update window events
		//processEvents(); // Not needed, OTTApplication calls this every loop

		// Check for button events
		gui.HandleMouseEvents(mouse);
		
		theta += 1.f * dTotalFrameTime;
		sliders[0].SetValue(std::sin(theta.Get()));
		sliders[1].SetValue(std::cos(theta.Get()));
		//sliders[2].SetValue(envelope1.get());
		//sliders[3].SetValue(envelope2.get());
		
		//envelope1.update(dTotalFrameTime);
		//envelope2.update(dTotalFrameTime);
		
		// Draw the screen
		this->RenderBuffer();

		return true;
	}

private:
	GuiContainer gui;

	GuiButton button;

	GuiCheckbox checkbox;

	GuiRadioButton radio;

	GuiSlider sliders[4];
	
	WrappedValue theta;
	
	//DecayEnvelope envelope1;
	
	//DecayEnvelope envelope2;
};

} /* namespace ott */

// Declare a new 2d application
ott::SpriteApp app;

void ott::ButtonClicked(GuiElement*, bool state) {
	if (state) {
		app.Clicked();
	}
}

void ott::StateChanged(GuiElement*, bool) {
	std::cout << " TOGGLED\n";
}

void ott::EditingFinished(GuiElement*, bool) {
	std::cout << " DONE\n";
}

int32_t main(int32_t argc, char* argv[]) {
	// Initialize application and open output window
	app.Start(argc, argv);

	// Run the main loop
	return app.Execute();
}
