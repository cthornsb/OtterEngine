#include "OTTDecayEnvelope.hpp"

#include <cmath>

void ott::DecayEnvelope::Trigger(const float& val){
	if(val > fValue){
		fSustain = val;
		dTotalTime = 0;
		fAttackSlope = (fMax - fValue) / (float)dTimeAttack;
		fDecaySlope = (dTimeDecay > 0 ? (fSustain - fMax) / (float)dTimeDecay : 0);
	}
	bActive = true;
	bHeld = true;
}
	
void ott::DecayEnvelope::Release(){
	dTotalTime = 0;
	bHeld = false;
}

void ott::DecayEnvelope::Update(const double& dTime){
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
				this->Release();
		}
	}
	else{ // Release
		switch(eType){ // Update envelope output value
		case DecayType::Exponential:
			fValue = fMin + fSustain * std::exp(-fReleaseSlope * dTotalTime);
			if(fValue - fMin <= 0.000001f)
				this->Kill(); // Cease output
			break;
		case DecayType::Parabolic:
			fValue = fSustain - fReleaseSlope * dTotalTime * dTotalTime; 
			if(fValue <= fMin)
				this->Kill(); // Cease output
			break;
		case DecayType::Linear: // Constant decrease from sustain output to minimum output
			fValue = fSustain - fReleaseSlope * dTotalTime;
			if(fValue <= fMin)
				this->Kill(); // Cease output
			break;
		case DecayType::Delta: // Instantaneous drop to minimum envelope output
			this->Kill();
			break;
		default:
			break;
		}
	}
	dTotalTime += dTime;
}
	
void ott::DecayEnvelope::Kill(){
	fSustain = 0.f;
	fValue = fMin; 
	dTotalTime = 0;
	bActive = false;
}
