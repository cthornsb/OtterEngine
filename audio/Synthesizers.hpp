#ifndef SYNTHESIZERS_HPP
#define SYNTHESIZERS_HPP

#include "AudioSampler.hpp"
#include "OTTRandom.hpp"
#include "DecayEnvelope.hpp"

namespace Synthesizers{	
	class SineWave : public AudioSampler {
	public:
		/** Default constructor
		  */
		SineWave() :
			AudioSampler()
		{
		}

	protected:
		/** Sample the sine wave at a specified phase
		  */
		float userSample(const float& dt) override;
	};

	class TriangleWave : public AudioSampler {
	public:
		/** Default constructor
		  */
		TriangleWave() :
			AudioSampler()
		{
		}
		
	protected:
		/** Sample the wave at a specified phase
		  */
		float userSample(const float& dt) override;
	};	
	
	class SquareWave : public AudioSampler {
	public:
		/** Default constructor
		  */
		SquareWave() :
			AudioSampler(),
			nHarmonics(10)
		{
		}

	protected:
		int32_t nHarmonics; ///< Maximum wave harmonic number
		
		/** Sample the wave at a specified phase
		  */
		float userSample(const float& dt) override;
	};

	class SawtoothWave : public AudioSampler {
	public:
		/** Default constructor
		  */
		SawtoothWave() :
			AudioSampler(),
			nHarmonics(10)
		{
		}

	protected:
		int32_t nHarmonics; ///< Maximum wave harmonic number

		/** Sample the wave at a specified phase
		  */
		float userSample(const float& dt) override;
	};

	class Noise : public AudioSampler {
	public:
		/** Default constructor
		  */
		Noise() :
			AudioSampler(),
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

		/** Right shift all bits of shift register when waveform phase rolls over.
		  * If shift register is now zero, set it to a random 32-bit integer.
		  * Output state is set to state of bit 0 of shift register.
		  */
		void userPhaseRollover() override;
	};

	class SquarePulse : public AudioSampler {
	public:
		/** Default constructor
		  */
		SquarePulse() :
			AudioSampler(),
			bPulseState(false)
		{
		}

	protected:
		bool bPulseState; ///< Current state of audio pulse (high / low)

		/** Sample the wave at a specified phase
		  */
		float userSample(const float& dt) override;

		/** Toggle the state of wave pulse when waveform phase rolls over
		  */
		void userPhaseRollover() override;
	};

	class TrapezoidPulse : public AudioSampler {
	public:
		/** Default constructor
		  */
		TrapezoidPulse() :
			AudioSampler(),
			pulse(0.f, 0.0005, 0.0)
		{
			pulse.setDecayType(DecayType::LINEAR);
			pulse.setReleaseTime(0.0005f);
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

		/** Trigger the wave pulse when waveform phase rolls over
		  */
		void userPhaseRollover() override;
	};
} // namespace Synthesizers

#endif // ifndef SYNTHESIZERS_HPP
