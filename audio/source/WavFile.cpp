#include <iostream>
#include <vector>

#include "WavFile.hpp"

namespace WavFile {

	void WavHeader::print() const {
		float lengthOfAudio = nLengthData / (float)(nBytesPerSecond);
		std::cout << " File Size: " << nFileSize << " B" << std::endl;
		std::cout << " Sample Rate: " << nSampleRate << " Hz" << std::endl;
		std::cout << " Number Channels: " << nChannels << std::endl;
		std::cout << " Length of Audio: " << lengthOfAudio << " s" << std::endl;
		std::cout << " Audio Bitrate: " << nBitsPerSample * nSamples / (1000.f * lengthOfAudio) << " kbps" << std::endl;
	}

	bool WavHeader::readHeader(std::ifstream& f) {
		// Read 44 bytes from file header
		std::string str = "    ";
		f.read(&str[0], 4); // RIFF
		if (str != "RIFF")
			return false;
		f.read((char*)&nFileSize, 4);
		f.read(&str[0], 4); // WAVE
		f.read(&str[0], 4); // fmt 
		f.read((char*)&nLengthFormatData, 4); // 16 is PCM
		f.read((char*)&nFormat, 2); // 1 is PCM
		f.read((char*)&nChannels, 2);
		f.read((char*)&nSampleRate, 4); // in Hz
		f.read((char*)&nBytesPerSecond, 4); // Data rate (B/s) [(SampleRate * BitsPerSample * Channels) / 8]
		f.read((char*)&nBytesPerSample, 2); // Bytes per sample [(BitsPerSample * Channels) / 8]
		f.read((char*)&nBitsPerSample, 2);
		f.read(&str[0], 4); // data
		if (str != "data")
			return false; // ??
		f.read((char*)&nLengthData, 4);

		// Compute Wav file parameters
		nSamples = nLengthData / nBytesPerSample;
		nBitsPerChannel = nBitsPerSample / nChannels;
		fSamplePeriod = 1.f / nSampleRate;
		nLengthRemaining = nLengthData;

		return (f.good() && !f.eof());
	}

	bool WavHeader::writeHeader(std::ofstream& f) {
		// Write 44 bytes to file header
		std::string str;
		str = "RIFF"; f.write(&str[0], 4); // RIFF
		f.write((char*)&nFileSize, 4);
		str = "WAVE"; f.write(&str[0], 4); // WAVE
		str = "fmt "; f.write(&str[0], 4); // fmt 
		f.write((char*)&nLengthFormatData, 4); // 16 B for PCM
		f.write((char*)&nFormat, 2); // 1 is PCM
		f.write((char*)&nChannels, 2);
		f.write((char*)&nSampleRate, 4); // in Hz
		f.write((char*)&nBytesPerSecond, 4); // Data rate (B/s) [(SampleRate * BitsPerSample * Channels) / 8]
		f.write((char*)&nBytesPerSample, 2); // Bytes per sample [(BitsPerSample * Channels) / 8]
		f.write((char*)&nBitsPerSample, 2);
		str = "data"; f.write(&str[0], 4); // data
		f.write((char*)&nLengthData, 4);
		return (f.good() && !f.eof());
	}

	void WavHeader::compute() {
		nBitsPerSample = nBitsPerChannel * nChannels;
		nBytesPerSample = nBitsPerChannel * nChannels / 8;
		nBytesPerSecond = nSampleRate * nBitsPerChannel * nChannels / 8;
		fSamplePeriod = 1.f / nSampleRate;
	}

	WavFilePlayer::WavFilePlayer(const std::string& filename) :
		WavFilePlayer()
	{
		audio.open(filename.c_str());
		if (audio.good()) {
			readHeader(audio);
		}
	}

	WavFilePlayer::~WavFilePlayer() {
		audio.close();
	}

	void WavFilePlayer::sample(const float& timeStep, float* arr, const unsigned int& N) {
		if (!nLengthRemaining) { // Audio file completed
			for (unsigned long i = 0; i < N; i++) { // Over all frames
				arr[i] += 0.f;
			}
			return;
		}
		unsigned int samplesRequested = (int)(timeStep * N / fSamplePeriod);
		unsigned int bytesRequested = std::min(nLengthRemaining, samplesRequested * nBytesPerSample); // Total number of bytes requested
		std::vector<unsigned char> tempData(bytesRequested, 0);
		audio.read((char*)tempData.data(), bytesRequested * sizeof(unsigned char));
		unsigned int prevSample = 0;
		unsigned int currSample = 0;
		fPhase = 0.f;
		for (unsigned long i = 0; i < N; i++) { // Over all frames
			currSample = (unsigned int)(fPhase / fSamplePeriod);
			arr[i] += (2.f * tempData[currSample] / 255.f - 1.f);
			if (currSample != prevSample)
				prevSample = currSample;
			fPhase += timeStep;
		}
		nLengthRemaining -= bytesRequested;
	}

	WavFileRecorder::~WavFileRecorder() {
		if (bRecording)
			stopRecording();
	}

	bool WavFileRecorder::startRecording(const std::string fname) {
		if (bRecording)
			return false;

		// Open the output file
		sFilename = fname;
		audio.open(sFilename.c_str(), std::ios::binary);
		if (!audio.good())
			return false;

		// Write a dummy header with 44 zeros
		std::vector<char> dummy(44, 0);
		audio.write(dummy.data(), 44);

		// Reset wav file length parameters
		nFileSize = 36; // Total file size is currently 44 bytes (minus 8)
		nSamples = 0;
		nLengthData = 0;
		bRecording = true;

		return true;
	}

	bool WavFileRecorder::stopRecording() {
		if (!bRecording)
			return false;

		// Print wav header information
		print();

		// Seek to the beginning of the file
		audio.seekp(0);

		// Overwrite the dummy header
		writeHeader(audio);

		// Close the output file
		audio.close();

		return true;
	}

	bool WavFileRecorder::addSample(void* ptr) {
		if (!bRecording)
			return false;
		audio.write(reinterpret_cast<char*>(ptr), nBytesPerSample);
		nSamples++;
		nFileSize += nBytesPerSample;
		nLengthData += nBytesPerSample;
		return true;
	}

	bool WavFileRecorder::addSample(const SoundMixer& mixer) {
		// Not implemented
		return false;
	}

} // namespace WavFile
