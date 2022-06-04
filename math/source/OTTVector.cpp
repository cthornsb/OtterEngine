#include "OTTVector.hpp"

#include <cmath>

namespace ott {
namespace constants {

const Vector3 kZeroVector(0, 0, 0);
const Vector3 kUnitVectorX(1, 0, 0);
const Vector3 kUnitVectorY(0, 1, 0);
const Vector3 kUnitVectorZ(0, 0, 1);

} /* namespace constants */
} /* namespace ott */

/////////////////////////////////////////////////////////////////////
// class Vector
/////////////////////////////////////////////////////////////////////

bool ott::Vector::__equal(const Vector* rhs) const {
	if (dimension != rhs->dimension) // Vector size mismatch
		return false;
	for (uint16_t i = 0; i < dimension; i++) {
		if (elements[i] != rhs->elements[i])
			return false;
	}
	return true;
}

void ott::Vector::__assign(const Vector* rhs) {
	if (dimension != rhs->dimension) // Vector size mismatch
		return;
	for (uint16_t i = 0; i < dimension; i++) {
		elements[i] = rhs->elements[i];
	}
}

void ott::Vector::__multiply(const float& rhs) {
	for (uint16_t i = 0; i < dimension; i++) {
		elements[i] *= rhs;
	}
}

void ott::Vector::__multiply(const float& in, Vector* out) const {
	for (uint16_t i = 0; i < dimension; i++) {
		out->elements[i] = elements[i] * in;
	}
}

void ott::Vector::__divide(const float& rhs) {
	for (uint16_t i = 0; i < dimension; i++) {
		elements[i] /= rhs;
	}
}

void ott::Vector::__divide(const float& in, Vector* out) const {
	for (uint16_t i = 0; i < dimension; i++) {
		out->elements[i] = elements[i] / in;
	}
}

void ott::Vector::__add(const Vector* rhs) {
	if (dimension != rhs->dimension) // Vector size mismatch
		return;
	for (uint16_t i = 0; i < dimension; i++) {
		elements[i] += rhs->elements[i];
	}
}

void ott::Vector::__add(const Vector* in, Vector* out) const {
	if (dimension != in->dimension) // Vector size mismatch
		return;
	for (uint16_t i = 0; i < dimension; i++) {
		out->elements[i] = elements[i] + in->elements[i];
	}
}

void ott::Vector::__subtract(const Vector* rhs) {
	if (dimension != rhs->dimension) // Vector size mismatch
		return;
	for (uint16_t i = 0; i < dimension; i++) {
		elements[i] -= rhs->elements[i];
	}
}

void ott::Vector::__subtract(const Vector* in, Vector* out) const {
	if (dimension != in->dimension) // Vector size mismatch
		return;
	for (uint16_t i = 0; i < dimension; i++) {
		out->elements[i] = elements[i] - in->elements[i];
	}
}

void ott::Vector::__subtract(const Vector* lhs, const Vector* rhs, Vector* out) const {
	if (dimension != rhs->dimension) // Vector size mismatch
		return;
	for (uint16_t i = 0; i < dimension; i++) {
		out->elements[i] = lhs->elements[i] - rhs->elements[i];
	}
}

void ott::Vector::__normalize(Vector* rhs) {
	float mag = rhs->Length();
	for (uint16_t i = 0; i < dimension; i++) {
		rhs->elements[i] /= mag;
	}
}

void ott::Vector::__normalize(const Vector* in, Vector* out) const {
	float mag = in->Length();
	for (uint16_t i = 0; i < dimension; i++) {
		out->elements[i] = in->elements[i] / mag;
	}
}

float ott::Vector::__distance(const Vector* rhs) const {
	if (dimension != rhs->dimension) // Vector size mismatch
		return -1.f;
	Vector temp(dimension);
	__subtract(rhs, this, &temp);
	return temp.Length();
}

float ott::Vector::__dot(const Vector* rhs) const {
	if (dimension != rhs->dimension) // Vector size mismatch
		return -1.f;
	float retval = 0.f;
	for (uint16_t i = 0; i < dimension; i++) {
		retval += elements[i] * rhs->elements[i];
	}
	return retval;
}

float ott::Vector::__angle(const Vector* rhs) const {
	if (dimension != rhs->dimension) // Vector size mismatch
		return -1.f;
	return std::acos(__cosTheta(rhs));
}

float ott::Vector::__cosTheta(const Vector* rhs) const {
	if (dimension != rhs->dimension) // Vector size mismatch
		return -1.f;
	return (__dot(rhs) / (this->Length() * rhs->Length()));
}

void ott::Vector::__negate(const Vector* in, Vector* out) const {
	for (uint16_t i = 0; i < dimension; i++) {
		out->elements[i] = in->elements[i] * -1.f;
	}
}

/////////////////////////////////////////////////////////////////////
// class Vector3
/////////////////////////////////////////////////////////////////////

ott::Vector3 ott::Vector3::Cross(const Vector& rhs) const {
	return Vector3(
		(elements[1] * rhs.elements[2] - rhs.elements[1] * elements[2]),
		(rhs.elements[0] * elements[2] - elements[0] * rhs.elements[2]),
		(elements[0] * rhs.elements[1] - rhs.elements[0] * elements[1])
	);
}
