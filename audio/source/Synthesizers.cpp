#include <cmath>

#include "Synthesizers.hpp"

constexpr float PI = 3.14159f;

float Synthesizers::SineWave::userSample(const float& dt){
	return std::sinf(fPhase * fFrequency * 2.f * PI);
}

float Synthesizers::TriangleWave::userSample(const float& dt){
	return ((4.f * fFrequency) * std::fabs(std::fmod(fPhase - fPeriod/4, fPeriod) - fPeriod/2) - 1.f);
}

float Synthesizers::SquareWave::userSample(const float& dt){
	const float coeff = 2 * PI * fFrequency * fPhase;
	float sum = 0.f;
	for(int32_t i = 1; i <= nHarmonics; i++){
		sum += std::sinf(coeff * (2.f * i - 1)) / (2.f * i - 1);
	}
	return (4.f * sum / PI);
}

float Synthesizers::SawtoothWave::userSample(const float& dt){
	const float coeff = 2 * PI * fFrequency * fPhase;
	float sum = 0.f;
	for(int32_t i = 1; i <= nHarmonics; i++){
		sum += (i % 2 == 0 ? 1.f : -1.f) * std::sinf(coeff * i) / i;
	}
	return (-2.f * sum / PI);
}

float Synthesizers::Noise::userSample(const float& dt) {
	return (bPulseState ? -1.f : 1.f);
}

void Synthesizers::Noise::userPhaseRollover() {
	nShiftRegister = nShiftRegister >> 1;
	if (nShiftRegister == 0)
		nShiftRegister = random();
	bPulseState = ((nShiftRegister & 0x1) == 0x1);
}

float Synthesizers::SquarePulse::userSample(const float& dt) {
	return (bPulseState ? 1.f : 0.f);
}

void Synthesizers::SquarePulse::userPhaseRollover() {
	bPulseState = !bPulseState;
}

float Synthesizers::TrapezoidPulse::userSample(const float& dt) {
	if (pulse.isActive()) {
		pulse.update(dt);
		return pulse();
	}
	return 0.f;
}

void Synthesizers::TrapezoidPulse::userPhaseRollover() {
	pulse.trigger(1.f);
}
