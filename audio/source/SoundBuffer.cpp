#include <cmath> // fmod

#include "SoundBuffer.hpp"

void SoundBuffer::setNumberOfChannels(const size_t& N) {
	if (N <= samples.size())
		return;
	if (nSamples) { // Clear any existing audio data
		for (auto buff = samples.begin(); buff != samples.end(); buff++) {
			clearQueue(*buff);
		}
	}
	for (size_t i = 0; i < (N - samples.size()); i++) {
		samples.push_back(std::queue<float>());
		vEmpty.push_back(0.f);
	}
	nSamples = 0;
	nOutputChannels = N;
}

void SoundBuffer::pushSample(const float* input){
	lock.lock(); // Writing to buffer
	for (auto buff = samples.begin(); buff != samples.end(); buff++) {
		buff->push(*input);
		input++;
	}
	nSamples++;
	checkNumberOfSamples();
	lock.unlock();
}

void SoundBuffer::pushSamples(const float* input, const size_t& N) {
	lock.lock(); // Writing to buffer
	for (auto buff = samples.begin(); buff != samples.end(); buff++) {
		for (size_t i = 0; i < N; i++) {
			buff->push(*input);
			input++;
		}
	}
	nSamples += N;
	checkNumberOfSamples();
	lock.unlock();
}

void SoundBuffer::copySample(const float& input) {
	lock.lock(); // Writing to buffer
	for (auto buff = samples.begin(); buff != samples.end(); buff++) {
		buff->push(input);
	}
	nSamples++;
	checkNumberOfSamples();
	lock.unlock();
}

void SoundBuffer::copySamples(const float* input, const size_t& N) {
	lock.lock(); // Writing to buffer
	for (auto buff = samples.begin(); buff != samples.end(); buff++) {
		for (size_t i = 0; i < N; i++) {
			buff->push(input[i]);
		}
	}
	nSamples += N;
	checkNumberOfSamples();
	lock.unlock();
}

bool SoundBuffer::getSample(float* output){
	if (!nSamples) { // Buffer is empty, fill with zeroes
		for (size_t i = 0; i < nOutputChannels; i++) {
			*output = 0.f;
			output++;
		}
		return false;
	}
	lock.lock(); // Reading from buffer
	for (auto buff = samples.begin(); buff != samples.end(); buff++) {
		*output = buff->front();
		output++;
		buff->pop();
	}
	nSamples--;
	lock.unlock();
	return true;
}

bool SoundBuffer::getSamples(float* output, const size_t& N){
	if (nSamples <= 1) { // Buffer is empty, fill with zeroes
		for (size_t i = 0; i < nOutputChannels * N; i++) {
			*output = 0.f;
			output++;
		}
		return false;
	}
	lock.lock(); // Reading from buffer
	bool retval = false;
	if (nSamples >= N) { // Buffer contains at least N samples
		for(size_t i = 0; i < N; i++){
			for (auto buff = samples.begin(); buff != samples.end(); buff++) {
				*output = buff->front();
				output++;
				buff->pop();
			}
			nSamples--;
		}
		retval = true;
	}
	else { // Not enough samples in the buffer, in-between values will be interpolated
		float fPeriod = (float)N / (nSamples - 1); // Number of "in-between" samples per actual sample
		float fCounter = 0.f;
		std::vector<float> fX0(nOutputChannels, 0.f);
		std::vector<float> fX1(nOutputChannels, 0.f);
		std::vector<float> fSlope(nOutputChannels, 0.f);
		for (size_t j = 0; j < nOutputChannels; j++) {
			fX0[j] = samples[j].front();
			samples[j].pop();
			fX1[j] = samples[j].front();
			samples[j].pop();
			fSlope[j] = (fX1[j] - fX0[j]) / fPeriod;
		}
		nSamples -= 2;
		for(size_t i = 0; i < N - 1; i++){ // Over N-1 samples
			for (size_t j = 0; j < nOutputChannels; j++) { // Over all output channels
				*output = fX0[j] + fCounter * fSlope[j];
				output++;
			}
			fCounter += 1.f;
			if (fCounter >= fPeriod) { // Update interpolation values
				for (size_t j = 0; j < nOutputChannels; j++) { // Over all output channels
					fX0[j] = fX1[j];
					fX1[j] = samples[j].front();
					samples[j].pop();
					// Re-compute slopes
					fSlope[j] = (fX1[j] - fX0[j]) / fPeriod;
					fCounter = std::fmod(fCounter, fPeriod);
				}
				nSamples--;
			}
		}
		// Final sample
		for (size_t j = 0; j < nOutputChannels; j++) { // Over all output channels
			*output = fX1[j];
			output++;
		}
	}
	lock.unlock();
	return retval;
}

void SoundBuffer::popSample(){
	if(!nSamples){ // Trying to pop with size=0
		return;
	}
	else if(nSamples == 1){ // Sample buffer will be empty after pop, backup the remaining sample
		//fEmpty = front();
	}
	for (auto buff = samples.begin(); buff != samples.end(); buff++)
		buff->pop();
	nSamples--;
}

void SoundBuffer::checkNumberOfSamples() {
	while (nSamples >= nSamplesPerBuffer * 2) {
		for (auto buff = samples.begin(); buff != samples.end(); buff++)
			buff->pop();
		nSamples--;
	}
}