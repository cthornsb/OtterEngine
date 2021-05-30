#include <cmath>

#include "DecayEnvelope.hpp"

namespace ott{
	void DecayEnvelope::trigger(const float& val){
		if(val > fValue){
			fSustain = val;
			dTotalTime = 0;
			fAttackSlope = (fMax - fValue) / fTimeAttack;
			fDecaySlope = (fSustain - fMax) / fTimeDecay;
		}
		bActive = true;
		bHeld = true;
	}
	
	void DecayEnvelope::release(){
		dTotalTime = 0;
		bHeld = false;
	}

	void DecayEnvelope::update(const double& dTime){
		if(!bActive)
			return;
		dTotalTime += dTime;
		if(bHeld){ // Attack, decay, sustain
			if(dTotalTime < fTimeAttack){ // Attack
				fValue += fAttackSlope * dTime;
			}
			else if(dTotalTime < fTimeSustainStart){ // Decay
				fValue = fMax + fDecaySlope * (dTotalTime - fTimeAttack);
			}
			else{ // Sustain
				fValue = fSustain;			
			}
		}
		else{ // Release
			switch(eType){ // Update envelope output value
			case DecayType::EXPONENTIAL:
				fValue = fMin + fSustain * std::exp(-fReleaseSlope * dTotalTime);
				if(fValue - fMin <= 1E-6)
					kill(); // Cease output
				break;
			case DecayType::PARABOLIC:
				fValue = fSustain - fReleaseSlope * dTotalTime * dTotalTime; 
				if(fValue <= fMin)
					kill(); // Cease output
				break;
			case DecayType::LINEAR: // Constant decrease from sustain output to minimum output
				fValue = fSustain - fReleaseSlope * dTotalTime;
				if(fValue <= fMin)
					kill(); // Cease output
				break;
			case DecayType::DELTA: // Instantaneous drop to minimum envelope output
				kill();
				break;
			default:
				break;
			}
		}
	}
	
	void DecayEnvelope::kill(){
		fSustain = 0.f;
		fValue = fMin; 
		dTotalTime = 0;
		bActive = false;
	}
}

