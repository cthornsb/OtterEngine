#include <algorithm>

#include "MovingAverage.hpp"

double ott::MovingAverage::operator () (const double& value) {
	add(value);
	return (dTotal / nValues);
}

void ott::MovingAverage::add(const double& value) {
	dTotal += (value - *iter);
	*iter = value;
	iter++;
	if (!bFull)
		nValues++;
	if (iter == dValues.end()) {
		iter = dValues.begin();
		bFull = true;
	}
}

void ott::MovingAverage::reset() {
	std::fill(dValues.begin(), dValues.end(), 0);
	bFull = false;
	nValues = 0;
	dTotal = 0;
	iter = dValues.begin();
}

void ott::MovingAverage::recount() {
	dTotal = 0;
	for (auto val = dValues.cbegin(); val != dValues.cend(); val++)
		dTotal += (*val);
}
