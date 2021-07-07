#ifndef WAV_FILE_HPP
#define WAV_FILE_HPP

#include <fstream>

#include "AudioSampler.hpp"
#include "SoundMixer.hpp"

namespace WavFile {

	class WavHeader {
	public:
		WavHeader() :
			nFileSize(0),
			nSampleRate(44100),
			nLengthFormatData(16), // 16 is PCM
			nLengthData(0),
			nLengthRemaining(0),
			nSamples(0),
			nFormat(1), // 1 is PCM
			nChannels(2),
			nBitsPerChannel(8),
			nBitsPerSample(nBitsPerChannel * nChannels),
			nBytesPerSample(nBitsPerChannel * nChannels / 8),
			nBytesPerSecond(nSampleRate * nBitsPerChannel * nChannels / 8),
			fSamplePeriod(1.f / nSampleRate)
		{
		}

		/** Set the number of input / output audio channels (default is 2)
		  */
		void setNumberOfChannels(const unsigned short& channels) {
			nChannels = channels;
			compute(); // Update dependant variables
		}

		/** Set the audio sample rate in Hz (default is 44100 Hz)
		  */
		void setSampleRate(const unsigned int& samplesPerSecond) {
			nSampleRate = samplesPerSecond;
			compute(); // Update dependant variables
		}

		/** Set the number of sample bits per channel (default is 8)
		  */
		void setBitsPerChannel(const unsigned short bitsPerChannel) {
			nBitsPerChannel = bitsPerChannel;
			compute(); // Update dependant variables
		}

		/** Print wav file header information
		  */
		void print() const ;

	protected:
		unsigned int nFileSize; ///< Total file size (minus 8 bytes)

		unsigned int nSampleRate; ///< Audio sample rate (Hz)

		unsigned int nLengthFormatData; ///< Total length of "fmt" data (16 B for PCM)

		unsigned int nLengthData; ///< Total length of audio data (in bytes)

		unsigned int nLengthRemaining; ///< Remaining bytes of audio data

		unsigned int nSamples; ///< Total number of audio samples

		unsigned short nFormat; ///< Audio format (1 is PCM format)

		unsigned short nChannels; ///< Number of audio channels

		unsigned short nBitsPerChannel; ///< Number of bits per sample per channel

		unsigned short nBitsPerSample; ///< Number of bits per sample (nBitsPerChannel * nChannels)

		unsigned short nBytesPerSample; ///< Number of bytes per sample (nBitsPerChannel * nChannels / 8)

		unsigned int nBytesPerSecond; ///< Data rate in bytes per second (nSampleRate * nBitsPerChannel * nChannels / 8)

		float fSamplePeriod; ///< Period of audio sampling in seconds (1 / nSampleRate)

		/** Read wav file header from input file stream
		  */
		bool readHeader(std::ifstream& f);

		/** Write wav file header to output file stream
		  */
		bool writeHeader(std::ofstream& f);

		/** Update wav file parameters with user specified values
		  */
		void compute();
	};

	class WavFilePlayer : public WavHeader, AudioSampler {
	public:
		/** Default file constructor
		  */
		WavFilePlayer() :
			WavHeader(),
			AudioSampler(),
			audio()
		{
		}

		/** Wav file constructor
		  */
		WavFilePlayer(const std::string& filename);

		/** Destructor
		  */
		~WavFilePlayer();

		/** Does nothing. Returns zero
		  */
		float sample(const float&) override { 
			return 0.f; 
		}

		/** Read a specified number of samples from the input wav file and write them to an output array
		  */
		void sample(const float& timeStep, float* arr, const unsigned int& N) override ;

	private:
		std::ifstream audio; ///< Input audio file stream
	};

	class WavFileRecorder : public WavHeader {
	public:
		/** Default file constructor
		  */
		WavFileRecorder() :
			WavHeader(),
			bRecording(false),
			sFilename(),
			audio()
		{
		}

		/** Destructor
		  */
		~WavFileRecorder();

		/** Open new wav file for recording
		  * @param filename Path to output wav file
		  * @return True if output file is opened successfully
		  */
		bool startRecording(const std::string fname);

		/** Finalize wav file and close it
		  * @param filename Path to output wav file
		  * @return True if file was closed successfully
		  */
		bool stopRecording();

		/** Add an audio sample directly to the output file 
		  */
		bool addSample(void* ptr);

		/** Add an audio sample from an audio mixer directly to the output file.
		  * Not implemented.
		  */
		bool addSample(const SoundMixer& mixer);

	private:
		bool bRecording; ///< Set if a wav file is open for recording

		std::string sFilename; ///< Output filename

		std::ofstream audio; ///< Output audio file stream
	};

} // namespace WavFile

#endif
