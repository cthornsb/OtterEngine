#include "AudioSampler.hpp"
#include "SoundMixer.hpp" // ott::clamp

void AudioSampler::setAmplitude(const float& A) {
	fAmplitude = ott::clamp(A);
}

float AudioSampler::sample(const float& dt) {
	if (!bPlaying) {
		return 0.f;
	}
	if (!bUseVolumeEnvelope) { // Volume envelope not in use
		incrementPhase(dt);
		return (fCurrentValue = fAmplitude * ott::clamp(userSample(dt), -1.f));
	}
	incrementPhase(dt);
	this->update(dt);
	return (fCurrentValue = fValue * fAmplitude * ott::clamp(userSample(dt), -1.f));
}

void AudioSampler::sample(const float& dt, float* arr, const unsigned int& N) {
	if (!bPlaying) {
		for (unsigned int i = 0; i < N; i++)
			arr[i] = 0.f;
	}
	if (!bUseVolumeEnvelope) { // Volume envelope not in use
		for (unsigned int i = 0; i < N; i++) {
			incrementPhase(dt);
			arr[i] = (fAmplitude * ott::clamp(userSample(dt), -1.f));
		}
	}
	else {
		for (unsigned int i = 0; i < N; i++) {
			incrementPhase(dt);
			this->update(dt);
			arr[i] = (fValue * fAmplitude * ott::clamp(userSample(dt), -1.f));
		}
	}
}

void AudioSampler::incrementPhase(const float& dt) {
	if ((fPhase += dt) >= fPeriod) {
		fPhase = std::fmod(fPhase, fPeriod);
		this->userPhaseRollover();
	}
}
