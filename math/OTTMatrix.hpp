#ifndef MATH_OTT_MATRIX_HPP
#define MATH_OTT_MATRIX_HPP

#include "OTTVector.hpp"

#include <cstdint>
#include <vector>

namespace ott {

/// <summary>
/// General square matrix class
/// Elements are in row-major format in memory i.e. {m00, m01, ... , mNN} for the N+1
/// dimensional matrix with rows {m00, m01, ... m0N}, {m10, m11, ... m1N}, ... , {mN0, mN1, ... mNN}
/// </summary>
class Matrix {
public:
	/** Default constructor (null)
	  */
	Matrix() :
		elements(),
		dimension(0),
		length(0)
	{
	}

	/** Empty matrix constructor
	  */
	Matrix(const uint16_t& size) :
		elements(size * size, 0.f),
		dimension(size),
		length(size * size)
	{
	}

	/** Identity matrix constructor
	  */
	Matrix(const uint16_t& size, const float& value) :
		Matrix(size)
	{
		for (uint16_t i = 0; i < length; i += (dimension + 1)) {
				elements[i] = value;
		}
	}

	/** Explicit element constructor
	  */
	Matrix(const uint16_t& size, const float* values) :
		Matrix(size)
	{
		for (uint16_t i = 0; i < size; i++) { // over rows
			for (uint16_t j = 0; j < size; j++) { // over columns
				elements[dimension * i + j] = values[dimension * i + j];
			}
		}
	}

	/** Copy constructor
	  */
	Matrix(const Matrix& other) :
		elements(other.elements),
		dimension(other.dimension),
		length(other.length)
	{
	}

	/** Matrix element access
	  * Does not check array index bounds
	  */
	float& operator [] (const uint16_t& index) {
		return elements[index];
	}

	/** Const matrix element access
	  * Does not check array index bounds
	  */
	float operator [] (const uint16_t& index) const {
		return elements[index];
	}

	/** Matrix element access
	  * Uses std::vector<float>::at() for index bound check
	  */
	float& At(const uint16_t& index) {
		return elements.at(index);
	}

	/** Const matrix element access
	  * Uses std::vector<float>::at() for index bound check
	  */
	float At(const uint16_t& index) const {
		return elements.at(index);
	}

	/** Matrix element access
	  * Uses std::vector<float>::at() for index bound check
	  */
	float& At(const uint16_t& row, const uint16_t& col) {
		return elements.at(dimension * row + col);
	}

	/** Const matrix element access
	  * Uses std::vector<float>::at() for index bound check
	  */
	float At(const uint16_t& row, const uint16_t& col) const {
		return elements.at(dimension * row + col);
	}

	/** Get pointer to the 0th matrix element
	  */
	float* Data() {
		return elements.data();
	}

	/** Get const pointer to the 0th matrix element
	  */
	const float* ConstData() const {
		return (const float*)elements.data();
	}

	/** Get the dimension of the matrix
	  */
	uint16_t Dimension() const {
		return dimension;
	}

	/** Get the number of elements in the matrix, equal to the dimension squared
	  */
	uint16_t Length() const { 
		return length; 
	}

	/** Copy a matrix column into a vector (zero-indexed)
	  * Vector must have the same dimension as the matrix
	  */
	void Column(const uint16_t& col, Vector* vec) const;

	/** Copy a matrix row into a vector (zero-indexed)
	  * Vector must have the same dimension as the matrix
	  */
	void Row(const uint16_t& row, Vector* vec) const;

	/** Multiply this matrix by another matrix and return the resulting matrix
	  */
	Matrix operator * (const Matrix& rhs) const {
		Matrix retval(rhs.dimension);
		__multiply(&rhs, &retval);
		return retval;
	}

	/** Multiply this matrix by a constant and return the resulting matrix
	  */
	Matrix operator * (const float& rhs) const {
		Matrix retval(dimension);
		__multiply(rhs, &retval);
		return retval;
	}

	/** Multiply this matrix by a vector and return the resulting vector
	  */
	Vector operator * (const Vector& rhs) const {
		Vector retval(rhs.dimension);
		__transform(&retval);
		return retval;
	}

	/** Divide this matrix by a constant and return the result
	  */
	Matrix operator / (const float& rhs) const {
		Matrix retval(dimension);
		__divide(rhs, &retval);
		return retval;
	}

	/** Add a matrix to this one and return the result
	  */
	Matrix operator + (const Matrix& rhs) const {
		Matrix retval(dimension);
		__add(&rhs, &retval);
		return retval;
	}

	/** Subtract a matrix from this one and return the result
	  */
	Matrix operator - (const Matrix& rhs) const {
		Matrix retval(dimension);
		__subtract(&rhs, &retval);
		return retval;
	}

	/** Assignment operator
	  */
	Matrix& operator = (const Matrix& rhs) {
		__assign(&rhs);
		return (*this);
	}

	/** Multiply this matrix by another matrix and return the result
	  */
	Matrix& operator *= (const Matrix& rhs) {
		__multiply(&rhs);
		return (*this);
	}

	/** Multiply this matrix by a constant and return the result
	  */
	Matrix& operator *= (const float& rhs) {
		__multiply(rhs);
		return (*this);
	}

	/** Divide this matrix by a constant and return the result
	  */
	Matrix& operator /= (const float& rhs) {
		__divide(rhs);
		return (*this);
	}

	/** Add a matrix to this one and return the result
	  */
	Matrix& operator += (const Matrix& rhs) {
		__add(&rhs);
		return (*this);
	}

	/** Subtract a matrix from this one and return the result
	  */
	Matrix& operator -= (const Matrix& rhs) {
		__subtract(&rhs);
		return (*this);
	}

	/** Operate on an input vector by multiplying it with this matrix
	  */
	Vector& TransformInPlace(Vector& vec) const {
		__transform(&vec);
		return vec;
	}

	/** Operate on an input vector by multiplying it with this matrix
	  */
	Vector Transform(const Vector& vec) const {
		Vector retval(vec.dimension);
		__transform(&vec, &retval);
		return retval;
	}

	/** Operate on an input vector by multiplying it with the transpose of this matrix
	  */
	Vector& TransposeInPlace(Vector& vec) const {
		__transpose(&vec);
		return vec;
	}

	/** Operate on an input vector by multiplying it with the transpose of this matrix
	  */
	Vector Transpose(const Vector& vec) const {
		Vector retval(vec.dimension);
		__transpose(&vec, &retval);
		return retval;
	}

	/** Zero all elements of this matrix
	  */
	void Zero();

	/** Set this matrix to an identity matrix (i.e. diagonal elements are equal to 1 and off-diagonal elements are equal to zero)
	  */
	void Identity();

protected:
	std::vector<float> elements; ///< Matrix elements

	uint16_t dimension; ///< Dimension of matrix

	uint16_t length; ///< Length of elements vector

	/** Assignment operator
	  */
	void __assign(const Matrix* rhs);

	/** Multiply this matrix by another matrix and return the result
	  */
	void __multiply(const Matrix* rhs);

	/** Multiply this matrix by another matrix and return the result
	  */
	void __multiply(const Matrix* in, Matrix* out) const;

	/** Multiply this matrix by a constant and return the result
	  */
	void __multiply(const float& rhs);

	/** Multiply this matrix by a constant and return the result
	  */
	void __multiply(const float& in, Matrix* out) const;

	/** Divide this matrix by a constant and return the result
	  */
	void __divide(const float& rhs);

	/** Divide this matrix by a constant and return the result
	  */
	void __divide(const float& in, Matrix* out) const;

	/** Add a matrix to this one and return the result
	  */
	void __add(const Matrix* rhs);

	/** Add a matrix to this one and return the result
	  */
	void __add(const Matrix* in, Matrix* out) const;

	/** Subtract a matrix from this one and return the result
	  */
	void __subtract(const Matrix* rhs);

	/** Add a matrix to this one and return the result
	  */
	void __subtract(const Matrix* in, Matrix* out) const;

	/** Operate on an input vector by multiplying it with this matrix
	  */
	void __transform(Vector* vec) const;

	/** Operate on an input vector by multiplying it with this matrix
	  */
	void __transform(const Vector* in, Vector* out) const;

	/** Operate on an input vector by multiplying it with the transpose of this matrix
	  */
	void __transpose(Vector* vec) const;

	/** Operate on an input vector by multiplying it with the transpose of this matrix
	  */
	void __transpose(const Vector* in, Vector* out) const;
};

/// <summary>
/// Two dimensional matrix
/// Elements are in row-major format in memory i.e. {m00, m01, ... , m11} for the matrix
/// with rows {m00, m01}, {m10, m11}
/// </summary>
class Matrix2 : public Matrix {
public:
	/** Empty 2-dimensional matrix
	  */
	Matrix2() : Matrix(2) { }

	/** Identity matrix constructor
	  */
	Matrix2(const float& value) :
		Matrix(2, value)
	{
	}

	/** Explicit element constructor
	  */
	Matrix2(const float* values) :
		Matrix(2, values)
	{
	}

	/** Rotation matrix constructor
	  */
	Matrix2(const Vector2& row0, const Vector2& row1) :
		Matrix(2)
	{
		elements[0] = row0[0]; elements[1] = row0[1];
		elements[2] = row1[0]; elements[3] = row1[1];
	}

	/** Explicit matrix element constructor
	  */
	Matrix2(const float& a00, const float& a01,
		const float& a10, const float& a11) :
		Matrix(2)
	{
		elements[0] = a00; elements[1] = a01;
		elements[2] = a10; elements[3] = a11;
	}

	/** Multiply this matrix by another matrix and return the resulting matrix
	  */
	Matrix2 operator * (const Matrix2& rhs) const {
		Matrix2 retval(rhs.dimension);
		__multiply(&rhs, &retval);
		return retval;
	}

	/** Multiply this matrix by a constant and return the resulting matrix
	  */
	Matrix2 operator * (const float& rhs) const {
		Matrix2 retval(dimension);
		__multiply(rhs, &retval);
		return retval;
	}

	/** Multiply this matrix by a vector and return the resulting vector
	  */
	Vector2 operator * (const Vector2& rhs) const {
		Vector2 retval(rhs.dimension);
		__transform(&retval);
		return retval;
	}

	/** Divide this matrix by a constant and return the result
	  */
	Matrix2 operator / (const float& rhs) const {
		Matrix2 retval(dimension);
		__divide(rhs, &retval);
		return retval;
	}

	/** Add a matrix to this one and return the result
	  */
	Matrix2 operator + (const Matrix2& rhs) const {
		Matrix2 retval(dimension);
		__add(&rhs, &retval);
		return retval;
	}

	/** Subtract a matrix from this one and return the result
	  */
	Matrix2 operator - (const Matrix2& rhs) const {
		Matrix2 retval(dimension);
		__subtract(&rhs, &retval);
		return retval;
	}

	/** Assignment operator
	  */
	Matrix2& operator = (const Matrix2& rhs) {
		__assign(&rhs);
		return (*this);
	}

	/** Multiply this matrix by another matrix and return the result
	  */
	Matrix2& operator *= (const Matrix2& rhs) {
		__multiply(&rhs);
		return (*this);
	}

	/** Multiply this matrix by a constant and return the result
	  */
	Matrix2& operator *= (const float& rhs) {
		__multiply(rhs);
		return (*this);
	}

	/** Divide this matrix by a constant and return the result
	  */
	Matrix2& operator /= (const float& rhs) {
		__divide(rhs);
		return (*this);
	}

	/** Add a matrix to this one and return the result
	  */
	Matrix2& operator += (const Matrix2& rhs) {
		__add(&rhs);
		return (*this);
	}

	/** Subtract a matrix from this one and return the result
	  */
	Matrix2& operator -= (const Matrix2& rhs) {
		__subtract(&rhs);
		return (*this);
	}

	/** Operate on an input vector by multiplying it with this matrix
	  */
	Vector2& TransformInPlace(Vector2& vec) const {
		__transform(&vec);
		return vec;
	}

	/** Operate on an input vector by multiplying it with this matrix
	  */
	Vector2 Transform(const Vector2& vec) const {
		Vector2 retval(vec.dimension);
		__transform(&vec, &retval);
		return retval;
	}

	/** Operate on an input vector by multiplying it with the transpose of this matrix
	  */
	Vector2& TransposeInPlace(Vector2& vec) const {
		__transpose(&vec);
		return vec;
	}

	/** Operate on an input vector by multiplying it with the transpose of this matrix
	  */
	Vector2 Transpose(const Vector2& vec) const {
		Vector2 retval(vec.dimension);
		__transpose(&vec, &retval);
		return retval;
	}

	/** Set the rotation matrix for a given angle (in radians)
	  */
	void SetRotation(const float& theta);

	/** Get the rotation matrix for a given angle (in radians)
	  */
	static Matrix2 RotationMatrix(const float& theta);
};

/// <summary>
/// Three dimensional matrix
/// Elements are in row-major format in memory i.e. {m00, m01, ... , m22} for the matrix
/// with rows {m00, m01, m02}, ... , {m20, m21, m22}
/// </summary>
class Matrix3 : public Matrix {
public:
	/** Empty 2-dimensional matrix
	  */
	Matrix3() : Matrix(3) { }

	/** Identity matrix constructor
	  */
	Matrix3(const float& value) :
		Matrix(3, value)
	{
	}

	/** Explicit element constructor
	  */
	Matrix3(const float* values) :
		Matrix(3, values)
	{
	}

	/** Vector constructor
	  */
	Matrix3(const Vector3& row0, const Vector3& row1, const Vector3& row2) :
		Matrix(3)
	{
		elements[0] = row0[0]; elements[1] = row0[1]; elements[2] = row0[2];
		elements[3] = row1[0]; elements[4] = row1[1]; elements[5] = row1[2];
		elements[6] = row2[0]; elements[7] = row2[1]; elements[8] = row2[2];
	}

	/** Rotation matrix constructor
	  */
	Matrix3(const float& theta, const float& phi, const float& psi) :
		Matrix(3)
	{
		this->SetRotation(theta, phi, psi);
	}

	/** Explicit matrix element constructor
	  */
	Matrix3(const float& a00, const float& a01, const float& a02,
		const float& a10, const float& a11, const float& a12,
		const float& a20, const float& a21, const float& a22) :
		Matrix(3)
	{
		elements[0] = a00; elements[1] = a01; elements[2] = a02;
		elements[3] = a10; elements[4] = a11; elements[5] = a12;
		elements[6] = a20; elements[7] = a21; elements[8] = a22;
	}

	/** Multiply this matrix by another matrix and return the resulting matrix
	  */
	Matrix3 operator * (const Matrix3& rhs) const {
		Matrix3 retval(rhs.dimension);
		__multiply(&rhs, &retval);
		return retval;
	}

	/** Multiply this matrix by a constant and return the resulting matrix
	  */
	Matrix3 operator * (const float& rhs) const {
		Matrix3 retval(dimension);
		__multiply(rhs, &retval);
		return retval;
	}

	/** Multiply this matrix by a vector and return the resulting vector
	  */
	Vector3 operator * (const Vector3& rhs) const {
		Vector3 retval(rhs.dimension);
		__transform(&retval);
		return retval;
	}

	/** Divide this matrix by a constant and return the result
	  */
	Matrix3 operator / (const float& rhs) const {
		Matrix3 retval(dimension);
		__divide(rhs, &retval);
		return retval;
	}

	/** Add a matrix to this one and return the result
	  */
	Matrix3 operator + (const Matrix3& rhs) const {
		Matrix3 retval(dimension);
		__add(&rhs, &retval);
		return retval;
	}

	/** Subtract a matrix from this one and return the result
	  */
	Matrix3 operator - (const Matrix3& rhs) const {
		Matrix3 retval(dimension);
		__subtract(&rhs, &retval);
		return retval;
	}

	/** Assignment operator
	  */
	Matrix3& operator = (const Matrix3& rhs) {
		__assign(&rhs);
		return (*this);
	}

	/** Multiply this matrix by another matrix and return the result
	  */
	Matrix3& operator *= (const Matrix3& rhs) {
		__multiply(&rhs);
		return (*this);
	}

	/** Multiply this matrix by a constant and return the result
	  */
	Matrix3& operator *= (const float& rhs) {
		__multiply(rhs);
		return (*this);
	}

	/** Divide this matrix by a constant and return the result
	  */
	Matrix3& operator /= (const float& rhs) {
		__divide(rhs);
		return (*this);
	}

	/** Add a matrix to this one and return the result
	  */
	Matrix3& operator += (const Matrix3& rhs) {
		__add(&rhs);
		return (*this);
	}

	/** Subtract a matrix from this one and return the result
	  */
	Matrix3& operator -= (const Matrix3& rhs) {
		__subtract(&rhs);
		return (*this);
	}

	/** Operate on an input vector by multiplying it with this matrix
	  */
	Vector3& TransformInPlace(Vector3& vec) const {
		__transform(&vec);
		return vec;
	}

	/** Operate on an input vector by multiplying it with this matrix
	  */
	Vector3 Transform(const Vector3& vec) const {
		Vector3 retval(vec.dimension);
		__transform(&vec, &retval);
		return retval;
	}

	/** Operate on an input vector by multiplying it with the transpose of this matrix
	  */
	Vector3& TransposeInPlace(Vector3& vec) const {
		__transpose(&vec);
		return vec;
	}

	/** Operate on an input vector by multiplying it with the transpose of this matrix
	  */
	Vector3 Transpose(const Vector3& vec) const {
		Vector3 retval(vec.dimension);
		__transpose(&vec, &retval);
		return retval;
	}

	/** Set the rotation matrix using theta, phi and psi (all in radians)
	  */
	void SetRotation(const float& theta, const float& phi, const float& psi);

	/** Get the rotation matrix for an angle about the x-axis (in radians)
	  */
	static Matrix3 PitchMatrix(const float& angle);

	/** Get the rotation matrix for an angle about the z-axis (in radians)
	  */
	static Matrix3 RollMatrix(const float& angle);

	/** Get the rotation matrix for an angle about the y-axis (in radians)
	  */
	static Matrix3 YawMatrix(const float& angle);

	/** Get the rotation matrix for an FPS style camera, with pitch angle and yaw angle (in radians)
	  */
	static Matrix3 FPSCameraMatrix(const float& pitch, const float& yaw);
};

/// <summary>
/// Four dimensional matrix
/// Elements are in row-major format in memory i.e. {m00, m01, ... , m33} for the matrix
/// with rows {m00, m01, m02, m03}, ... , {m30, m31, m32, m33}
/// </summary>
class Matrix4 : public Matrix {
public:
	/** Empty 2-dimensional matrix
	  */
	Matrix4() : Matrix(4) { }

	/** Identity matrix constructor
	  */
	Matrix4(const float& value) :
		Matrix(4, value)
	{
	}

	/** Explicit element constructor
	  */
	Matrix4(const float* values) :
		Matrix(4, values)
	{
	}

	/** Vector constructor
	  */
	Matrix4(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3) :
		Matrix(4)
	{
		elements[0]  = row0[0]; elements[1]  = row0[1]; elements[2]  = row0[2]; elements[3]  = row0[3];
		elements[4]  = row1[0]; elements[5]  = row1[1]; elements[6]  = row1[2]; elements[7]  = row1[3];
		elements[8]  = row2[0]; elements[9]  = row2[1]; elements[10] = row2[2]; elements[11] = row2[3];
		elements[12] = row3[0]; elements[13] = row3[1]; elements[14] = row3[2]; elements[15] = row3[3];
	}

	/** Translation matrix constructor
	  */
	Matrix4(const Vector3& vec) :
		Matrix(4)
	{
		for (uint16_t i = 0; i < 3; i++) {
			elements[5 * i] = 1.f;
			elements[4 * i + 3] = vec[i];
		}
		elements[15] = 1.f;
	}

	/** Rotation matrix constructor
	  */
	Matrix4(const Matrix3& mat) :
		Matrix(4)
	{
		for (uint16_t i = 0; i < 3; i++) { // over rows
			for (uint16_t j = 0; j < 3; j++) { // over columns
				elements[4 * i + j] = mat[3 * i + j];
			}
		}
		elements[15] = 1.f;
	}

	/** Rotation and translation matrix constructor
	  * Equivalent to the matrix multiplication S = T(R) where T is a translation matrix and R is a rotation matrix
	  */
	Matrix4(const Matrix3& mat, const Vector3& pos) :
		Matrix(4)
	{
		for (uint16_t i = 0; i < 3; i++) { // over rows
			for (uint16_t j = 0; j < 3; j++) { // over columns
				elements[4 * i + j] = mat[3 * i + j];
			}
		}
		elements[12] = -pos[0];
		elements[13] = -pos[1];
		elements[14] = pos[2];
		elements[15] = 1.f;
	}

	/** 3d unit vectors and position vector constructor (camera view matrix)
	*/
	Matrix4(const Vector3& uX, const Vector3& uY, const Vector3& uZ, const Vector3& pos) :
		Matrix(4)
	{
		// Unit vectors
		elements[0] = uX[0]; elements[1] = uX[1]; elements[2] = uX[2];
		elements[4] = uY[0]; elements[5] = uY[1]; elements[6] = uY[2];
		elements[8] = uZ[0]; elements[9] = uZ[1]; elements[10] = uZ[2];
		// Position vector
		elements[12] = -pos[0];
		elements[13] = -pos[1];
		elements[14] = pos[2];
		elements[15] = 1.f;
	}

	/** Explicit matrix element constructor
	  */
	Matrix4(const float& a00, const float& a01, const float& a02, const float& a03,
		const float& a10, const float& a11, const float& a12, const float& a13,
		const float& a20, const float& a21, const float& a22, const float& a23, 
		const float& a30, const float& a31, const float& a32, const float& a33) :
		Matrix(4)
	{
		elements[0]  = a00; elements[1]  = a01; elements[2]  = a02; elements[3]  = a03;
		elements[4]  = a10; elements[5]  = a11; elements[6]  = a12; elements[7]  = a13;
		elements[8]  = a20; elements[9]  = a21; elements[10] = a22; elements[11] = a23;
		elements[12] = a30; elements[13] = a31; elements[14] = a32; elements[15] = a33;
	}

	/** Multiply this matrix by another matrix and return the resulting matrix
	  */
	Matrix4 operator * (const Matrix4& rhs) const {
		Matrix4 retval(rhs.dimension);
		__multiply(&rhs, &retval);
		return retval;
	}

	/** Multiply this matrix by a constant and return the resulting matrix
	  */
	Matrix4 operator * (const float& rhs) const {
		Matrix4 retval(dimension);
		__multiply(rhs, &retval);
		return retval;
	}

	/** Multiply this matrix by a vector and return the resulting vector
	  */
	Vector4 operator * (const Vector4& rhs) const {
		Vector4 retval(rhs.dimension);
		__transform(&retval);
		return retval;
	}

	/** Divide this matrix by a constant and return the result
	  */
	Matrix4 operator / (const float& rhs) const {
		Matrix4 retval(dimension);
		__divide(rhs, &retval);
		return retval;
	}

	/** Add a matrix to this one and return the result
	  */
	Matrix4 operator + (const Matrix4& rhs) const {
		Matrix4 retval(dimension);
		__add(&rhs, &retval);
		return retval;
	}

	/** Subtract a matrix from this one and return the result
	  */
	Matrix4 operator - (const Matrix4& rhs) const {
		Matrix4 retval(dimension);
		__subtract(&rhs, &retval);
		return retval;
	}

	/** Assignment operator
	  */
	Matrix4& operator = (const Matrix4& rhs) {
		__assign(&rhs);
		return (*this);
	}

	/** Multiply this matrix by another matrix and return the result
	  */
	Matrix4& operator *= (const Matrix4& rhs) {
		__multiply(&rhs);
		return (*this);
	}

	/** Multiply this matrix by a constant and return the result
	  */
	Matrix4& operator *= (const float& rhs) {
		__multiply(rhs);
		return (*this);
	}

	/** Divide this matrix by a constant and return the result
	  */
	Matrix4& operator /= (const float& rhs) {
		__divide(rhs);
		return (*this);
	}

	/** Add a matrix to this one and return the result
	  */
	Matrix4& operator += (const Matrix4& rhs) {
		__add(&rhs);
		return (*this);
	}

	/** Subtract a matrix from this one and return the result
	  */
	Matrix4& operator -= (const Matrix4& rhs) {
		__subtract(&rhs);
		return (*this);
	}

	/** Operate on an input vector by multiplying it with this matrix
	  */
	Vector4& TransformInPlace(Vector4& vec) const {
		__transform(&vec);
		return vec;
	}

	/** Operate on an input vector by multiplying it with this matrix
	  */
	Vector4 Transform(const Vector4& vec) const {
		Vector4 retval(vec.dimension);
		__transform(&vec, &retval);
		return retval;
	}

	/** Operate on an input vector by multiplying it with the transpose of this matrix
	  */
	Vector4& TransposeInPlace(Vector4& vec) const {
		__transpose(&vec);
		return vec;
	}

	/** Operate on an input vector by multiplying it with the transpose of this matrix
	  */
	Vector4 Transpose(const Vector4& vec) const {
		Vector4 retval(vec.dimension);
		__transpose(&vec, &retval);
		return retval;
	}

	/** Set a transformaton view-matrix for a FPS-style camera in 3d space
	  * @param pos Focal point of camera
	  * @param rot 3d rotation matrix of camera
	  */
	Matrix4& SetViewMatrixFPS(const Vector3& pos, const Matrix3& rot);

	/** Copy a 3d vector into a column of this matrix
	  * @param col The column to copy the input vector into
	  * @param vec The 3d vector to copy
	  * @param last The remaining value of the 4th element in the column
	  */
	void SetSubMatrixColumn(const uint16_t& col, const Vector3& vec, const float& last = 0.f);

	/** Get a perspective projection transformation matrix for a given scene
	  */
	static Matrix4 PerspectiveMatrix(const float& fov, const float& aspectRatio, const float& nearPlane, const float& farPlane);

	/** Get an orthographic projection transformation matrix for a given scene
	  */
	static Matrix4 OrthographicMatrix(const float& left, const float& right, const float& bottom, const float& top, const float& nearPlane = -1.f, const float& farPlane = 1.f);

	/** Perform the triple matrix multiplication MVP = (P * (V * M))
	  * Note that this multiplication is the opposite of what you would normally
	  *  see when using OpenGL. This is because the Matrix class uses row-major
	  *  ordering while OpenGL uses column-major. So we simply reverse the order
	  *  of multiplication to match the MVP transformation matrix given by glm.
	  * @param P Pointer to the projection transform matrix (perspective, orthographic, etc)
	  * @param V Pointer to the camera's view transform matrix. NOT the camera's model matrix!
	  * @param M Pointer to the object's model transform matrix
	  * @return The total OpenGL transformation matrix (MVP) for a given camera and object
	  */
	static Matrix4 Concatenate(const Matrix4* P, const Matrix4* V, const Matrix4* M);

	/** Get a transformaton view-matrix for a camera looking at a point in 3d space
	  * @param pos Focal point of camera
	  * @param target The point the camera is looking at
	  * @param up The "up" vector, (0,1,0) by default
	  */
	static Matrix4 ViewMatrixLookAt(const Vector3& pos, const Vector3& target, const Vector3& up = constants::kUnitVectorY);

	/** Get a transformaton view-matrix for a FPS-style camera in 3d space
	  * @param pos Focal point of camera
	  * @param pitch The angle about the x-axis (in radians)
	  * @param pitch The angle about the y-axis (in radians)
	  */
	static Matrix4 ViewMatrixFPS(const Vector3& pos, const float& pitch, const float& yaw);
};

namespace constants {

extern const Matrix2 kIdentityMatrix2; ///< 2x2 identity matrix
extern const Matrix3 kIdentityMatrix3; ///< 3x3 identity matrix
extern const Matrix4 kIdentityMatrix4; ///< 4x4 identity matrix

} /* namespace constants */
} /* namespace ott */

#endif // #ifndef MATH_OTT_MATRIX_HPP
