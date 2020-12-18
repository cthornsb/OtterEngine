#ifndef MATRIX3_HPP
#define MATRIX3_HPP

#include <string>

#include "vector3.hpp"

class matrix3{
public:
	float elements[3][3]; ///< All elements of the matrix stored using [col][row]

	/** Default constructor
	  */
	matrix3();

	/** Rotation matrix constructor
	  */
	matrix3(const vector3 &vec);

	/** Rotation matrix constructor
	  */
	matrix3(const float &theta, const float &phi, const float &psi);

	/** Explicit matrix element constructor
	  */
	matrix3(const float &a00, const float &a10, const float &a20,
	        const float &a01, const float &a11, const float &a21,
	        const float &a02, const float &a12, const float &a22);

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
	matrix3 operator * (const float &rhs) const ;

	/** Multiply this matrix by a vector and return the resulting vector
	  */
	vector3 operator * (const vector3 &rhs) const ;

	/** Divide this matrix by a constant and return the result
	  */
	matrix3 operator / (const float &rhs) const ;

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
	matrix3& operator *= (const float &rhs); 

	/** Divide this matrix by a constant and return the result
	  */
	matrix3& operator /= (const float &rhs);

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
	void setRow(const size_t &row, const float &p1, const float &p2, const float &p3);

	/** Set this to a rotation matrix using a vector whose three coordinates are equal to theta, phi and psi respectively (all in radians)
	  */
	void setRotation(const vector3 &vec);

	/** Set this to a rotation matrix using theta, phi and psi (all in radians)
	  */
	void setRotation(const float &theta, const float &phi, const float &psi);

	static matrix3 getPitchMatrix(const float &angle);
	
	static matrix3 getRollMatrix(const float &angle);
	
	static matrix3 getYawMatrix(const float &angle);

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

extern const matrix3 identityMatrix;

#endif
