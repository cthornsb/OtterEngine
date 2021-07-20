#include <iostream>
#include <vector>

// Core
#include "OTTApplication.hpp"

// Audio
#define ENABLE_PORT_AUDIO // Must be defined BEFORE including SoundManager
#include "SoundManager.hpp"
#include "SoundMixer.hpp"
#include "Synthesizers.hpp"

// Math
#include "OTTRandom.hpp"

// GUI
#include "OTTGuiSlider.hpp"

AudioSampler* synth;

int audioCallback(const void*, void* output, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* data) {
	float* out = reinterpret_cast<float*>(output);
	SoundMixer* audio = reinterpret_cast<SoundMixer*>(data);
	for (unsigned long i = 0; i < framesPerBuffer; i++) {
		audio->copySample(synth->sample());
	}
	audio->getSamples(out, framesPerBuffer);
	return 0;
}

class AppTest : public OTTApplication {
public:
	AppTest() :
		OTTApplication(512, 512),
		dPulseWidth(0.0005),
		audio(&SoundManager::getInstance()),
		mixer(1, 1),
		sine(),
		square(),
		triangle(),
		sawtooth(),
		squarePulse(),
		trapPulse(),
		noise(),
		bufferCapacity(0, 0.f, 4096.f),
		noteVolume(0.f, 0.f, 1.f),
		masterVolume(0.f, 0.f, 1.f),
		vSynths({ &sine, &square, &triangle, &sawtooth, &squarePulse, &trapPulse, &noise }),
		iter(vSynths.begin())
	{
	}

	~AppTest() override {
		// Nothing to clean up, window will be closed by OTTWindow class
	}

protected:
	bool onUserCreateWindow() override {
		// Enable keyboard support
		enableKeyboard();
		enableImageBuffer(false);

		std::cout << " Press '-' or '+' to decrease / increase waveform amplitude" << std::endl;
		std::cout << " Press '[' or ']' to decrease / increase waveform frequency" << std::endl;
		std::cout << " Press '<' or '>' to decrease / increase trapezoid pulse width" << std::endl;
		std::cout << " Press 's' to switch synthesizer type" << std::endl;
		std::cout << " Press 'r' to reset output waveform" << std::endl;
		std::cout << " Press and hold 'space' to play note" << std::endl;

		// Set default output volume
		mixer.setVolume(0.0125f);

		// Set default synthesizer
		synth = &sine;

		// Initialize audio
		audio->setCallbackFunction(audioCallback);
		audio->init(&mixer);
		audio->start();

		bufferCapacity.setPosition(10, 10);
		bufferCapacity.setSize(492, 50);
		noteVolume.setPosition(10, 70);
		noteVolume.setSize(241, 50);
		masterVolume.setPosition(261, 70);
		masterVolume.setSize(241, 50);

		// Enable volume envelopes
		for (auto unit = vSynths.begin(); unit != vSynths.end(); unit++) {
			(*unit)->useVolumeEnvelope();
			(*unit)->setAttackTime(0.0625);
			(*unit)->setDecayTime(0.0625);
			(*unit)->setReleaseSlope(3.0);
			(*unit)->setAmplitude(0.025f);
		}

		// Set initial trapezoid pulse width
		trapPulse.setPulseWidth(dPulseWidth);

		// Set noise frequency
		noise.setFrequency(8000);

		// Success
		return true;
	}

	bool onUserLoop() override {
		clear(); // Clear the screen
		buffer.fill(0); // Clear image buffer

		if (keys.poll('-')) { // Decrease volume
			(*iter)->setAmplitude((*iter)->getAmplitude() - 0.0125f);
		}
		if (keys.poll('=')) { // Increase volume
			(*iter)->setAmplitude((*iter)->getAmplitude() + 0.0125f);
		}
		if (keys.check('[')) { // Decrease frequency
			(*iter)->setFrequency((*iter)->getFrequency() - 1.f);
		}
		if (keys.check(']')) { // Increase frequency
			(*iter)->setFrequency((*iter)->getFrequency() + 1.f);
		}
		if (keys.check(',')) { // Decrease trapezoid pulse width
			trapPulse.setPulseWidth(dPulseWidth -= 0.00001);
		}
		if (keys.check('.')) { // Increase trapezoid pulse width
			trapPulse.setPulseWidth(dPulseWidth += 0.00001);
		}
		if (keys.poll('s')) { // Switch synth
			iter++;
			if (iter == vSynths.end())
				iter = vSynths.begin();
			synth = *iter;
		}
		if (keys.poll('r')) { // Reset waveform
			(*iter)->reset();
		}
		if (keys.check(0x20)) { // Play note
			(*iter)->play();
			(*iter)->trigger(1.f);
		}
		else if ((*iter)->isHeld()) {
			(*iter)->release();
		}

		bufferCapacity.setValue((float)mixer.getNumberOfSamples());
		bufferCapacity.draw(&buffer);
		noteVolume.setValue((*iter)->getVolume());
		noteVolume.draw(&buffer);
		masterVolume.setValue((*iter)->getAmplitude());
		masterVolume.draw(&buffer);

		// Draw the screen
		renderBuffer();

		return true;
	}

private:
	double dPulseWidth;

	SoundManager* audio;

	SoundMixer mixer;

	Synthesizers::SineWave sine;

	Synthesizers::SquareWave square;

	Synthesizers::TriangleWave triangle;

	Synthesizers::SawtoothWave sawtooth;

	Synthesizers::SquarePulse squarePulse;

	Synthesizers::TrapezoidPulse trapPulse;

	Synthesizers::Noise noise;

	OTTGuiSlider bufferCapacity;

	OTTGuiSlider noteVolume;

	OTTGuiSlider masterVolume;

	std::vector<AudioSampler*> vSynths;

	std::vector<AudioSampler*>::iterator iter;
};

int main(int argc, char* argv[]) {
	// Declare a new 2d application
	AppTest app;

	// Initialize application and open output window
	app.start(argc, argv);

	// Run the main loop
	return app.execute();
}