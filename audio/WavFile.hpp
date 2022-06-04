#ifndef WAV_FILE_HPP
#define WAV_FILE_HPP

#include <fstream>
#include <cinttypes>
#include <cfloat>
#include <queue>

#include "AudioSampler.hpp"
#include "SoundMixer.hpp"

namespace ott {

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
		void setNumberOfChannels(const uint16_t& channels) {
			nChannels = channels;
			compute(); // Update dependant variables
		}

		/** Set the audio sample rate in Hz (default is 44100 Hz)
		  */
		void setSampleRate(const uint32_t& samplesPerSecond) {
			nSampleRate = samplesPerSecond;
			compute(); // Update dependant variables
		}

		/** Set the number of sample bits per channel (default is 8)
		  */
		void setBitsPerChannel(const uint16_t bitsPerChannel) {
			nBitsPerChannel = bitsPerChannel;
			compute(); // Update dependant variables
		}

		/** Print wav file header information
		  */
		void print() const ;

	protected:
		uint32_t nFileSize; ///< Total file size (minus 8 bytes)

		uint32_t nSampleRate; ///< Audio sample rate (Hz)

		uint32_t nLengthFormatData; ///< Total length of "fmt" data (16 B for PCM)

		uint32_t nLengthData; ///< Total length of audio data (in bytes)

		uint32_t nLengthRemaining; ///< Remaining bytes of audio data

		uint32_t nSamples; ///< Total number of audio samples

		uint16_t nFormat; ///< Audio format (1 is PCM format)

		uint16_t nChannels; ///< Number of audio channels

		uint16_t nBitsPerChannel; ///< Number of bits per sample per channel

		uint16_t nBitsPerSample; ///< Number of bits per sample (nBitsPerChannel * nChannels)

		uint16_t nBytesPerSample; ///< Number of bytes per sample (nBitsPerChannel * nChannels / 8)

		uint32_t nBytesPerSecond; ///< Data rate in bytes per second (nSampleRate * nBitsPerChannel * nChannels / 8)

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
			bGood(false),
			bLoaded(false),
			audio(),
			fSamples(),
			fInterpolated(),
			nBuffer8(),
			nBuffer16(),
			nBuffer24(),
			fBuffer32(),
			nBufferIndex(0),
			nBufferSize(0)
		{
		}

		/** Wav file constructor
		  */
		WavFilePlayer(const std::string& fname);

		/** Destructor
		  */
		~WavFilePlayer();

		/** Open an input wav file and read the header
		  */
		bool load(const std::string& fname);

		/** Reset to beginning of wav file
		  */
		void reset() override;

	private:
		bool bGood; ///< Set if audio data still remains

		bool bLoaded; ///< Set if input file stream is loaded successfully

		std::ifstream audio; ///< Input audio file stream

		std::vector<float> fSamples[2]; ///< Current t0 and t1 audio samples for each channel, to be used for interpolation

		std::vector<float> fInterpolated; ///< Current interpolated audio values for each channel

		std::vector<uint8_t> nBuffer8; ///< Buffer containing 8-bit audio data

		std::vector<uint16_t> nBuffer16; ///< Buffer containing 16-bit audio data

		std::vector<uint32_t> nBuffer24; ///< Buffer containing 24-bit audio data

		std::vector<float_t> fBuffer32; ///< Buffer containing 32-bit floating point audio data

		size_t nBufferIndex; ///< Current audio buffer index

		size_t nBufferSize; ///< Total audio buffer size (in bytes)

		/** Sample output audio wave, after incrementing the phase a specified amount, and return the result
		  */
		float userSample(const float& dt) override;

		/** Called when waveform phase rolls over.
		  * Does nothing by default.
		  */
		void userPhaseRollover() override;
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

		/** Add a raw audio sample directly to the output file.
		  * Input array must contain AT LEAST nBytesPerSample.
		  */
		bool addSample(void* ptr);

		/** Add an audio sample from an audio mixer to the output file.
		  * Sound mixer must contain AT LEAST as many channels as the recorded wav file.
		  */
		bool addSample(const SoundMixer& mixer);

	private:
		bool bRecording; ///< Set if a wav file is open for recording

		std::string sFilename; ///< Output filename

		std::ofstream audio; ///< Output audio file stream
	};

} // namespace WavFile

#endif
