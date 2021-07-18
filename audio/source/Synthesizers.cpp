#include <cmath>

#include "Synthesizers.hpp"

constexpr float PI = 3.14159f;

float Synthesizers::SimpleSynth::sample(const float& dt) {
	if (!bUseVolumeEnvelope) { // Volume envelope not in use
		fPhase += dt;
		return (fAmplitude * ott::clamp(userSample(dt)));
	}
	fPhase += dt;
	this->update(dt);
	return (fValue * fAmplitude * ott::clamp(userSample(dt)));
}

void Synthesizers::SimpleSynth::sample(const float& dt, float* arr, const unsigned int& N) {
	if (!bUseVolumeEnvelope) { // Volume envelope not in use
		for (unsigned int i = 0; i < N; i++) {
			fPhase += dt;
			arr[i] = (fAmplitude * ott::clamp(userSample(dt)));
		}
	}
	else {
		for (unsigned int i = 0; i < N; i++) {
			fPhase += dt;
			this->update(dt);
			arr[i] = (fValue * fAmplitude * ott::clamp(userSample(dt)));
		}
	}
}

float Synthesizers::SineWave::userSample(const float& dt){
	return std::sinf(fPhase * fFrequency * 2.f * PI);
}

float Synthesizers::TriangleWave::userSample(const float& dt){
	return ((4.f * fFrequency) * std::fabs(std::fmod(fPhase - fPeriod/4, fPeriod) - fPeriod/2) - 1.f);
}

float Synthesizers::SquareWave::userSample(const float& dt){
	const float coeff = 2 * PI * fFrequency * fPhase;
	float sum = 0.f;
	for(int i = 1; i <= nHarmonics; i++){
		sum += std::sinf(coeff * (2.f * i - 1)) / (2.f * i - 1);
	}
	return (4.f * sum / PI);
}

float Synthesizers::SawtoothWave::userSample(const float& dt){
	const float coeff = 2 * PI * fFrequency * fPhase;
	float sum = 0.f;
	for(int i = 1; i <= nHarmonics; i++){
		sum += (i % 2 == 0 ? 1.f : -1.f) * std::sinf(coeff * i) / i;
	}
	return (-2.f * sum / PI);
}

float Synthesizers::Noise::userSample(const float& dt) {
	if (fPhase >= fPeriod) {
		nShiftRegister = nShiftRegister >> 1;
		if (nShiftRegister == 0)
			nShiftRegister = random();
		bPulseState = ((nShiftRegister & 0x1) == 0x1);
		fPhase = std::fmod(fPhase, fPeriod);
	}
	return (bPulseState ? -1.f : 1.f);
}

float Synthesizers::SquarePulse::userSample(const float& dt) {
	if (fPhase >= fPeriod) {
		bPulseState = !bPulseState;
		fPhase = std::fmod(fPhase, fPeriod);
	}
	return (bPulseState ? 1.f : 0.f);
}

float Synthesizers::TrapezoidPulse::userSample(const float& dt) {
	if (fPhase >= fPeriod) {
		pulse.trigger(1.f);
		fPhase = std::fmod(fPhase, fPeriod);
	}
	if (pulse.isActive()) {
		pulse.update(dt);
		return pulse();
	}
	return 0.f;
}
