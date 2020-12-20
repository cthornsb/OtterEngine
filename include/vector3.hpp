#ifndef VECTOR3_HPP
#define VECTOR3_HPP

class vector3{
public:
	float x; ///< X coordinate
	float y; ///< Y coordinate
	float z; ///< Z coordinate

	/** Default constructor (zero vector)
	  */
	vector3() : x(0), y(0), z(0) { }
	
	/** 2d vector constructor
	  */
	vector3(const float &x_, const float &y_) : x(x_), y(y_), z(0) { }
	
	/** 3d vector constructor
	  */
	vector3(const float &x_, const float &y_, const float &z_) : x(x_), y(y_), z(z_) { }

	/** Copy constructor
	  */
	vector3(const vector3 &other) : x(other.x), y(other.y), z(other.z) { }

	/** Element access operator
	  * @param index The coordinate index (0=x, 1=y, 2=z)
	  * @return The specified coordinate (0, 1, or 2) or NAN if the index is outside the valid range
	  */
	float operator [] (const unsigned int& index) const;

	/** Assignment operator
	  */
	vector3& operator = (const vector3 &rhs);
	
	/** Perform the dot product between this vector and another vector and return the result
	  */
	float operator * (const vector3 &rhs) const { return (rhs.x*x + rhs.y*y + rhs.z*z); }
	
	/** Multiply this vector by a constant value and return the result
	  */
	vector3 operator * (const float &rhs) const { return vector3(x*rhs, y*rhs, z*rhs); }

	/** Divide this vector by a constant value and return the result
	  */
	vector3 operator / (const float &rhs) const { return vector3(x/rhs, y/rhs, z/rhs); }

	/** Add another vector to this vector and return the result
	  */
	vector3 operator + (const vector3 &rhs) const { return vector3(x+rhs.x, y+rhs.y, z+rhs.z); }

	/** Subtract another vector from this vector and return the result
	  */
	vector3 operator - (const vector3 &rhs) const { return vector3(x-rhs.x, y-rhs.y, z-rhs.z); }

	/** Equality operator
	  */
	bool operator == (const vector3 &rhs) const { return (x==rhs.x && y==rhs.y && z==rhs.z); }

	/** Return true if the length of this vector is equal to a value and return false otherwise
	  */
	bool operator == (const float &rhs) const { return (length() == rhs); }

	/** Inequality operator
	  */
	bool operator != (const vector3 &rhs) const { return (x!=rhs.x || y!=rhs.y || z!=rhs.z); }

	/** Return true if the length of this vector is not equal to a value and return false otherwise
	  */
	bool operator != (const float &rhs) const { return (length() != rhs); }

	/** Return true if the length of this vector is greater than that of another vector and return false otherwise
	  */
	bool operator > (const vector3 &rhs) const { return (length() > rhs.length()); }

	/** Return true if the length of this vector is greater than a value and return false otherwise
	  */	
	bool operator > (const float &rhs) const { return (length() > rhs); }
	
	/** Return true if the length of this vector is less than that of another vector and return false otherwise
	  */
	bool operator < (const vector3 &rhs) const { return (length() < rhs.length()); }

	/** Return true if the length of this vector is less than a value and return false otherwise
	  */	
	bool operator < (const float &rhs) const { return (length() < rhs); }

	/** Return true if the length of this vector is greater than or equal to that of another vector and return false otherwise
	  */
	bool operator >= (const vector3 &rhs) const { return (length() >= rhs.length()); }

	/** Return true if the length of this vector is greater than or equal to a value and return false otherwise
	  */	
	bool operator >= (const float &rhs) const { return (length() >= rhs); }

	/** Return true if the length of this vector is less than or equal to that of another vector and return false otherwise
	  */
	bool operator <= (const vector3 &rhs) const { return (length() <= rhs.length()); }

	/** Return true if the length of this vector is less than or equal to a value and return false otherwise
	  */	
	bool operator <= (const float &rhs) const { return (length() <= rhs); }

	/** Multiply this vector by a constant value and return the result
	  */
	vector3& operator *= (const float &rhs);
	
	/** Divide this vector by a constant value and return the result
	  */
	vector3& operator /= (const float &rhs);
	
	/** Add another vector to this vector and return the result
	  */
	vector3& operator += (const vector3 &rhs);

	/** Subtract another vector from this vector and return the result
	  */
	vector3& operator -= (const vector3 &rhs);

	/** Perform the cross-product of this and another vector and return the result
	  */
	vector3 cross(const vector3 &rhs) const ;

	/** Get a normalized version of this vector
	  * @note Does not modify the coordinates of this vector
	  */
	vector3 normalize() const ;

	/** Normalize this vector and return the result
	  */
	vector3& normInPlace();

	/** Return true if this vector has unit length and return false otherwise
	  */
	bool isUnit() const { return (square() == 1); }

	/** Return true if this is a zero vector and return false otherwise
	  */
	bool isZero() const { return (x==0 && y==0 && z==0); }

	/** Compute the length of this vector
	  */
	float length() const ;
	
	/** Compute the square of the length of this vector
	  */
	float square() const { return (x*x+y*y+z*z); }
	
	/** Compute the distance between this and another point in 3d space
	  */
	float distance(const vector3& other) const ;

	/** Compute the angle between this and another vector (in radians)
	  */
	float angle(const vector3& other) const;

	/** Compute the cosine of the angle between this and another vector (unitless)
	  */
	float cosTheta(const vector3& other) const;

	/** Flip the vector to point in the opposite direction
	  */
	void invert();

	/** Zero all elements of this vector
	  */
	void zero();
	
	/** Dump vector coordinates to stdout
	  */
	void dump() const ;
};

extern const vector3 zeroVector;
extern const vector3 unitVectorX;
extern const vector3 unitVectorY;
extern const vector3 unitVectorZ;

#endif
