#ifndef SOUND_BUFFER_HPP
#define SOUND_BUFFER_HPP

#include <queue>
#include <mutex>

#include "portaudio.h"

class SoundBuffer {
public:
	/** Default constructor
	  */
	SoundBuffer() :
		nSamplesPerBuffer(2048),
		nOutputChannels(2),
		nSamples(0),
		vEmpty(2),
		samples(2, std::queue<float>())
	{
	}

	/** Output channel constructor
	  */
	SoundBuffer(const size_t& N) :
		nSamplesPerBuffer(2048),
		nOutputChannels(N),
		nSamples(0),
		vEmpty(N),
		samples(N, std::queue<float>())
	{
	}

	/** Destructor
	  */
	~SoundBuffer() { 
	}
	
	/** Copy constructor (deleted)
	  */
	SoundBuffer(const SoundBuffer&) = delete;
	
	/** Copy operator (deleted)
	  */
	SoundBuffer& operator = (const SoundBuffer&) = delete;

	/** Get the number of output audio channels
	  */
	size_t getNumberOfChannels() const {
		return nOutputChannels;
	}

	/** Get the number of audio samples currently in the output buffer
	  */
	size_t getNumberOfSamples() const {
		return nSamples;
	}

	/** Resize output buffer to a specified number of channels.
	  * Any existing audio data will be deleted.
	  */
	void setNumberOfChannels(const size_t& N);

	/** Set the expected number of audio samples per out-going audio buffer
	  */
	void setNumberSamplesPerBuffer(const size_t& samples) {
		nSamplesPerBuffer = samples;
	}

	/** Push a single sample into the output buffer.
	  * Assumes that the input array contains AT LEAST as many samples as there are output channels.
	  */ 
	void pushSample(const float* input);

	/** Push N samples into the output buffer.
	  * Assumes that the input array contains AT LEAST as many samples as there are output channels multiplied by N.
	  */
	void pushSamples(const float* input, const size_t& N);

	/** Copy a single sample to all output channels.
	  */
	void copySample(const float& input);

	/** Copy N samples to all output channels
	  */
	void copySamples(const float* input, const size_t& N);

	/** Retrieve a single sample from the audio buffer
	  * If the buffer is empty, the most recent sample will be returned. If this is the case, false will be returned.
	  * @param output Array of samples to copy into (must have a length of at least 2)
	  * @return True if at least one sample was contained in the buffer and return false otherwise
	  */
	bool getSample(float* output);
	
	/** Retrieve N samples from the audio buffer
	  * If the number of samples is greater than the length of the buffer, attempt to interpolate between existing samples
	  * to create the illusion of there being more audio data. If this is the case, false will be returned.
	  * @parm output Array of samples to copy into (must have a length of at least 2 * N)
	  * @return True if the buffer contained at least N samples and return false otherwise
	  */
	bool getSamples(float* output, const size_t& N);

	/** Clear a queue of floats
	  */
	static void clearQueue(std::queue<float>& input) {
		while (!input.empty())
			input.pop();
	}

protected:
	size_t nSamplesPerBuffer; ///< Expected number of audio samples per out-going audio buffer

	size_t nOutputChannels; ///< Number of audio output channels

	size_t nSamples; ///< Number of samples currently in the output buffer

	std::vector<float> vEmpty; ///< 

	std::vector<std::queue<float> > samples; ///< Audio samples for all output channels

	std::mutex lock; ///< Mutex lock for buffer read/write access

	/** Pop the oldest sample off the buffer
	  * If the buffer will be made empty by the pop, do nothing.
	  */
	void popSample();

	/** Check the number of audio samples, and remove samples if the buffer grows too large
	  */
	void checkNumberOfSamples();
};

#endif
