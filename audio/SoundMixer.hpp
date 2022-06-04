#ifndef SOUND_MIXER_HPP
#define SOUND_MIXER_HPP

#include "SoundBuffer.hpp"
#include "UnitTimer.hpp"

namespace ott {
	/** Clamp an input value to the range [low, high]
	  */
	float clamp(const float& input, const float& low = 0.f, const float& high = 1.f);
}

class SoundMixer : public UnitTimer, public SoundBuffer {
public:
	/** Default constructor
	  */
	SoundMixer() :
		UnitTimer(1),
		SoundBuffer(),
		bMuted(false),
		bModified(false),
		bMonoOutput(false),
		nOutputChannels(2),
		nInputChannels(2),
		fMasterVolume(1.f),
		fOffsetDC(0.f),
		fOutputVolume(2, 1.f),
		fOutputSamples(2, 0.f),
		fInputVolume(2, 1.f),
		fInputSamples(2, 0.f),
		bSendInputToOutput(2, std::vector<bool>(2, false))
	{ 
		bEnabled = true; // Enable timer
		reload(); // Refill counter so timer starts immediately
	}

	/** Stereo output constructor
	  * @param ch Number of input audio channels
	  */
	SoundMixer(const uint32_t& input) :
		UnitTimer(1),
		SoundBuffer(),
		bMuted(false),
		bModified(false),
		bMonoOutput(false),
		nOutputChannels(2),
		nInputChannels(input),
		fMasterVolume(1.f),
		fOffsetDC(0.f),
		fOutputVolume(2, 1.f),
		fOutputSamples(2, 0.f),
		fInputVolume(input, 1.f),
		fInputSamples(input, 0.f),
		bSendInputToOutput(2, std::vector<bool>(input, false))
	{ 
		bEnabled = true; // Enable timer
		reload(); // Refill counter so timer starts immediately
	}

	/** Generic input / output constructor
	  * @param input Number of input audio channels
	  * @param output Number of output audio channels
	  */
	SoundMixer(const uint32_t& input, const uint32_t& output) :
		UnitTimer(1),
		SoundBuffer(),
		bMuted(false),
		bModified(false),
		bMonoOutput(false),
		nOutputChannels(output),
		nInputChannels(input),
		fMasterVolume(1.f),
		fOffsetDC(0.f),
		fOutputVolume(output, 1.f),
		fOutputSamples(output, 0.f),
		fInputVolume(input, 1.f),
		fInputSamples(input, 0.f),
		bSendInputToOutput(output, std::vector<bool>(input, false))
	{ 
		bEnabled = true; // Enable timer
		reload(); // Refill counter so timer starts immediately
	}

	/** Destructor
	  */
	~SoundMixer() { 
	}

	/** Direct element access operator for current raw output samples (no output volume applied)
	  */
	float operator [](const uint32_t& ch) const {
		return fOutputSamples[ch];
	}

	/** Get the current sample for one of the output channels.
	  * Apply master output volume, output channel volumes, and translate to range [-DC, 1].
	  * If an invalid output channel number is specified, 0 will be returned.
	  */
	float get(const uint32_t& ch) const ;

	/** Get the current sample for one of the output channels
	  * @return True if ch is a valid output channel number
	  */
	bool get(const uint32_t& ch, float& sample) const ;

	/** Get a pointer to the input sample buffer
	  */
	float* getSampleBuffer(){
		return fInputSamples.data();
	}

	/** Get the current master output volume
	  */
	float getVolume() const {
		return fMasterVolume;
	}
	
	/** Get the number of input audio channels
	  */
	int32_t getNumInputChannels() const {
		return nInputChannels;
	}
	
	/** Get the number of output audio channels
	  */
	int32_t getNumOutputChannels() const {
		return nOutputChannels;
	}
	
	/** Return true if audio output is muted and return false otherwise
	  */
	bool isMuted() const {
		return bMuted;
	}

	/** Mute or un-mute master output
	  * @return True if the output is muted and return false otherwise
	  */
	bool mute(){
		return (bMuted = !bMuted);
	}

	/** Set master output volume
	  * Volume clamped to range [0, 1]
	  */
	void setVolume(const float& volume){
		if(bMuted) // Un-mute
			bMuted = false;
		fMasterVolume = ott::clamp(volume);
		if(fMasterVolume == 0.f)
			bMuted = true;
	}

	/** Increase master output volume by a specified amount (clamped to between 0 and 1)
	  */
	void increaseVolume(const float& change = 0.1f){
		setVolume(fMasterVolume + change);
	}
	
	/** Decrease master output volume by a specified amount (clamped to between 0 and 1)
	  */
	void decreaseVolume(const float& change = 0.1f){
		setVolume(fMasterVolume - change);
	}
	
	/** Set the volume for one of the input channels
	  */
	void setInputLevel(const uint32_t& ch, const float& volume){
		if(ch < nInputChannels)
			fInputVolume[ch] = ott::clamp(volume);
	}
	
	/** Set the volume of all input channels
	  */
	void setInputLevels(const float& volume){
		for(uint32_t i = 0; i < nInputChannels; i++)
			fInputVolume[i] = ott::clamp(volume);
	}
	
	/** Set the volume for one of the output channels
	  */
	void setOutputLevel(const uint32_t& ch, const float& volume){
		if(ch < nOutputChannels)
			fOutputVolume[ch] = ott::clamp(volume);
	}
	
	/** Set the volume of all output channels
	  */
	void setOutputLevels(const float& volume){
		for(uint32_t i = 0; i < nOutputChannels; i++)
			fOutputVolume[i] = ott::clamp(volume);
	}

	/** Set the negative DC offset of the output audio effectively making the output up to 100% louder (default is 0)
	  * Offset clamped to range [0, 1]
	  */
	void setOffsetDC(const float& offset){
		fOffsetDC = ott::clamp(offset);
	}

	/** For mixers with two output channels (left and right), set the left / right audio output balance.
	  * A value of -1 is 100% left and 0% right, 0 is 100% left and 100% right, +1 is 0% left and 100% right.
	  * Does nothing if mixer does not have two output channels.
	  */
	void setBalance(const float& bal);
	
	/** Enable or disable mono audio output. 
	  * If enabled, all output channels will be averaged together into a single output signal.
	  */
	void setMonoOutput(bool state){
		bMonoOutput = state;
	}
	
	/** Send an audio input channel to an audio output channel
	  * @param input Desired input channel
	  * @param output Desired output channel
	  */
	void setInputToOutput(const uint32_t& input, const uint32_t& output, bool state=true){
		bSendInputToOutput[output][input] = state;
	}
	
	/** Set the audio sample for a specified channel (clamped to -1 to 1)
	  * Also sets the mixer's modified flag
	  */
	void setInputSample(const uint32_t& ch, const float& vol){
		bModified = true;
		fInputSamples[ch] = ott::clamp(vol, -1.f);
	}
	
	/** Reset mixer timer to zero
	  */
	void reset() override;
	
private:
	bool bMuted; ///< Audio output muted by user

	bool bModified; ///< Flag indiciating that one or more input samples were modified

	bool bMonoOutput; ///< Stereo output flag

	uint32_t nOutputChannels; ///< Number of input audio channels
	
	uint32_t nInputChannels; ///< Number of output audio channels

	float fMasterVolume; ///< Master output volume
	
	float fOffsetDC; ///< "DC" offset of output audio waveform (in range 0 to 1)
	
	std::vector<float> fOutputVolume; ///< Output channel volume

	std::vector<float> fOutputSamples; ///< Raw output audio sample buffer with no output volume modification

	std::vector<float> fInputVolume; ///< Volumes for all input audio channels

	std::vector<float> fInputSamples; ///< Audio sample buffer for audio inputs

	std::vector<std::vector<bool> > bSendInputToOutput; ///< Flags for which input signals will be sent to which output signals
	
	/** Update output audio samples
	  * This update should occur any time an audio unit clocks over.
	  * @return True if at least one of the input samples was modified and return false otherwise
	  */
	bool update();
	
	/** Push the current output sample onto the fifo buffer
	  */
	void rollover() override ;
};

#endif
