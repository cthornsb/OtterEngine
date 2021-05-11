#include <algorithm>

#include "SoundMixer.hpp"

void SoundMixer::get(const unsigned int& ch, float& sample) {
	if(ch < nOutputChannels)
		sample = fOutputSamples[ch];
}

void SoundMixer::setBalance(const float& bal){
	if(nOutputChannels != 2)
		return;
	if(bal <= 0.f){ // Left balance
		fOutputVolume[0] = 1.f;
		fOutputVolume[1] = (bal + 1.f);
	}
	else{ // Right balance
		fOutputVolume[0] = (1.f - bal);
		fOutputVolume[1] = 1.f;
	}
}

void SoundMixer::reset(){
	nCyclesSinceLastClock = 0;
	reload(); // Refill counter so timer starts immediately
}

bool SoundMixer::update(){
	if(bMuted){
		for(unsigned int i = 0; i < nOutputChannels; i++)
			fOutputSamples[i] = 0.f;
		return false;
	}
	for(unsigned int i = 0; i < nOutputChannels; i++){ // Over left and right output channels
		fOutputSamples[i] = 0.f;
		for(unsigned int j = 0; j < nInputChannels; j++){ // Over input channels
			fOutputSamples[i] += fInputVolume[j] * (bSendInputToOutput[i][j] ? fInputSamples[j] : 0.f);
		}
		// Normalize audio output and apply master and channel volumes
		fOutputSamples[i] = ((1.f + fOffsetDC) * fMasterVolume * fOutputVolume[i] * (fOutputSamples[i] / nInputChannels)) - fOffsetDC; // Translate to range [-DC, 1]
	}
	if(bMonoOutput){ // Re-mix for mono output
		float fAverage = 0.f;
		for(unsigned int i = 0; i < nOutputChannels; i++)
			fAverage += fOutputSamples[i];
		fAverage /= nOutputChannels; // 0 to 1
		for(unsigned int i = 0; i < nOutputChannels; i++)
			fOutputSamples[i] = fAverage;
	}
	bModified = false;
	return true;
}

float SoundMixer::clamp(const float& input, const float& low/* = 0.f*/, const float& high/* = 1.f*/) const {
	return std::max(low, std::min(high, input));
}

void SoundMixer::rollover(){
	reload(); // Refill timer period
	if(bModified) // Update output samples if one or more input samples were modified
		update();
	pushSample(fOutputSamples[0], fOutputSamples[1]); // Push current sample onto the fifo buffer (mutex protected)
}

