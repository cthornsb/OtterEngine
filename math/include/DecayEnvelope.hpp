#ifndef DECAY_ENVELOPE_HPP
#define DECAY_ENVELOPE_HPP

#include <iostream> // TEMP

namespace ott{
	class DecayEnvelope{
	public:
		enum class DecayType{
			EXPONENTIAL,
			PARABOLIC,
			LINEAR,
			DELTA
		};
	
		DecayEnvelope() :
			bActive(false),
			bHeld(false),
			fSustain(0.f),
			fValue(0.f),
			fMin(0.f),
			fMax(1.f),
			fAttackSlope(0.f),
			fDecaySlope(0.f),
			fReleaseSlope(0.f),
			fTimeAttack(1.f),
			fTimeDecay(1.f),
			fTimeSustain(-1.f),
			fTimeSustainStart(2.f),
			dTotalTime(0),
			eType(DecayType::EXPONENTIAL)
		{
		}

		DecayEnvelope(const float& value, const float& attackTime, const float& decayTime, const float& minValue = 0.f, const float& maxValue = 1.f) :
			bActive(false),
			bHeld(false),
			fSustain(value),
			fValue(value),
			fMin(minValue),
			fMax(maxValue),
			fAttackSlope(0.f),
			fDecaySlope(0.f),
			fReleaseSlope(0.f),
			fTimeAttack(attackTime),
			fTimeDecay(decayTime),
			fTimeSustain(-1.f),
			fTimeSustainStart(attackTime + decayTime),
			dTotalTime(0),
			eType(DecayType::EXPONENTIAL)
		{
		}

		/*float operator () const {
			return fValue;
		}*/

		/** Get the current envelope value
		  */
		float get() const {
			return fValue;
		}
		
		/** Return true if the envelope is currently actively decreasing 
		  */
		bool isActive() const {
			return bActive;
		}
		
		/** Set the "slope" of envelope release phase
		  */
		void setReleaseSlope(const float& slope){
			fReleaseSlope = slope;
		}
		
		/** Set type of envelope decay
		  */
		void setDecayType(const DecayType& type){
			eType = type;
		}
		
		/** Set the amount of time taken for envelope to rise from minimum output to sustain output value
		  */
		void setAttackTime(const double& dt){
			fTimeAttack = dt;
			fTimeSustainStart = fTimeAttack + fTimeDecay;
		}
		
		/** Set the amount of time taken for envelope to decay to sustain level from maximum
		  */
		void setDecayTime(const double& dt){
			fTimeDecay = dt;
			fTimeSustainStart = fTimeAttack + fTimeDecay;
		}

		/** Set the current driver signal level
		  */
		void trigger(const float& val);
		
		/** Release envelope sustain and start decaying to zero
		  */
		void release();
	
		/** Update the current envelope time step
		  */
		void update(const double& dTime);
		
		/** Kill envelope output
		  */
		void kill();
	
	protected:
		bool bActive; ///< Set if envelope is currently decreasing
		
		bool bHeld; ///< Set if sustain is being held, envelope will begin release mode after sustain is disabled
	
		float fSustain; ///< Current value of driver signal

		float fValue; ///< Current envelope value

		float fMin; ///< Minimum envelope value
		
		float fMax; ///< Maximum envelope value

		float fAttackSlope; ///< Envelope attack phase slope

		float fDecaySlope; ///< Envelope decay phase slope
		
		float fReleaseSlope; ///< Envelope release phase slope
		
		double fTimeAttack; ///< Length of time taken for envelope to rise to output level from zero
		
		double fTimeDecay; ///< Length of time taken for envelope to decay to sustain level from maximum
		
		double fTimeSustain; ///< Length of time that envelope output is held at sustain level
		
		double fTimeSustainStart; ///< Time at which envelope begins to decay to zero
		
		double dTotalTime; ///< Time since the last driver signal was applied to the envelope
		
		DecayType eType; ///< Envelope decay type
		
		/** 
		  */
		virtual void onUserUpdate() {
		}
	};
}

#endif // ifndef DECAY_ENVELOPE_HPP
