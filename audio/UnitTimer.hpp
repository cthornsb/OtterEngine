#ifndef UNIT_TIMER_HPP
#define UNIT_TIMER_HPP

class UnitTimer{
public:
	/** Default constructor
	  */
	UnitTimer() :
		bEnabled(false),
		nPeriod(0),
		nCounter(0),
		nPeriodMultiplier(1),
		nCyclesSinceLastClock(0),
		nWavelengthPeriod(8)
	{
	}

	/** Constructor specifying the master clock period (in 1 MHz clock ticks)
	  */
	UnitTimer(const uint32_t& period) :
		bEnabled(false),
		nPeriod(period),
		nCounter(0),
		nPeriodMultiplier(1),
		nCyclesSinceLastClock(0),
		nWavelengthPeriod(8)
	{
	}
	
	/** Get the current period of the timer (computed from the channel's 11-bit frequency)
	  */
	uint16_t getPeriod() const { 
		return (nPeriod / nPeriodMultiplier); 
	}
	
	/** Get the reciprocal of the 11-bit frequency of the timer
	  */
	uint16_t getFrequency() const {
		return (2048 - getPeriod());
	}

	/** Get the actual frequency (in Hz)
	  */
	virtual float getRealFrequency() const {
		return (4194304.f / (nWavelengthPeriod * nPeriod)); // in Hz
	}

	/** Return true if the timer is enabled, and return false otherwise
	  */
	bool isEnabled() const {
		return bEnabled;
	}

	/** Set the period of the timer (in 1 MHz master clock ticks)
	  */
	virtual void setPeriod(const uint16_t& period) {
		nPeriod = nPeriodMultiplier * period;
	}

	/** Set the frequency and period of the timer
	  */
	void setFrequency(const uint16_t& freq){
		setPeriod(2048 - (freq & 0x7ff));
	}
	
	/** Set the frequency and period of the timer using two input bytes
	  */
	void setFrequency(const uint8_t& lowByte, const uint8_t& highByte){
		setPeriod(2048 - (((highByte & 0x7) << 8) + lowByte));
	}

	/** Enable the timer
	  */
	void enable() { 
		nCyclesSinceLastClock = 0; // Reset master clock
		bEnabled = true; 
		userEnable();
	}
	
	/** Disable the timer
	  */
	void disable() { 
		bEnabled = false; 
		userDisable();
	}

	/** Clock the timer, returning true if the phase rolled over and returning false otherwise
	  */
	bool clock();
	
	/** Reload the unit timer with its period
	  */
	virtual void reload();

	/** Reset all timer values and flags
	  */
	virtual void reset();

protected:
	bool bEnabled; ///< Timer enabled flag
	
	uint16_t nPeriod; ///< Period of timer
	
	uint16_t nCounter; ///< Current timer value
	
	uint16_t nPeriodMultiplier; ///< Period multiplier factor
	
	uint32_t nCyclesSinceLastClock; ///< Number of input clock ticks since last unit clock rollover
	
	uint32_t nWavelengthPeriod; ///< Audio output waveform wavelength expressed in input clock ticks
	
	/** Additional operations performed whenever enable() is called
	  */
	virtual void userEnable() { }
	
	/** Additional operations performed whenever disable() is called
	  */
	virtual void userDisable() { }
	
	/** Method called when unit timer clocks over (every nPeriod input clock ticks)
	  */	
	virtual void rollover() { 
		reload();
	}
};

#endif
