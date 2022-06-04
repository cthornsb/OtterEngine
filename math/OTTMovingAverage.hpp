#ifndef MATH_OTT_MOVING_AVERAGE_HPP
#define MATH_OTT_MOVING_AVERAGE_HPP

#include <vector>

namespace ott {

class MovingAverage {
public:
	/** Default constructor (100 values)
	  */
	MovingAverage() :
		bFull(false),
		nValues(0),
		dTotal(0),
		dValues(100, 0),
		iter(dValues.begin())
	{
	}

	/** N value constructor
	  */
	MovingAverage(const uint16_t& N) :
		bFull(false),
		nValues(0),
		dTotal(0),
		dValues(N, 0),
		iter(dValues.begin())
	{
	}

	/** Get the current running average
	  */
	double operator () () const {
		return (nValues > 0 ? (dTotal / nValues) : 0);
	}

	/** Add a new value to the running average and subtract the oldest value
	  * @return The current running average
	  */
	double operator () (const double& value);

	/** Add a new value to the running average and subtract the oldest value
	  */
	void Add(const double& value);

	/** Reset the current running average and all saved values
	  */
	void Reset();

private:
	bool bFull; ///< Set when the moving average is full of values

	uint16_t nValues; ///< Total number of values to average

	double dTotal; ///< Current running average

	std::vector<double> dValues; ///< Vector of running average values

	std::vector<double>::iterator iter; ///< The next value which will be modified

	/** Recompute the total sum to remove any rounding errors which may have been introduced
	  */
	void Recount();
};

} /* namespace ott */

#endif // #ifndef MATH_OTT_MOVING_AVERAGE_HPP