#ifndef MOVING_AVERAGE_HPP
#define MOVING_AVERAGE_HPP

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
		MovingAverage(const unsigned short& N) :
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
		void add(const double& value);

		/** Reset the current running average and all saved values
		  */
		void reset();

	private:
		bool bFull; ///< Set when the moving average is full of values

		unsigned short nValues; ///< Total number of values to average

		double dTotal; ///< Current running average

		std::vector<double> dValues; ///< Vector of running average values

		std::vector<double>::iterator iter; ///< The next value which will be modified

		/** Recompute the total sum to remove any rounding errors which may have been introduced
		  */
		void recount();
	};

} // namespace ott

#endif // ifndef MOVING_AVERAGE_HPP