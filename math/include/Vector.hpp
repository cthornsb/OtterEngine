#ifndef MATH_VECTOR_HPP
#define MATH_VECTOR_HPP

#include <ostream>
#include <vector>
#include <math.h>

class Vector {
public:
	friend class Matrix;

	std::vector<float> elements; ///< Vector elements

	unsigned short dimension; ///< dimension of vector

	/** Default constructor (null)
	  */
	Vector() :
		elements(),
		dimension(0)
	{
	}

	/** Empty vector constructor
	  */
	Vector(const unsigned short& size) :
		elements(size, 0.f),
		dimension(size)
	{
	}

	/** Vector fill constructor
	  */
	Vector(const unsigned short& size, const float& value) :
		Vector(size)
	{
		for (unsigned short i = 0; i < dimension; i++) {
			elements[i] = value;
		}
	}

	/** Explicit element constructor
	  */
	Vector(const unsigned short& size, const float* values) :
		Vector(size)
	{
		for (unsigned short i = 0; i < dimension; i++) { // over all elements
			elements[i] = values[i];
		}
	}

	/** Copy constructor
	  */
	Vector(const Vector& other) :
		elements(other.elements),
		dimension(other.dimension)
	{
	}

	/** Stream insertion operator
	  */
	friend std::ostream& operator << (std::ostream& out, const Vector& vec) {
		return vec.__stream(out);
	}

	/** Const vector element access
	  */
	float operator [] (const unsigned short& index) const {
		return elements.at(index);
	}

	/** Vector element access
	  */
	float& operator [] (const unsigned short& index) {
		return elements.at(index);
	}

	/** Const vector element access
	  */
	float at(const unsigned short& index) const {
		return elements.at(index);
	}

	/** Vector element access
	  */
	float& at(const unsigned short& index) {
		return elements.at(index);
	}

	/** Get a pointer to the 0th component of this vector
	  */
	float* getData() {
		return elements.data();
	}

	/** Get a const pointer to the 0th component of this vector
	  */
	const float* getConstData() const {
		return (const float*)elements.data();
	}

	/** Equality operator
	  */
	bool operator == (const Vector& rhs) const {
		return __equal(&rhs);
	}

	/** Inequality operator
	  */
	bool operator != (const Vector& rhs) const {
		return (!__equal(&rhs));
	}

	/** Multiply this vector by another vector and return the resulting dot product
	  */
	float operator * (const Vector& rhs) const {
		return __dot(&rhs);
	}

	/** Multiply this Vector by a constant and return the resulting Vector
	  */
	Vector operator * (const float& rhs) const {
		Vector retval(dimension);
		__multiply(rhs, &retval);
		return retval;
	}

	/** Divide this Vector by a constant and return the result
	  */
	Vector operator / (const float& rhs) const {
		Vector retval(dimension);
		__divide(rhs, &retval);
		return retval;
	}

	/** Add a Vector to this one and return the result
	  */
	Vector operator + (const Vector& rhs) const {
		Vector retval(dimension);
		__add(&rhs, &retval);
		return retval;
	}

	/** Subtract a Vector from this one and return the result
	  */
	Vector operator - (const Vector& rhs) const {
		Vector retval(dimension);
		__subtract(&rhs, &retval);
		return retval;
	}

	/** Unary operator
	  */
	Vector operator - () const {
		Vector retval(dimension);
		__negate(this, &retval);
		return retval;
	}

	/** Assignment operator
	  */
	Vector& operator = (const Vector& rhs) {
		__assign(&rhs);
		return (*this);
	}

	/** Multiply this Vector by a constant and return the result
	  */
	Vector& operator *= (const float& rhs) {
		__multiply(rhs);
		return (*this);
	}

	/** Divide this Vector by a constant and return the result
	  */
	Vector& operator /= (const float& rhs) {
		__divide(rhs);
		return (*this);
	}

	/** Add a Vector to this one and return the result
	  */
	Vector& operator += (const Vector& rhs) {
		__add(&rhs);
		return (*this);
	}

	/** Subtract a Vector from this one and return the result
	  */
	Vector& operator -= (const Vector& rhs) {
		__subtract(&rhs);
		return (*this);
	}

	/** Normalize this vector in place
	  */
	Vector& normInPlace() {
		__normalize(this);
		return (*this);
	}

	/** Normalize this and store into a resultant vector
	  */
	Vector normalize() const {
		Vector retval(dimension);
		__normalize(this, &retval);
		return retval;
	}

	/** Compute the distance between this vector and an input vector
	  * @return The distance or -1 if the input vector has the incorrect dimension
	  */
	float distance(const Vector& rhs) const {
		return __distance(&rhs);
	}

	/** Compute the dot product between this vector and an input vector
	  * @return The dot product or -1 if the input vector has the incorrect dimension
	  */
	float dot(const Vector& rhs) const {
		return __dot(&rhs);
	}

	/** Compute the angle between this vector and an input vector
	  * @return The angle (in radians) or -1 if the input vector has the incorrect dimension
	  */
	float angle(const Vector& rhs) const {
		return __angle(&rhs);
	}

	/** Compute the cosine of the angle between this vector and an input vector
	  * @return The cosine or -1 if the input vector has the incorrect dimension
	  */
	float cosTheta(const Vector& rhs) const {
		return __cosTheta(&rhs);
	}

	/** Return the length of this vector
	  */
	float length() const {
		return sqrtf(square());
	}

	/** Return the sum of the squares of the elements of this vector
	  */
	float square() const {
		float retval = 0.f;
		for (unsigned short i = 0; i < dimension; i++) {
			retval += elements[i] * elements[i];
		}
		return retval;
	}

	/** Negate all elements of this vector
	  */
	void invert() {
		for (unsigned short i = 0; i < dimension; i++) {
			elements[i] *= -1.f;
		}
	}

	/** Zero all elements of this vector
	  */
	void zero() {
		for (unsigned short i = 0; i < dimension; i++) {
			elements[i] = 0.f;
		}
	}

protected:
	/** Equality operator
	  */
	bool __equal(const Vector* rhs) const;

	/** Assignment operator
	  */
	void __assign(const Vector* rhs);

	/** Multiply this vector by a constant in place
	  */
	void __multiply(const float& rhs);

	/** Multiply this vector by a constant and store the result in an output vector
	  */
	void __multiply(const float& in, Vector* out) const;

	/** Divide this vector by a constant in place
	  */
	void __divide(const float& rhs);

	/** Divide this vector by a constant and store the result in an output vector
	  */
	void __divide(const float& in, Vector* out) const;

	/** Add a vector to this vector in place
	  */
	void __add(const Vector* rhs);

	/** Add a vector to this one and store the result in an output vector
	  */
	void __add(const Vector* in, Vector* out) const;

	/** Subtract a vector from this vector in place
	  * Performs the operation this -= in
	  */
	void __subtract(const Vector* rhs);

	/** Subtract a vector from this vector and store the result in an output vector
	  * Performs the operation out = this - in
	  */
	void __subtract(const Vector* in, Vector* out) const;

	/** Subtract a vector from another vector and store the result in an output vector
	  * Performs the operation out = lhs - rhs
	  */
	void __subtract(const Vector* lhs, const Vector* rhs, Vector* out) const;

	/** Normalize input vector in place
	  */
	void __normalize(Vector* rhs);

	/** Normalize input vector into a resultant vector
	  */
	void __normalize(const Vector* in, Vector* out) const;

	/** Compute the distance between this vector and an input vector
	  * @return The distance or -1 if the input vector has the incorrect dimension
	  */
	float __distance(const Vector* rhs) const;

	/** Compute the dot product between this vector and an input vector
	  * @return The dot product or -1 if the input vector has the incorrect dimension
	  */
	float __dot(const Vector* rhs) const;

	/** Compute the angle between this vector and an input vector
	  * @return The angle (in radians) or -1 if the input vector has the incorrect dimension
	  */
	float __angle(const Vector* rhs) const;

	/** Compute the cosine of the angle between this vector and an input vector
	  * @return The cosine or -1 if the input vector has the incorrect dimension
	  */
	float __cosTheta(const Vector* rhs) const;

	/** Negate all elements of a vector and store the result in an output vector
	  */
	void __negate(const Vector* in, Vector* out) const;

	/** Stream insertion
	  */
	std::ostream& __stream(std::ostream& out) const {
		out << "(" << elements[0];
		for (unsigned short i = 1; i < dimension; i++) {
			out << ", " << elements[i];
		}
		out << ")";
		return out;
	}
};

class Vector2 : public Vector {
public:
	/** Empty 2-dimensional Vector
	  */
	Vector2() : Vector(2) { }

	/** Filled vector constructor
	  */
	Vector2(const float& value) :
		Vector(2, value)
	{
	}

	/** Explicit element constructor
	  */
	Vector2(const float* values) :
		Vector(2, values)
	{
	}

	/** Explicit Vector element constructor
	  */
	Vector2(const float& a0, const float& a1) :
		Vector(2)
	{
		elements[0] = a0; 
		elements[1] = a1;
	}

	/** Stream insertion operator
	  */
	friend std::ostream& operator << (std::ostream& out, const Vector2& vec) {
		return vec.__stream(out);
	}

	/** Equality operator
	  */
	bool operator == (const Vector2& rhs) const {
		return __equal(&rhs);
	}

	/** Inequality operator
	  */
	bool operator != (const Vector2& rhs) const {
		return (!__equal(&rhs));
	}

	/** Multiply this vector by another vector and return the resulting dot product
	  */
	float operator * (const Vector2& rhs) const {
		return __dot(&rhs);
	}

	/** Multiply this Vector by a constant and return the resulting Vector
	  */
	Vector2 operator * (const float& rhs) const {
		Vector2 retval(dimension);
		__multiply(rhs, &retval);
		return retval;
	}

	/** Divide this Vector by a constant and return the result
	  */
	Vector2 operator / (const float& rhs) const {
		Vector2 retval(dimension);
		__divide(rhs, &retval);
		return retval;
	}

	/** Add a Vector to this one and return the result
	  */
	Vector2 operator + (const Vector2& rhs) const {
		Vector2 retval(dimension);
		__add(&rhs, &retval);
		return retval;
	}

	/** Subtract a Vector from this one and return the result
	  */
	Vector2 operator - (const Vector2& rhs) const {
		Vector2 retval(dimension);
		__subtract(&rhs, &retval);
		return retval;
	}

	/** Unary operator
	  */
	Vector2 operator - () const {
		Vector2 retval;
		__negate(this, &retval);
		return retval;
	}

	/** Assignment operator
	  */
	Vector2& operator = (const Vector2& rhs) {
		__assign(&rhs);
		return (*this);
	}

	/** Multiply this Vector by a constant and return the result
	  */
	Vector2& operator *= (const float& rhs) {
		__multiply(rhs);
		return (*this);
	}

	/** Divide this Vector by a constant and return the result
	  */
	Vector2& operator /= (const float& rhs) {
		__divide(rhs);
		return (*this);
	}

	/** Add a Vector to this one and return the result
	  */
	Vector2& operator += (const Vector2& rhs) {
		__add(&rhs);
		return (*this);
	}

	/** Subtract a Vector from this one and return the result
	  */
	Vector2& operator -= (const Vector2& rhs) {
		__subtract(&rhs);
		return (*this);
	}

	/** Normalize this vector in place
	  */
	Vector2& normInPlace() {
		__normalize(this);
		return (*this);
	}

	/** Normalize this and store into a resultant vector
	  */
	Vector2 normalize() const {
		Vector2 retval(dimension);
		__normalize(this, &retval);
		return retval;
	}

	/** Compute the distance between this vector and an input vector
	  * @return The distance or -1 if the input vector has the incorrect dimension
	  */
	float distance(const Vector2& rhs) const {
		return __distance(&rhs);
	}

	/** Compute the dot product between this vector and an input vector
	  * @return The dot product or -1 if the input vector has the incorrect dimension
	  */
	float dot(const Vector2& rhs) const {
		return __dot(&rhs);
	}

	/** Compute the angle between this vector and an input vector
	  * @return The angle (in radians) or -1 if the input vector has the incorrect dimension
	  */
	float angle(const Vector2& rhs) const {
		return __angle(&rhs);
	}

	/** Compute the cosine of the angle between this vector and an input vector
	  * @return The cosine or -1 if the input vector has the incorrect dimension
	  */
	float cosTheta(const Vector2& rhs) const {
		return __cosTheta(&rhs);
	}

	/** Return x-axis component
	  */
	float& x() {
		return elements[0];
	}

	/** Return y-axis component
	  */
	float& y() {
		return elements[1];
	}
};

class Vector3 : public Vector {
public:
	/** Empty 2-dimensional Vector
	  */
	Vector3() : Vector(3) {
	}

	/** Filled vector constructor
	  */
	Vector3(const float& value) :
		Vector(3, value)
	{
	}

	/** Explicit element constructor
	  */
	Vector3(const float* values) :
		Vector(3, values)
	{
	}

	/** Explicit Vector element constructor
	  */
	Vector3(const float& a0, const float& a1, const float& a2) :
		Vector(3)
	{
		elements[0] = a0;
		elements[1] = a1;
		elements[2] = a2;
	}

	/** Stream insertion operator
	  */
	friend std::ostream& operator << (std::ostream& out, const Vector3& vec) {
		return vec.__stream(out);
	}

	/** Equality operator
	  */
	bool operator == (const Vector3& rhs) const {
		return __equal(&rhs);
	}

	/** Inequality operator
	  */
	bool operator != (const Vector3& rhs) const {
		return (!__equal(&rhs));
	}

	/** Multiply this vector by another vector and return the resulting dot product
	  */
	float operator * (const Vector3& rhs) const {
		return __dot(&rhs);
	}

	/** Multiply this Vector by a constant and return the resulting Vector
	  */
	Vector3 operator * (const float& rhs) const {
		Vector3 retval(dimension);
		__multiply(rhs, &retval);
		return retval;
	}

	/** Divide this Vector by a constant and return the result
	  */
	Vector3 operator / (const float& rhs) const {
		Vector3 retval(dimension);
		__divide(rhs, &retval);
		return retval;
	}

	/** Add a Vector to this one and return the result
	  */
	Vector3 operator + (const Vector3& rhs) const {
		Vector3 retval(dimension);
		__add(&rhs, &retval);
		return retval;
	}

	/** Subtract a Vector from this one and return the result
	  */
	Vector3 operator - (const Vector3& rhs) const {
		Vector3 retval(dimension);
		__subtract(&rhs, &retval);
		return retval;
	}

	/** Unary operator
	  */
	Vector3 operator - () const {
		Vector3 retval;
		__negate(this, &retval);
		return retval;
	}

	/** Assignment operator
	  */
	Vector3& operator = (const Vector3& rhs) {
		__assign(&rhs);
		return (*this);
	}

	/** Multiply this Vector by a constant and return the result
	  */
	Vector3& operator *= (const float& rhs) {
		__multiply(rhs);
		return (*this);
	}

	/** Divide this Vector by a constant and return the result
	  */
	Vector3& operator /= (const float& rhs) {
		__divide(rhs);
		return (*this);
	}

	/** Add a Vector to this one and return the result
	  */
	Vector3& operator += (const Vector3& rhs) {
		__add(&rhs);
		return (*this);
	}

	/** Subtract a Vector from this one and return the result
	  */
	Vector3& operator -= (const Vector3& rhs) {
		__subtract(&rhs);
		return (*this);
	}

	/** Normalize this vector in place
	  */
	Vector3& normInPlace() {
		__normalize(this);
		return (*this);
	}

	/** Normalize this and store into a resultant vector
	  */
	Vector3 normalize() const {
		Vector3 retval(dimension);
		__normalize(this, &retval);
		return retval;
	}

	/** Compute the distance between this vector and an input vector
	  * @return The distance or -1 if the input vector has the incorrect dimension
	  */
	float distance(const Vector3& rhs) const {
		return __distance(&rhs);
	}

	/** Compute the dot product between this vector and an input vector
	  * @return The dot product or -1 if the input vector has the incorrect dimension
	  */
	float dot(const Vector3& rhs) const {
		return __dot(&rhs);
	}

	/** Compute the angle between this vector and an input vector
	  * @return The angle (in radians) or -1 if the input vector has the incorrect dimension
	  */
	float angle(const Vector3& rhs) const {
		return __angle(&rhs);
	}

	/** Compute the cosine of the angle between this vector and an input vector
	  * @return The cosine or -1 if the input vector has the incorrect dimension
	  */
	float cosTheta(const Vector3& rhs) const {
		return __cosTheta(&rhs);
	}

	/** Return x-axis component
	  */
	float& x() {
		return elements[0];
	}

	/** Return y-axis component
	  */
	float& y() {
		return elements[1];
	}

	/** Return z-axis component
	  */
	float& z() {
		return elements[2];
	}

	/** Compute the cross product of two vectors
	  */
	Vector3 cross(const Vector& rhs) const;
};

class Vector4 : public Vector {
public:
	/** Empty 2-dimensional Vector
	  */
	Vector4() : Vector(4) { }

	/** Filled vector constructor
	  */
	Vector4(const float& value) :
		Vector(4, value)
	{
	}

	/** Explicit element constructor
	  */
	Vector4(const float* values) :
		Vector(4, values)
	{
	}

	/** Explicit Vector element constructor
	  */
	Vector4(const float& a0, const float& a1, const float& a2, const float& a3) :
		Vector(4)
	{
		elements[0] = a0;
		elements[1] = a1;
		elements[2] = a2;
		elements[3] = a3;
	}

	/** Stream insertion operator
	  */
	friend std::ostream& operator << (std::ostream& out, const Vector4& vec) {
		return vec.__stream(out);
	}

	/** Equality operator
	  */
	bool operator == (const Vector4& rhs) const {
		return __equal(&rhs);
	}

	/** Inequality operator
	  */
	bool operator != (const Vector4& rhs) const {
		return (!__equal(&rhs));
	}

	/** Multiply this vector by another vector and return the resulting dot product
	  */
	float operator * (const Vector4& rhs) const {
		return __dot(&rhs);
	}

	/** Multiply this Vector by a constant and return the resulting Vector
	  */
	Vector4 operator * (const float& rhs) const {
		Vector4 retval(dimension);
		__multiply(rhs, &retval);
		return retval;
	}

	/** Divide this Vector by a constant and return the result
	  */
	Vector4 operator / (const float& rhs) const {
		Vector4 retval(dimension);
		__divide(rhs, &retval);
		return retval;
	}

	/** Add a Vector to this one and return the result
	  */
	Vector4 operator + (const Vector4& rhs) const {
		Vector4 retval(dimension);
		__add(&rhs, &retval);
		return retval;
	}

	/** Subtract a Vector from this one and return the result
	  */
	Vector4 operator - (const Vector4& rhs) const {
		Vector4 retval(dimension);
		__subtract(&rhs, &retval);
		return retval;
	}

	/** Unary operator
	  */
	Vector4 operator - () const {
		Vector4 retval;
		__negate(this, &retval);
		return retval;
	}

	/** Assignment operator
	  */
	Vector4& operator = (const Vector4& rhs) {
		__assign(&rhs);
		return (*this);
	}

	/** Multiply this Vector by a constant and return the result
	  */
	Vector4& operator *= (const float& rhs) {
		__multiply(rhs);
		return (*this);
	}

	/** Divide this Vector by a constant and return the result
	  */
	Vector4& operator /= (const float& rhs) {
		__divide(rhs);
		return (*this);
	}

	/** Add a Vector to this one and return the result
	  */
	Vector4& operator += (const Vector4& rhs) {
		__add(&rhs);
		return (*this);
	}

	/** Subtract a Vector from this one and return the result
	  */
	Vector4& operator -= (const Vector4& rhs) {
		__subtract(&rhs);
		return (*this);
	}

	/** Normalize this vector in place
	  */
	Vector4& normInPlace() {
		__normalize(this);
		return (*this);
	}

	/** Normalize this and store into a resultant vector
	  */
	Vector4 normalize() const {
		Vector4 retval(dimension);
		__normalize(this, &retval);
		return retval;
	}

	/** Compute the distance between this vector and an input vector
	  * @return The distance or -1 if the input vector has the incorrect dimension
	  */
	float distance(const Vector4& rhs) const {
		return __distance(&rhs);
	}

	/** Compute the dot product between this vector and an input vector
	  * @return The dot product or -1 if the input vector has the incorrect dimension
	  */
	float dot(const Vector4& rhs) const {
		return __dot(&rhs);
	}

	/** Compute the angle between this vector and an input vector
	  * @return The angle (in radians) or -1 if the input vector has the incorrect dimension
	  */
	float angle(const Vector4& rhs) const {
		return __angle(&rhs);
	}

	/** Compute the cosine of the angle between this vector and an input vector
	  * @return The cosine or -1 if the input vector has the incorrect dimension
	  */
	float cosTheta(const Vector4& rhs) const {
		return __cosTheta(&rhs);
	}

	/** Return x-axis component
	  */
	float& x() {
		return elements[0];
	}

	/** Return y-axis component
	  */
	float& y() {
		return elements[1];
	}

	/** Return z-axis component
	  */
	float& z() {
		return elements[2];
	}

	/** Return w-axis component
	  */
	float& w() {
		return elements[3];
	}
};

extern const Vector3 zeroVector; ///< Three dimensional zero-vector (0, 0, 0)

extern const Vector3 unitVectorX; ///< X-axis unit vector (1, 0, 0)

extern const Vector3 unitVectorY; ///< Y-axis unit vector (0, 1, 0)

extern const Vector3 unitVectorZ; ///< Z-axis unit vector (0, 0, 1)

#endif
