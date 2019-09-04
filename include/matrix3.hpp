#ifndef MATRIX3_HPP
#define MATRIX3_HPP

#include <string>

#include "vector3.hpp"

class matrix3{
public:
	double elements[3][3]; ///< All elements of the matrix stored using [col][row]

	/** Default constructor
	  */
	matrix3();

	/** Rotation matrix constructor
	  */
	matrix3(const vector3 &vec);

	/** Rotation matrix constructor
	  */
	matrix3(const double &theta, const double &phi, const double &psi);

	/** Explicit matrix element constructor
	  */
	matrix3(const double &a00, const double &a10, const double &a20,
	        const double &a01, const double &a11, const double &a21,
	        const double &a02, const double &a12, const double &a22);

	/** Copy constructor
	  */
	matrix3(const matrix3 &other);

	/** Assignment operator
	  */
	matrix3& operator = (const matrix3 &rhs);

	/** Multiply this matrix by another matrix and return the resulting matrix
	  */
	matrix3 operator * (const matrix3 &rhs) const ;

	/** Multiply this matrix by a constant and return the resulting matrix
	  */
	matrix3 operator * (const double &rhs) const ;

	/** Multiply this matrix by a vector and return the resulting vector
	  */
	vector3 operator * (const vector3 &rhs) const ;

	/** Divide this matrix by a constant and return the result
	  */
	matrix3 operator / (const double &rhs) const ;

	/** Add a matrix to this one and return the result
	  */
	matrix3 operator + (const matrix3 &rhs) const ;

	/** Subtract a matrix from this one and return the result
	  */
	matrix3 operator - (const matrix3 &rhs) const ;

	/** Multiply this matrix by another matrix and return the result
	  */
	matrix3& operator *= (const matrix3 &rhs);

	/** Multiply this matrix by a constant and return the result
	  */	
	matrix3& operator *= (const double &rhs); 

	/** Divide this matrix by a constant and return the result
	  */
	matrix3& operator /= (const double &rhs);

	/** Add a matrix to this one and return the result
	  */
	matrix3& operator += (const matrix3 &rhs);

	/** Subtract a matrix from this one and return the result
	  */
	matrix3& operator -= (const matrix3 &rhs);

	/** Get one row from the matrix
	  */
	void getRow(const size_t &row, vector3 &vec) const ;

	/** Get one row from the matrix
	  */
	vector3 getRow(const size_t &row) const ;

	/** Set one row in the matrix using a vector
	  */
	void setRow(const size_t &row, const vector3 &vec);

	/** Set one row in the matrix explicitly
	  */
	void setRow(const size_t &row, const double &p1, const double &p2, const double &p3);

	/** Set this to a rotation matrix using a vector whose three coordinates are equal to theta, phi and psi respectively (all in radians)
	  */
	void setRotation(const vector3 &vec);

	/** Set this to a rotation matrix using theta, phi and psi (all in radians)
	  */
	void setRotation(const double &theta, const double &phi, const double &psi);

	/** Operate on an input vector by multiplying it with this matrix
	  */
	void transform(vector3 &vec) const ;

	/** Operate on an input vector by multiplying it with the transpose of this matrix
	  */
	void transpose(vector3 &vec) const ;

	/** Dump all matrix elements into a returned string
	  */
	std::string dump();

	/** Zero all elements of this matrix
	  */	
	void zero();

	/** Set this matrix to an identity matrix (i.e. diagonal elements are equal to 1 and off-diagonal elements are equal to zero)
	  */
	void identity();
};

#endif
