#ifndef SOUND_MANAGER_HPP
#define SOUND_MANAGER_HPP

#include <memory>

#ifdef ENABLE_PORT_AUDIO

#include "portaudio.h"

typedef int (*portCallback)( const void*, void*, unsigned long, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* );

#endif // ifdef ENABLE_PORT_AUDIO

class SoundMixer;

class SoundManager{
public:
	/** Terminate audio stream
	  */
	~SoundManager();

	/** Copy constructor (deleted)
	  */
	SoundManager(const SoundManager&) = delete;
	
	/** Copy operator (deleted)
	  */
	SoundManager& operator = (const SoundManager&) = delete;

	/** Get instance of singleton
	  */
	static SoundManager& getInstance(){
		static SoundManager instance;
		return instance;			
	}

	/** Get the number of audio channels
	  */
	int getNumberOfChannels() const { 
		return nChannels; 
	}
	
	/** Get the audio sample rate (in Hz)
	  */
	double getSampleRate() const { 
		return dSampleRate; 
	}
	
	/** Get the number of audio samples per buffer
	  */
	unsigned long getFramesPerBuffer() const { 
		return nFramesPerBuffer; 
	}

	/** Return true if the audio interface is running, and return false otherwise
	  */
	bool isRunning() const { 
		return bRunning; 
	}

	/** Set the number of audio channels (default = 2)
	  * Has no effect if called after audio stream is initialized.
	  */
	void setNumberOfChannels(const int& channels){ 
		nChannels = channels; 
	}
	
	/** Set the audio sample rate in Hz (default = 44100 Hz)
	  * Has no effect if called after audio stream is initialized
	  */
	void setSampleRate(const double& rate){ 
		dSampleRate = rate;
	}

	/** Set the number of samples per audio buffer (default = 2048)
	  * Has no effect if called after audio stream is initialized
	  */	
	void setFramesPerBuffer(const unsigned long& frames){ 
		nFramesPerBuffer = frames;
	}

#ifdef ENABLE_PORT_AUDIO
	/** Set the audio callback function
	  * Has no effect if called after audio stream is initialized
	  */
	void setCallbackFunction(portCallback call){ 
		callback = call;
	}
#endif // ifdef ENABLE_PORT_AUDIO

	/** Initialize audio stream
	  * @param mixer Pointer to output audio mixer
	  */
	bool init(void* data);
	
	/** Terminate audio stream
	  */
	bool terminate();

	/** Start audio stream
	  */
	bool start();
	
	/** Pause audio stream
	  */
	void sleep(const long& length);
	
	/** Stop audio stream
	  */
	bool stop();
	
	/** 
	  */
	void quit(){
		bQuitting = true;
	}
	
	/** Loop until quit() is called
	  * Stream will be terminated before returning.
	  */
	void execute();

#ifdef ENABLE_PORT_AUDIO
	/** Default port callback function
	  */
	static int defaultCallback( 
		const void *input, 
		void *output, 
		unsigned long framesPerBuffer, 
		const PaStreamCallbackTimeInfo* timeInfo, 
		PaStreamCallbackFlags statusFlags,
		void *data 
	);
#endif // ifdef ENABLE_PORT_AUDIO

private:
	bool bQuitting;

	bool bInitialized;
	
	bool bRunning;
	
	int nChannels;
	
	double dSampleRate; ///< Audio sample rate
	
	unsigned long nFramesPerBuffer;

#ifdef ENABLE_PORT_AUDIO
	PaStream* stream; ///< Port audio stream pointer

	portCallback callback; ///< Port audio callback function pointer
#endif // ifdef ENABLE_PORT_AUDIO
	
	/** Default constructor
	  */
	SoundManager();
};

#endif

