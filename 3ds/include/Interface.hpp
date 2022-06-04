#ifndef OTT_3DS_INTERFACE_HPP
#define OTT_3DS_INTERFACE_HPP

#include <3ds.h>
#include <stdio.h> // printf
#include <cstdint>

#define BIT(NUMBER) 1 << NUMBER

constexpr uint8_t N3DS_NUMBER_OF_BUTTONS = 32;

constexpr uint8_t N3DS_BUTTON_A = 0;
constexpr uint8_t N3DS_BUTTON_B = 1;
constexpr uint8_t N3DS_BUTTON_X = 10;
constexpr uint8_t N3DS_BUTTON_Y = 11;

constexpr uint8_t N3DS_BUTTON_SELECT = 2;
constexpr uint8_t N3DS_BUTTON_START = 3;

constexpr uint8_t N3DS_DPAD_RIGHT = 4;
constexpr uint8_t N3DS_DPAD_LEFT = 5;
constexpr uint8_t N3DS_DPAD_UP = 6;
constexpr uint8_t N3DS_DPAD_DOWN = 7;

constexpr uint8_t N3DS_SHOULDER_RIGHT = 8;
constexpr uint8_t N3DS_SHOULDER_LEFT = 9;
constexpr uint8_t N3DS_SHOULDER_ZLEFT = 14;
constexpr uint8_t N3DS_SHOULDER_ZRIGHT = 15;

constexpr uint8_t N3DS_TOUCH_SCREEN = 20;

constexpr uint8_t N3DS_CSTICK_RIGHT = 24;
constexpr uint8_t N3DS_CSTICK_LEFT = 25;
constexpr uint8_t N3DS_CSTICK_UP = 26;
constexpr uint8_t N3DS_CSTICK_DOWN = 27;

constexpr uint8_t N3DS_CPAD_RIGHT = 28;
constexpr uint8_t N3DS_CPAD_LEFT = 29;
constexpr uint8_t N3DS_CPAD_UP = 30;
constexpr uint8_t N3DS_CPAD_DOWN = 31;

class Interface {
	Interface() {
		gfxInitDefault();
	}

	~Interface() {
		gfxExit();
	}

	void initAudio() {
		ndspInit();

		ndspSetOutputMode(NDSP_OUTPUT_STEREO);

		ndspChnSetInterp(0, NDSP_INTERP_LINEAR);
		ndspChnSetRate(0, SAMPLERATE);
		ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);

		// Output at 100% on the first pair of left and right channels.
		float mix[12] = { 0.f };
		mix[0] = 1.0;
		mix[1] = 1.0;
		ndspChnSetMix(0, mix);
	}

	/** Check if a 3ds button was just pressed
	  */
	bool pressed(const uint8_t& key) const {
		return (nButtonsDown & BIT(key));
	}

	/** Check if a 3ds button is currently held
	  */
	bool held(const uint8_t& key) const {
		return (nButtonsHeld & BIT(key));
	}

	/** Check if a 3ds button is currently held
	  */
	bool released(const uint8_t& key) const {
		return (nButtonsUp & BIT(key));
	}

	/** Get the current position of the circle stick
	  */
	void stick(double& dx, double& dy) {
		dx = stick.dx;
		dy = stick.dy;
	}

	/** Execute main loop
	  * @return 0 if main loop exits gracefully
	  */
	int32_t execute() {
		if (!bInitSuccessful)
			return -1;
		while (aptMainLoop()) {
			// Process input events
			hidScanInput();
			hidCircleRead(&stick);

			nButtonsDown = hidKeysDown();
			nButtonsHeld = hidKeysHeld();
			nButtonsUp = hidKeysUp();

			// Execute the main loop
			if (!this->onUserLoop())
				break;

			gfxFlushBuffers();
			gfxSwapBuffers();

			gspWaitForVBlank();
		}
		return 0;
	}

protected:
	uint32_t nButtonsDown;

	uint32_t nButtonsHeld;

	uint32_t nButtonsUp;

	circlePosition stick;

	/** Method called every iteration of the main loop.
	  * If method returns false, main loop will exit.
	  */
	virtual bool onUserLoop() {
		return true;
	}
};

#endif // ifndef OTT_3DS_INTERFACE_HPP