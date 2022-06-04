#ifndef AUDIO_SAMPLER_HPP
#define AUDIO_SAMPLER_HPP

#include "PianoKeys.hpp"

#include "DecayEnvelope.hpp"

class AudioSampler : public ott::DecayEnvelope {
public:
	/** Default constructor
	  */
	AudioSampler() :
		ott::DecayEnvelope(),
		bUseVolumeEnvelope(false),
		bPlaying(false),
		fAmplitude(1.f),
		fFrequency(440.f),
		fPeriod(1.f / 440.f),
		fPhase(0.f),
		fPhaseStep(1.f / 44100.f),
		fCurrentValue(0.f)
	{
	}

	/** Destructor
	  */
	virtual ~AudioSampler() {
	}

	/** Set maximum waveform amplitude
	  */
	void setAmplitude(const float& A);

	/** Set audio waveform frequency to that of a natural music note
	  */
	void setFrequency(const PianoKeys::Key& key, const int32_t& octave = 4) {
		setFrequency(PianoKeys::Keyboard::getFrequency(key, PianoKeys::Modifier::NONE, octave));
	}

	/** Set audio waveform frequency to that of a natural, sharp, or flat music note
	  */
	void setFrequency(const PianoKeys::Key& key, const PianoKeys::Modifier& mod, const int32_t& octave = 4) {
		setFrequency(PianoKeys::Keyboard::getFrequency(key, mod, octave));
	}

	/** Set audio waveform frequency (in Hz)
	  */
	void setFrequency(const float& freq) {
		fFrequency = freq;
		fPeriod = 1.f / freq;
	}

	/** Set audio output sample rate (in Hz)
	  */
	void setSampleRate(const float& freq) {
		fPhaseStep = 1.f / freq;
	}

	/** Enable or disable output volume envelope.
	  * If enabled, volume envelope will be used to moderate waveform amplitude.
	  */
	void useVolumeEnvelope(bool state = true) {
		bUseVolumeEnvelope = state;
	}

	/** Enable audio output
	  */
	void play() {
		bPlaying = true;
	}

	/** Disable audio output.
	  * Sampling the waveform will always return zero, waveform phase will not increase.
	  */
	void pause() {
		bPlaying = false;
	}

	/** Get the current maximum waveform amplitude
	  */
	float getAmplitude() const {
		return fAmplitude;
	}

	/** Get the current volume envelope output volume
	  */
	float getVolume() const {
		return fValue;
	}

	/** Get the current audio waveform frequency (in Hz)
	  */
	float getFrequency() const {
		return fFrequency;
	}

	/** Get the current audio waveform period (in seconds)
	  */
	float getPeriod() const {
		return (1.f / fFrequency);
	}

	/** Get a pointer to the output volume envelope
	  */
	ott::DecayEnvelope* getVolumeEnvelope() {
		return reinterpret_cast<ott::DecayEnvelope*>(this);
	}

	/** Sample output audio wave, after incrementing the phase a specified amount, and return the result
	  */
	float sample(const float& dt);

	/** Sample the audio waveform N times, incrementing the phase by the sample rate phase step before each sample.
	  * Output array must contain AT LEAST N elements.
	  */
	void sample(const float& dt, float* arr, const uint32_t& N);

	/** Sample output audio wave, after incrementing the phase by the sample rate phase step, and return the result
	  */
	float sample() {
		return sample(fPhaseStep);
	}

	/** Sample the audio waveform N times, after incrementing the phase by the sample rate phase step before each sample.
	  * Output array must contain AT LEAST N elements.
	  */
	void sample(float* arr, const uint32_t& N) {
		sample(fPhaseStep, arr, N);
	}

	/** Reset audio waveform
	  */
	virtual void reset() {
		fPhase = 0.f;
	}

protected:
	bool bUseVolumeEnvelope; ///< Set if volume envelope will moderate amplitude of audio waveform

	bool bPlaying; ///< Set if wav file is currently being played

	float fAmplitude; ///< Maximum amplitude of audio waveform (clamped between 0 and 1)

	float fFrequency; ///< Frequency of audio waveform (in Hz)

	float fPeriod; ///< Period of audio waveform (in seconds)

	float fPhase; ///< Current phase of audio waveform (in seconds)

	float fPhaseStep; ///< Standard phase step of waveform based on sampling frequency (in seconds)

	float fCurrentValue; ///< Current waveform output

	/** Increment the phase of the output waveform.
	  * If incrementing the phase causes the waveform period to roll over, call userPhaseRollover().
	  */
	void incrementPhase(const float& dt);

	/** Sample output audio wave, after incrementing the phase a specified amount, and return the result
	  */
	virtual float userSample(const float& dt) = 0;

	/** Called when waveform phase rolls over.
	  * Does nothing by default.
	  */
	virtual void userPhaseRollover() {
	}
};

#endif // ifndef AUDIO_SAMPLER_HPP