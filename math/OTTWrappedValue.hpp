#ifndef MATH_OTT_WRAPPED_VALUE_HPP
#define MATH_OTT_WRAPPED_VALUE_HPP

namespace ott {

class WrappedValue {
public:
	/** Default constructor
	  */
	WrappedValue() :
		value(0.f),
		minVal(0.f),
		maxVal(1.f),
		delta(0.f),
		lock(false)
	{
	}

	/** Value range constructor
	  * @param initial Initial value
	  * @param min_ Minimum value
	  * @param max_ Maximum value
	  * @param lockValue If set, value will be clamped between min_ and max_, otherwise value will wrap around
	  */
	WrappedValue(const float& initial, const float& min_, const float& max_, bool lockValue = false) :
		value(initial),
		minVal(min_),
		maxVal(max_),
		delta(max_ - min_),
		lock(lockValue)
	{
	}

	/** Get the current value
	  */
	float operator () () const {
		return value;
	}

	/** Assignment operator
	  * Limits are not checked.
	  */
	float operator = (const float& rhs){
		return (value = rhs);
	}

	/** Addition operator
	  */
	float operator += (const float& rhs){
		return (value = this->Wrap(rhs));
	}

	/** Subtraction operator
	  */
	float operator -= (const float& rhs){
		return (value = this->Wrap(-rhs));
	}

	/** Addition operator
	  */
	float operator + (const float& rhs) const{
		return this->Wrap(rhs);
	}

	/** Subtraction operator
	  */
	float operator - (const float& rhs) const{
		return this->Wrap(-rhs);
	}

	/** Get the current value
	  */
	float Get() const{
		return value;
	}

private:
	float value; ///< Wrapped value
	
	float minVal; ///< Minimum possible value
	
	float maxVal; ///< Maximum possible value
	
	float delta; ///< Possible range of value

	bool lock; ///< Set if value is clamped to its minimum and maximum limits

	/** Wrap an input value between a minimum and maximum
	  * Alternatively, if (lock) is set, value is clamped between minVal and maxVal.
	  */
	float Wrap(const float& delta) const;
};

} /* namespace ott */

#endif // #ifndef MATH_OTT_WRAPPED_VALUE_HPP
