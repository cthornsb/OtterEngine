#ifndef SYNTHESIZERS_HPP
#define SYNTHESIZERS_HPP

#include "PianoKeys.hpp"
#include "SoundMixer.hpp" // ott::clamp

#include "OTTRandom.hpp"
#include "DecayEnvelope.hpp"

namespace Synthesizers{
	class SimpleSynth : public ott::DecayEnvelope {
	public:
		/** Default constructor
		  */
		SimpleSynth() : 
			ott::DecayEnvelope(),
			bUseVolumeEnvelope(false),
			fAmplitude(1.f),
			fFrequency(440.f),
			fPeriod(1.f / 440.f),
			fPhase(0.f),
			fPhaseStep(1.f / 44100.f)
		{
		}

		/** Destructor
		  */
		virtual ~SimpleSynth() { 
		}
		
		/** Set maximum waveform amplitude
		  */
		void setAmplitude(const float& A){ 
			fAmplitude = ott::clamp(A);
		}

		/** Set audio waveform frequency to that of a natural music note
		  */
		void setFrequency(const PianoKeys::Key& key, const int& octave=4){
			setFrequency(PianoKeys::getFrequency(key, PianoKeys::Modifier::NONE, octave));
		}
	
		/** Set audio waveform frequency to that of a natural, sharp, or flat music note
		  */
		void setFrequency(const PianoKeys::Key& key, const PianoKeys::Modifier& mod, const int& octave=4){
			setFrequency(PianoKeys::getFrequency(key, mod, octave));
		}
		
		/** Set audio waveform frequency (in Hz)
		  */
		void setFrequency(const float& freq){
			fFrequency = freq;
			fPeriod = 1.f / freq;
		}

		/** Set audio output sample rate (in Hz)
		  */
		void setSampleRate(const float& freq) {
			fPhaseStep = 1.f / freq;
		}

		/** Enable or disable output volume envelope.
		  * If enabled, volume envelope will be used to moderate waveform amplitude.
		  */
		void useVolumeEnvelope(bool state = true) {
			bUseVolumeEnvelope = state;
		}
		
		/** Get the current maximum waveform amplitude
		  */
		float getAmplitude() const { 
			return fAmplitude; 
		}
		
		/** Get the current volume envelope output volume
		  */
		float getVolume() const {
			return fValue;
		}

		/** Get the current audio waveform frequency (in Hz)
		  */
		float getFrequency() const { 
			return fFrequency; 
		}
		
		/** Get the current audio waveform period (in seconds)
		  */
		float getPeriod() const { 
			return (1.f / fFrequency); 
		}

		/** Get a pointer to the output volume envelope
		  */
		ott::DecayEnvelope* getVolumeEnvelope() {
			return reinterpret_cast<ott::DecayEnvelope*>(this);
		}

		/** Sample output audio wave, after incrementing the phase a specified amount, and return the result
		  */
		float sample(const float& dt);

		/** Sample the audio waveform N times, incrementing the phase by the sample rate phase step before each sample.
		  * Output array must contain AT LEAST N elements.
		  */
		void sample(const float& dt, float* arr, const unsigned int& N);

		/** Sample output audio wave, after incrementing the phase by the sample rate phase step, and return the result
		  */
		float sample() {
			return sample(fPhaseStep);
		}

		/** Sample the audio waveform N times, after incrementing the phase by the sample rate phase step before each sample.
		  * Output array must contain AT LEAST N elements.
		  */
		void sample(float* arr, const unsigned int& N) {
			sample(fPhaseStep, arr, N);
		}

	protected:
		bool bUseVolumeEnvelope; ///< Set if volume envelope will moderate amplitude of audio waveform

		float fAmplitude; ///< Maximum amplitude of audio waveform (clamped between 0 and 1)
		
		float fFrequency; ///< Frequency of audio waveform (in Hz)
		
		float fPeriod; ///< Period of audio waveform (in seconds)

		float fPhase; ///< Current phase of audio waveform (in seconds)

		float fPhaseStep; ///< Standard phase step of waveform based on sampling frequency (in seconds)
		
		/** Sample output audio wave, after incrementing the phase a specified amount, and return the result
		  */
		virtual float userSample(const float& dt) = 0;
	};
	
	class SineWave : public SimpleSynth {
	public:
		/** Default constructor
		  */
		SineWave() :
			SimpleSynth()
		{
		}

	protected:
		/** Sample the sine wave at a specified phase
		  */
		float userSample(const float& dt) override;
	};

	class TriangleWave : public SimpleSynth {
	public:
		/** Default constructor
		  */
		TriangleWave() :
			SimpleSynth()
		{
		}
		
	protected:
		/** Sample the wave at a specified phase
		  */
		float userSample(const float& dt) override;
	};	
	
	class SquareWave : public SimpleSynth {
	public:
		/** Default constructor
		  */
		SquareWave() :
			SimpleSynth(),
			nHarmonics(10)
		{
		}

	protected:
		int nHarmonics; ///< Maximum wave harmonic number
		
		/** Sample the wave at a specified phase
		  */
		float userSample(const float& dt) override;
	};

	class SawtoothWave : public SimpleSynth {
	public:
		/** Default constructor
		  */
		SawtoothWave() :
			SimpleSynth(),
			nHarmonics(10)
		{
		}

	protected:
		int nHarmonics; ///< Maximum wave harmonic number

		/** Sample the wave at a specified phase
		  */
		float userSample(const float& dt) override;
	};

	class Noise : public SimpleSynth {
	public:
		/** Default constructor
		  */
		Noise() :
			SimpleSynth(),
			bPulseState(false),
			nShiftRegister(0),
			random(OTTRandom::Generator::XORSHIFT)
		{
			nShiftRegister = random();
		}

	protected:
		bool bPulseState; ///< Current state of bit zero of the 32-bit shift register

		uint32_t nShiftRegister; ///< Current 32-bit shift register

		OTTRandom random; ///< Psuedo random number generator 

		/** Sample the shift register at a specified phase
		  */
		float userSample(const float& dt) override;
	};

	class SquarePulse : public SimpleSynth {
	public:
		/** Default constructor
		  */
		SquarePulse() :
			SimpleSynth(),
			bPulseState(false)
		{
		}

	protected:
		bool bPulseState; ///< Current state of audio pulse (high / low)

		/** Sample the wave at a specified phase
		  */
		float userSample(const float& dt) override;
	};

	class TrapezoidPulse : public SimpleSynth {
	public:
		/** Default constructor
		  */
		TrapezoidPulse() :
			SimpleSynth(),
			pulse(0.f, 0.0005, 0.0)
		{
			pulse.setDecayType(DecayType::LINEAR);
			pulse.setReleaseTime(0.0005);
			pulse.setSustainTime(0.0005);
		}

		/** Set the full width at half maximum (FWHM) of the trapezoidal pulse (in seconds)
		  */
		void setPulseWidth(const double& dt) {
			pulse.setSustainTime(dt - (pulse.getAttackTime() + pulse.getReleaseTime()) / 2);
		}

	protected:
		ott::DecayEnvelope pulse; ///< Audio pulse envelope

		/** Sample the wave at a specified phase
		  */
		float userSample(const float& dt) override;
	};
} // namespace Synthesizers

#endif // ifndef SYNTHESIZERS_HPP
