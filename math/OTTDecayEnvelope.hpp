#ifndef MATH_OTT_DECAY_ENVELOPE_HPP
#define MATH_OTT_DECAY_ENVELOPE_HPP

namespace ott{

class DecayEnvelope{
public:
	enum class DecayType{
		Exponential,
		Parabolic,
		Linear,
		Delta
	};
	
	/** Default constructor
	  */
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
		dTimeAttack(1.f),
		dTimeDecay(1.f),
		dTimeSustain(-1.f),
		dTimeSustainStart(2.f),
		dTotalTime(0),
		eType(DecayType::Exponential)
	{
	}

	/** Attack and decay time constructor
	  */
	DecayEnvelope(const float& value, const double& attackTime, const double& decayTime, const float& minValue = 0.f, const float& maxValue = 1.f) :
		bActive(false),
		bHeld(false),
		fSustain(value),
		fValue(value),
		fMin(minValue),
		fMax(maxValue),
		fAttackSlope(0.f),
		fDecaySlope(0.f),
		fReleaseSlope(0.f),
		dTimeAttack(attackTime),
		dTimeDecay(decayTime),
		dTimeSustain(-1.f),
		dTimeSustainStart(attackTime + decayTime),
		dTotalTime(0),
		eType(DecayType::Exponential)
	{
	}

	/** Get the current envelope value
	  */
	float operator () () const {
		return fValue;
	}

	/** Get the current envelope value
	  */
	float Get() const {
		return fValue;
	}
		
	/** Return true if the envelope is currently actively decreasing 
	  */
	bool Active() const {
		return bActive;
	}

	/** Return true if the envelope is currently in sustain mode
	  */
	bool Held() const {
		return bHeld;
	}
	
	/** Get the "slope" of envelope release phase
	  */
	float ReleaseSlope() const {
		return fReleaseSlope;
	}

	/** Get type of envelope decay
	  */
	DecayType Type() const {
		return eType;
	}

	/** Get the amount of time for envelope to rise from minimum output to sustain output value
	  */
	double AttackTime() const {
		return dTimeAttack;
	}

	/** Set the amount of time for envelope to decay to sustain level from maximum
	  */
	double DecayTime() const {
		return dTimeDecay;
	}

	/** Set the length of time to stay at the sustain level before automatically releasing
	  */
	double SustainTime() const {
		return dTimeSustain;
	}

	/** Set the length of time to release from sustain level to zero.
	  * Warning: Only works for LINEAR release decay type.
	  */
	float ReleaseTime() const {
		return (1.f / fReleaseSlope);
	}

	/** Set the "slope" of envelope release phase
	  */
	void SetReleaseSlope(const float& slope){
		fReleaseSlope = slope;
	}
		
	/** Set type of envelope decay
	  */
	void SetDecayType(const DecayType& type){
		eType = type;
	}
		
	/** Set the amount of time taken for envelope to rise from minimum output to sustain output value
	  */
	void SetAttackTime(const double& dt){
		dTimeAttack = dt;
		dTimeSustainStart = dTimeAttack + dTimeDecay;
	}
		
	/** Set the amount of time taken for envelope to decay to sustain level from maximum
	  */
	void SetDecayTime(const double& dt){
		dTimeDecay = dt;
		dTimeSustainStart = dTimeAttack + dTimeDecay;
	}

	/** Set the length of time to stay at the sustain level before automatically releasing
	  */
	void SetSustainTime(const double& dt) {
		dTimeSustain = dt;
	}

	/** Set the length of time to release from sustain level to zero.
	  * Warning: Only works for LINEAR release decay type.
	  */
	void SetReleaseTime(const float& dt) {
		fReleaseSlope = 1.f / dt;
	}

	/** Set the current driver signal level
	  */
	void Trigger(const float& val);
		
	/** Release envelope sustain and start decaying to zero
	  */
	void Release();
	
	/** Update the current envelope time step
	  */
	void Update(const double& dTime);
		
	/** Kill envelope output
	  */
	void Kill();
	
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
		
	double dTimeAttack; ///< Length of time taken for envelope to rise to output level from zero
		
	double dTimeDecay; ///< Length of time taken for envelope to decay to sustain level from maximum
		
	double dTimeSustain; ///< Length of time that envelope output is held at sustain level
		
	double dTimeSustainStart; ///< Time at which envelope begins to decay to zero
		
	double dTotalTime; ///< Time since the last driver signal was applied to the envelope
		
	DecayType eType; ///< Envelope decay type
};

} /* namespace ott */

#endif // #ifndef MATH_OTT_DECAY_ENVELOPE_HPP
