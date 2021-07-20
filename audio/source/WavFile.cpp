#include <iostream>
#include <algorithm> // std::min
#include <vector>

#include "WavFile.hpp"

namespace WavFile {

	void WavHeader::print() const {
		float lengthOfAudio = nLengthData / (float)(nBytesPerSecond);
		std::cout << " File Size: " << nFileSize << " B" << std::endl;
		std::cout << " Sample Rate: " << nSampleRate << " Hz" << std::endl;
		std::cout << " Number Channels: " << nChannels << std::endl;
		std::cout << " Length of Audio: " << lengthOfAudio << " s" << std::endl;
		std::cout << " Bits per channel: " << nBitsPerChannel << std::endl;
		std::cout << " Bytes per sample: " << nBytesPerSample << std::endl;
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
		if (str != "WAVE")
			return false;
		f.read(&str[0], 4); // fmt 
		if (str != "fmt ")
			return false;
		f.read((char*)&nLengthFormatData, 4); // 16 is PCM
		f.read((char*)&nFormat, 2); // 1 is PCM
		f.read((char*)&nChannels, 2);
		f.read((char*)&nSampleRate, 4); // in Hz
		f.read((char*)&nBytesPerSecond, 4); // Data rate (B/s) [(SampleRate * BitsPerSample * Channels) / 8]
		f.read((char*)&nBytesPerSample, 2); // Bytes per sample [(BitsPerSample * Channels) / 8]
		f.read((char*)&nBitsPerChannel, 2); // Bits per channel
		f.read(&str[0], 4); // data
		while (str != "data" && !f.eof()) { // Skip any extraneous data in the header
			str[0] = str[1];
			str[1] = str[2];
			str[2] = str[3];
			f.read(&str[3], 1);
		}
		if (str != "data")
			return false; // ??
		f.read((char*)&nLengthData, 4);

		// Compute Wav file parameters
		nSamples = nLengthData / nBytesPerSample;
		fSamplePeriod = 1.f / nSampleRate;
		nLengthRemaining = nLengthData;
		compute();

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
		f.write((char*)&nBitsPerChannel, 2);
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

	WavFilePlayer::WavFilePlayer(const std::string& fname) :
		WavFilePlayer()
	{
		load(fname);
	}

	WavFilePlayer::~WavFilePlayer() {
		audio.close();
	}

	bool WavFilePlayer::load(const std::string& fname) {
		audio.open(fname.c_str(), std::ios::binary);
		if (audio.good()) {
			if (!readHeader(audio)) {
				audio.close();
				return false;
			}
			fSamples[0] = std::vector<float>(nChannels, 0.f);
			fSamples[1] = std::vector<float>(nChannels, 0.f);
			fInterpolated = std::vector<float>(nChannels, 0.f);
			if (nBitsPerChannel == 8) { // Unsigned 8-bit integer
				nBuffer8 = std::vector<uint8_t>(nLengthData, 0);
				audio.read((char*)nBuffer8.data(), nLengthData);
				nBufferSize = nLengthData;
			}
			else if (nBitsPerChannel == 16) { // Signed 16-bit integer
				nBuffer16 = std::vector<uint16_t>(nLengthData / 2, 0);
				audio.read((char*)nBuffer16.data(), nLengthData);
				nBufferSize = nLengthData / 2;
			}
			else if (nBitsPerChannel == 24) { // Signed 24-bit integer
				nBufferSize = 0;
				nBuffer24 = std::vector<uint32_t>(nLengthData / 3, 0);
				uint8_t readBuffer[3072]; // 1024 24-bit values
				unsigned int nBytesToRead = 3072;
				while (nLengthRemaining > 0) {
					nBytesToRead = std::min(nLengthRemaining, (unsigned int)3072);
					audio.read((char*)readBuffer, nBytesToRead);
					for (int i = 0; i < nBytesToRead / 3; i++) {
						nBuffer24[nBufferSize++] = readBuffer[i * 3] + (readBuffer[i * 3 + 1] << 8) + (readBuffer[i * 3 + 2] << 16);
					}
					nLengthRemaining -= nBytesToRead;
				}
				nBufferSize = nLengthData / 3;
			}
			else if (nBitsPerChannel == 32) { // 32-bit floating point
				fBuffer32 = std::vector<float_t>(nLengthData / 4, 0);
				audio.read((char*)fBuffer32.data(), nLengthData);
				nBufferSize = nLengthData / 4;
			}

			// Close the file
			bLoaded = (audio.good() && !audio.eof());
			bGood = bLoaded;
			audio.close();

			// Set sampling period and frequency
			fPeriod = fSamplePeriod;
			fFrequency = 1.f / fSamplePeriod;

			// Reset waveform parameters
			reset();
		}
		else {
			std::cout << " [WavFilePlayer] Error! Failed to read input wav file \"" << fname << "\"." << std::endl;
			audio.close();
			bGood = false;
			bLoaded = false;
		}	
		return bGood;
	}

	void WavFilePlayer::reset() {
		fPhase = 0.f;
		nBufferIndex = 0;
		if (bLoaded) {
			bGood = true;
			userPhaseRollover(); // Load the first sample
		}
	}

	float WavFilePlayer::userSample(const float& dt) {
		if (!bGood) {
			return 0.f;
		}
		for (unsigned short ch = 0; ch < nChannels; ch++) { // Over all channels
			fInterpolated[ch] = fSamples[0][ch] + fPhase * (fSamples[1][ch] - fSamples[0][ch]) / fSamplePeriod;
		}
		return (fCurrentValue = fInterpolated[0]);
	}

	void WavFilePlayer::userPhaseRollover() {
		if (!bGood || (nBufferIndex >= nBufferSize)) { // Not enough bytes
			bGood = false;
			return;
		}
		switch (nBitsPerChannel) {
		case 8:
			for (unsigned short ch = 0; ch < nChannels; ch++) { // Over all channels
				fSamples[0][ch] = fSamples[1][ch]; // Move all t1 samples to t0
				fSamples[1][ch] = nBuffer8[nBufferIndex++] / 255.f;
			}
			break;
		case 16:
			for (unsigned short ch = 0; ch < nChannels; ch++) { // Over all channels
				fSamples[0][ch] = fSamples[1][ch]; // Move all t1 samples to t0
				fSamples[1][ch] = ((nBuffer16[nBufferIndex] & 0x8000) == 0 ? nBuffer16[nBufferIndex] / 32767.f : -1.f + (nBuffer16[nBufferIndex] & 0x7fff) / 32767.f);
				nBufferIndex++;
			}
			break;
		case 24:
			for (unsigned short ch = 0; ch < nChannels; ch++) { // Over all channels
				fSamples[0][ch] = fSamples[1][ch]; // Move all t1 samples to t0
				fSamples[1][ch] = ((nBuffer24[nBufferIndex] & 0x800000) == 0 ? nBuffer24[nBufferIndex] / 8388607.f : -1.f + (nBuffer24[nBufferIndex] & 0x7fffff) / 8388607.f);
				nBufferIndex++;
			}
			break;
		case 32:
			for (unsigned short ch = 0; ch < nChannels; ch++) { // Over all channels
				fSamples[0][ch] = fSamples[1][ch]; // Move all t1 samples to t0
				fSamples[1][ch] = fBuffer32[nBufferIndex++];
			}
			break;
		default:
			break;
		}
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
		if (!bRecording)
			return false;
		if (nBitsPerChannel == 8) { // Unsigned 8-bit integer
			// -1 -> 0
			//  0 -> 127
			// +1 -> 255
			uint8_t sample;
			for (unsigned short ch = 0; ch < nChannels; ch++) {
				sample = (uint8_t)(0.5f * (ott::clamp(mixer[ch], -1.f) + 1.f) * 255.f);
				audio.write(reinterpret_cast<char*>(&sample), 1);
			}
		}
		else if (nBitsPerChannel == 16) { // Signed 16-bit integer
			// -1 -> -32768
			//  0 -> 0
			// +1 -> 32767
			int16_t sample;
			for (unsigned short ch = 0; ch < nChannels; ch++) {
				sample = (int16_t)(-32768.f + (0.5f * (ott::clamp(mixer[ch], -1.f) + 1.f) * 65535.f));
				audio.write(reinterpret_cast<char*>(&sample), 2);
			}
		}
		else if (nBitsPerChannel == 24) { // Signed 24-bit integer
			// -1 -> -8388608
			//  0 -> 0
			// +1 -> 8388607
			int32_t sample;
			for (unsigned short ch = 0; ch < nChannels; ch++) {
				sample = (int32_t)(-8388608.f + (0.5f * (ott::clamp(mixer[ch], -1.f) + 1.f) * 16777215.f));
				audio.write(reinterpret_cast<char*>(&sample), 3);
			}
		}
		else if (nBitsPerChannel == 32) { // 32-bit floating point
			float_t sample;
			for (unsigned short ch = 0; ch < nChannels; ch++) {
				sample = (float_t)ott::clamp(mixer[ch], -1.f);
				audio.write(reinterpret_cast<char*>(&sample), 4);
			}
		}
		else { // Unknown bit depth
			return false;
		}
		nSamples++;
		nFileSize += nBytesPerSample;
		nLengthData += nBytesPerSample;
		return true;
	}

} // namespace WavFile
