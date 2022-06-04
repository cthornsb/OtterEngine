#include "OTTMovingAverage.hpp"

#include <algorithm>

double ott::MovingAverage::operator () (const double& value) {
	this->Add(value);
	return (dTotal / nValues);
}

void ott::MovingAverage::Add(const double& value) {
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

void ott::MovingAverage::Reset() {
	std::fill(dValues.begin(), dValues.end(), 0);
	bFull = false;
	nValues = 0;
	dTotal = 0;
	iter = dValues.begin();
}

void ott::MovingAverage::Recount() {
	dTotal = 0;
	for (auto val = dValues.cbegin(); val != dValues.cend(); val++)
		dTotal += (*val);
}
