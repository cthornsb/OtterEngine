#ifndef WRAPPED_VALUE_HPP
#define WRAPPED_VALUE_HPP

class WrappedValue {
public:
	WrappedValue() :
		value(0),
		minVal(0),
		maxVal(0),
		delta(0),
		lock(false)
	{
	}

	WrappedValue(const float& initial, const float& min_, const float& max_, bool lockValue = false) :
		value(initial),
		minVal(min_),
		maxVal(max_),
		delta(max_ - min_),
		lock(lockValue)
	{
	}

	float operator = (const float& rhs) { return (value = rhs); }

	float operator += (const float& rhs) { return (value = wrap(rhs)); }

	float operator -= (const float& rhs) { return (value = wrap(-rhs)); }

	float operator + (const float& rhs) const { return wrap(rhs); }

	float operator - (const float& rhs) const { return wrap(-rhs); }

	float get() const { return value; }

private:
	float value;
	float minVal;
	float maxVal;
	float delta;

	bool lock;

	float wrap(const float& delta) const;
};

#endif