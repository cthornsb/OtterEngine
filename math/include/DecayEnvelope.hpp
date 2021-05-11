#ifndef DECAY_ENVELOPE_HPP
#define DECAY_ENVELOPE_HPP

#include <iostream> // TEMP

namespace ott{
	class DecayEnvelope{
	public:
		/** Default constructor
		  */
		DecayEnvelope() :
			bActive(false),
			fCurrent(0.f),
			fValue(0.f),
			fMin(0.f),
			fAcceleration(1.f),
			dTotalTime(0)
		{
		}
		
		/** Initial value constructor
		  */
		DecayEnvelope(const float& value, const float& minValue) :
			bActive(false),
			fCurrent(value),
			fValue(value),
			fMin(minValue),
			fAcceleration(1.f),
			dTotalTime(0)
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
		
		/** Set "acceleration" of decay (units / s^2)
		  */
		void setAcceleration(const float& acceleration){
			fAcceleration = acceleration;
		}
		
		/** Set the current driver signal level
		  */
		void setDriverSignal(const float& val);
		
		/** Update the current envelope time step
		  */
		void update(const double& dTime);
	
	private:
		bool bActive; ///< Set if envelope is currently decreasing
	
		float fCurrent; ///< Current value of driver signal

		float fValue; ///< Current envelope value

		float fMin; ///< Minimum envelope value
		
		float fAcceleration; ///< Envelope decay acceleration (units / s^2)
		
		double dTotalTime; ///< Time since the last driver signal was applied to the envelope
	};
}

#endif // ifndef DECAY_ENVELOPE_HPP
