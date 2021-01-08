#include <iostream>
#include <cmath>

#include "Vector.hpp"

const Vector3 zeroVector(0, 0, 0);
const Vector3 unitVectorX(1, 0, 0);
const Vector3 unitVectorY(0, 1, 0);
const Vector3 unitVectorZ(0, 0, 1);

/////////////////////////////////////////////////////////////////////
// class Vector
/////////////////////////////////////////////////////////////////////

bool Vector::__equal(const Vector* rhs) const {
	if (dimension != rhs->dimension) // Vector size mismatch
		return false;
	for (unsigned short i = 0; i < dimension; i++) {
		if (elements[i] != rhs->elements[i])
			return false;
	}
	return true;
}

void Vector::__assign(const Vector* rhs) {
	if (dimension != rhs->dimension) // Vector size mismatch
		return;
	for (unsigned short i = 0; i < dimension; i++) {
		elements[i] = rhs->elements[i];
	}
}

void Vector::__multiply(const float& rhs) {
	for (unsigned short i = 0; i < dimension; i++) {
		elements[i] *= rhs;
	}
}

void Vector::__multiply(const float& in, Vector* out) const {
	for (unsigned short i = 0; i < dimension; i++) {
		out->elements[i] = elements[i] * in;
	}
}

void Vector::__divide(const float& rhs) {
	for (unsigned short i = 0; i < dimension; i++) {
		elements[i] /= rhs;
	}
}

void Vector::__divide(const float& in, Vector* out) const {
	for (unsigned short i = 0; i < dimension; i++) {
		out->elements[i] = elements[i] / in;
	}
}

void Vector::__add(const Vector* rhs) {
	if (dimension != rhs->dimension) // Vector size mismatch
		return;
	for (unsigned short i = 0; i < dimension; i++) {
		elements[i] += rhs->elements[i];
	}
}

void Vector::__add(const Vector* in, Vector* out) const {
	if (dimension != in->dimension) // Vector size mismatch
		return;
	for (unsigned short i = 0; i < dimension; i++) {
		out->elements[i] = elements[i] + in->elements[i];
	}
}

void Vector::__subtract(const Vector* rhs) {
	if (dimension != rhs->dimension) // Vector size mismatch
		return;
	for (unsigned short i = 0; i < dimension; i++) {
		elements[i] -= rhs->elements[i];
	}
}

void Vector::__subtract(const Vector* in, Vector* out) const {
	if (dimension != in->dimension) // Vector size mismatch
		return;
	for (unsigned short i = 0; i < dimension; i++) {
		out->elements[i] = elements[i] - in->elements[i];
	}
}

void Vector::__subtract(const Vector* lhs, const Vector* rhs, Vector* out) const {
	if (dimension != rhs->dimension) // Vector size mismatch
		return;
	for (unsigned short i = 0; i < dimension; i++) {
		out->elements[i] = lhs->elements[i] - rhs->elements[i];
	}
}

void Vector::__normalize(Vector* rhs) {
	float mag = rhs->length();
	for (unsigned short i = 0; i < dimension; i++) {
		rhs->elements[i] /= mag;
	}
}

void Vector::__normalize(const Vector* in, Vector* out) const {
	float mag = in->length();
	for (unsigned short i = 0; i < dimension; i++) {
		out->elements[i] = in->elements[i] / mag;
	}
}

float Vector::__distance(const Vector* rhs) const {
	if (dimension != rhs->dimension) // Vector size mismatch
		return -1.f;
	Vector temp(dimension);
	__subtract(rhs, this, &temp);
	return temp.length();
}

float Vector::__dot(const Vector* rhs) const {
	if (dimension != rhs->dimension) // Vector size mismatch
		return -1.f;
	float retval = 0.f;
	for (unsigned short i = 0; i < dimension; i++) {
		retval += elements[i] * rhs->elements[i];
	}
	return retval;
}

float Vector::__angle(const Vector* rhs) const {
	if (dimension != rhs->dimension) // Vector size mismatch
		return -1.f;
	return std::acos(__cosTheta(rhs));
}

float Vector::__cosTheta(const Vector* rhs) const {
	if (dimension != rhs->dimension) // Vector size mismatch
		return -1.f;
	return (__dot(rhs) / (this->length() * rhs->length()));
}

void Vector::__negate(const Vector* in, Vector* out) const {
	for (unsigned short i = 0; i < dimension; i++) {
		out->elements[i] = in->elements[i] * -1.f;
	}
}

/////////////////////////////////////////////////////////////////////
// class Vector2
/////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////
// class Vector3
/////////////////////////////////////////////////////////////////////

Vector3 Vector3::cross(const Vector& rhs) const {
	return Vector3(
		(elements[1] * rhs.elements[2] - rhs.elements[1] * elements[2]),
		(rhs.elements[0] * elements[2] - elements[0] * rhs.elements[2]),
		(elements[0] * rhs.elements[1] - rhs.elements[0] * elements[1])
	);
}

/////////////////////////////////////////////////////////////////////
// class Vector4
/////////////////////////////////////////////////////////////////////