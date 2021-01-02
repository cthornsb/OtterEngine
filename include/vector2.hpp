#ifndef vector2_HPP
#define vector2_HPP

class vector2{
public:
	float x; ///< X coordinate
	float y; ///< Y coordinate

	/** Default constructor (zero vector)
	  */
	vector2() : x(0), y(0) { }
	
	/** 2d vector constructor
	  */
	vector2(const float &x_, const float &y_) : x(x_), y(y_) { }
	
	/** Copy constructor
	  */
	vector2(const vector2 &other) : x(other.x), y(other.y) { }

	/** Element access operator
	  * @param index The coordinate index (0=x or 1=y)
	  * @return The specified coordinate (0 or 1) or NAN if the index is outside the valid range
	  */
	float operator [] (const unsigned int& index) const;

	/** Assignment operator
	  */
	vector2& operator = (const vector2 &rhs);
	
	/** Perform the dot product between this vector and another vector and return the result
	  */
	float operator * (const vector2 &rhs) const { return (rhs.x*x + rhs.y*y); }
	
	/** Multiply this vector by a constant value and return the result
	  */
	vector2 operator * (const float &rhs) const { return vector2(x*rhs, y*rhs); }

	/** Divide this vector by a constant value and return the result
	  */
	vector2 operator / (const float &rhs) const { return vector2(x/rhs, y/rhs); }

	/** Add another vector to this vector and return the result
	  */
	vector2 operator + (const vector2 &rhs) const { return vector2(x+rhs.x, y+rhs.y); }

	/** Subtract another vector from this vector and return the result
	  */
	vector2 operator - (const vector2 &rhs) const { return vector2(x-rhs.x, y-rhs.y); }

	/** Unary plus operator
	  */
	vector2 operator + () const { return vector2(*this); }

	/** Unary minus operator
	  */
	vector2 operator - () const { return vector2(-x, -y); }

	/** Equality operator
	  */
	bool operator == (const vector2 &rhs) const { return (x==rhs.x && y==rhs.y); }

	/** Return true if the length of this vector is equal to a value and return false otherwise
	  */
	bool operator == (const float &rhs) const { return (length() == rhs); }

	/** Inequality operator
	  */
	bool operator != (const vector2 &rhs) const { return (x!=rhs.x || y!=rhs.y); }

	/** Return true if the length of this vector is not equal to a value and return false otherwise
	  */
	bool operator != (const float &rhs) const { return (length() != rhs); }

	/** Return true if the length of this vector is greater than that of another vector and return false otherwise
	  */
	bool operator > (const vector2 &rhs) const { return (length() > rhs.length()); }

	/** Return true if the length of this vector is greater than a value and return false otherwise
	  */	
	bool operator > (const float &rhs) const { return (length() > rhs); }
	
	/** Return true if the length of this vector is less than that of another vector and return false otherwise
	  */
	bool operator < (const vector2 &rhs) const { return (length() < rhs.length()); }

	/** Return true if the length of this vector is less than a value and return false otherwise
	  */	
	bool operator < (const float &rhs) const { return (length() < rhs); }

	/** Return true if the length of this vector is greater than or equal to that of another vector and return false otherwise
	  */
	bool operator >= (const vector2 &rhs) const { return (length() >= rhs.length()); }

	/** Return true if the length of this vector is greater than or equal to a value and return false otherwise
	  */	
	bool operator >= (const float &rhs) const { return (length() >= rhs); }

	/** Return true if the length of this vector is less than or equal to that of another vector and return false otherwise
	  */
	bool operator <= (const vector2 &rhs) const { return (length() <= rhs.length()); }

	/** Return true if the length of this vector is less than or equal to a value and return false otherwise
	  */	
	bool operator <= (const float &rhs) const { return (length() <= rhs); }

	/** Multiply this vector by a constant value and return the result
	  */
	vector2& operator *= (const float &rhs);
	
	/** Divide this vector by a constant value and return the result
	  */
	vector2& operator /= (const float &rhs);
	
	/** Add another vector to this vector and return the result
	  */
	vector2& operator += (const vector2 &rhs);

	/** Subtract another vector from this vector and return the result
	  */
	vector2& operator -= (const vector2 &rhs);

	/** Get a normalized version of this vector
	  * @note Does not modify the coordinates of this vector
	  */
	vector2 normalize() const ;

	/** Normalize this vector and return the result
	  */
	vector2& normInPlace();

	/** Return true if this vector has unit length and return false otherwise
	  */
	bool isUnit() const { return (square() == 1); }

	/** Return true if this is a zero vector and return false otherwise
	  */
	bool isZero() const { return (x==0 && y==0); }

	/** Compute the length of this vector
	  */
	float length() const ;
	
	/** Compute the square of the length of this vector
	  */
	float square() const { return (x*x+y*y); }
	
	/** Compute the distance between this and another point in 3d space
	  */
	float distance(const vector2& other) const ;

	/** Compute the angle between this and another vector (in radians)
	  */
	float angle(const vector2& other) const;

	/** Compute the cosine of the angle between this and another vector (unitless)
	  */
	float cosTheta(const vector2& other) const;

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

#endif
