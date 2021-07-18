#include <cmath>

#include "DecayEnvelope.hpp"

namespace ott{
	void DecayEnvelope::trigger(const float& val){
		if(val > fValue){
			fSustain = val;
			dTotalTime = 0;
			fAttackSlope = (fMax - fValue) / (float)dTimeAttack;
			fDecaySlope = (dTimeDecay > 0 ? (fSustain - fMax) / (float)dTimeDecay : 0);
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
		if(bHeld){ // Attack, decay, sustain
			if(dTotalTime < dTimeAttack){ // Attack
				fValue += fAttackSlope * dTime;
			}
			else if(dTotalTime < dTimeSustainStart){ // Decay
				fValue = fMax + fDecaySlope * (dTotalTime - dTimeAttack);
			}
			else{ // Sustain
				fValue = fSustain;
				if (dTimeSustain >= 0.f && (dTotalTime >= (dTimeSustainStart + dTimeSustain)))
					release();
			}
		}
		else{ // Release
			switch(eType){ // Update envelope output value
			case DecayType::EXPONENTIAL:
				fValue = fMin + fSustain * std::exp(-fReleaseSlope * dTotalTime);
				if(fValue - fMin <= 0.000001f)
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
		dTotalTime += dTime;
	}
	
	void DecayEnvelope::kill(){
		fSustain = 0.f;
		fValue = fMin; 
		dTotalTime = 0;
		bActive = false;
	}
}

