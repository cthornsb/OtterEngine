#ifndef MATRIX2_HPP
#define MATRIX2_HPP

#include <string>

#include "vector2.hpp"

class matrix2{
public:
	float elements[2][2]; ///< All elements of the matrix stored using [col][row]

	/** Default constructor
	  */
	matrix2();

	/** Rotation matrix constructor
	  */
	matrix2(const vector2 &vec);

	/** Rotation matrix constructor
	  */
	matrix2(const float &theta);

	/** Explicit matrix element constructor
	  */
	matrix2(const float &a00, const float &a10,
	        const float &a01, const float &a11);

	/** Copy constructor
	  */
	matrix2(const matrix2 &other);

	/** Assignment operator
	  */
	matrix2& operator = (const matrix2 &rhs);

	/** Multiply this matrix by another matrix and return the resulting matrix
	  */
	matrix2 operator * (const matrix2 &rhs) const ;

	/** Multiply this matrix by a constant and return the resulting matrix
	  */
	matrix2 operator * (const float &rhs) const ;

	/** Multiply this matrix by a vector and return the resulting vector
	  */
	vector2 operator * (const vector2 &rhs) const ;

	/** Divide this matrix by a constant and return the result
	  */
	matrix2 operator / (const float &rhs) const ;

	/** Add a matrix to this one and return the result
	  */
	matrix2 operator + (const matrix2 &rhs) const ;

	/** Subtract a matrix from this one and return the result
	  */
	matrix2 operator - (const matrix2 &rhs) const ;

	/** Multiply this matrix by another matrix and return the result
	  */
	matrix2& operator *= (const matrix2 &rhs);

	/** Multiply this matrix by a constant and return the result
	  */	
	matrix2& operator *= (const float &rhs); 

	/** Divide this matrix by a constant and return the result
	  */
	matrix2& operator /= (const float &rhs);

	/** Add a matrix to this one and return the result
	  */
	matrix2& operator += (const matrix2 &rhs);

	/** Subtract a matrix from this one and return the result
	  */
	matrix2& operator -= (const matrix2 &rhs);

	/** Get one row from the matrix
	  */
	void getRow(const size_t &row, vector2 &vec) const ;

	/** Get one row from the matrix
	  */
	vector2 getRow(const size_t &row) const ;

	/** Set one row in the matrix using a vector
	  */
	void setRow(const size_t &row, const vector2 &vec);

	/** Set one row in the matrix explicitly
	  */
	void setRow(const size_t &row, const float &p1, const float &p2);

	/** Set this to a rotation matrix using a vector whose three coordinates are equal to theta, phi and psi respectively (all in radians)
	  */
	void setRotation(const vector2 &vec);

	/** Set this to a rotation matrix using theta, phi and psi (all in radians)
	  */
	void setRotation(const float &theta);

	/** Operate on an input vector by multiplying it with this matrix
	  */
	void transformInPlace(vector2 &vector) const ;

	/** Operate on an input vector by multiplying it with this matrix
	  */
	vector2 transform(const vector2& vector) const;

	/** Operate on an input vector by multiplying it with the transpose of this matrix
	  */
	void transpose(vector2 &vector) const ;

	/** Dump all matrix elements into a returned string
	  */
	std::string dump() const ;

	/** Zero all elements of this matrix
	  */	
	void zero();

	/** Set this matrix to an identity matrix (i.e. diagonal elements are equal to 1 and off-diagonal elements are equal to zero)
	  */
	void identity();
};

extern const matrix2 identityMatrix2;

#endif
