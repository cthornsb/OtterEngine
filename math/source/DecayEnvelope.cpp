#include "DecayEnvelope.hpp"

namespace ott{
	void DecayEnvelope::setDriverSignal(const float& val){
		fCurrent = val;
		if(val > fValue){ // Instantaneous impulse
			fValue = val; 
			dTotalTime = 0;
		}
		bActive = true;
	}
	
	void DecayEnvelope::update(const double& dTime){
		if(!bActive)
			return;
		dTotalTime += dTime;
		fValue -= fAcceleration * dTotalTime * dTotalTime;
		if(fValue <= fMin){
			fValue = fMin;
			bActive = false;
		}
	}
}

