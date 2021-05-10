#include "Matrix.hpp"

const Matrix2 identityMatrix2(1.f);

const Matrix3 identityMatrix3(1.f);

const Matrix4 identityMatrix4(1.f);

/////////////////////////////////////////////////////////////////////
// class Matrix
/////////////////////////////////////////////////////////////////////

void Matrix::getColumn(const unsigned short& col, Vector* vec) const {
	if (dimension != vec->dimension) // Vector size mismatch
		return;
	for (size_t i = 0; i < dimension; i++) {
		vec->elements[i] = elements[col + i * dimension];
	}
}

void Matrix::getRow(const unsigned short& row, Vector* vec) const {
	if (dimension != vec->dimension) // Vector size mismatch
		return;
	for (size_t i = 0; i < dimension; i++) {
		vec->elements[i] = elements[row * dimension + i];
	}
}

void Matrix::zero() {
	for (size_t i = 0; i < dimension; i++) {
		for (size_t j = 0; j < dimension; j++) {
			elements[dimension * i + j] = 0.f;
		}
	}
}

void Matrix::identity() {
	zero(); // Start by zeroing the matrix
	for (size_t i = 0; i < dimension; i++) {
		elements[dimension * i + i] = 1.f; // Set the diagonal elements to 1
	}
}

void Matrix::__assign(const Matrix* rhs) {
	if (dimension != rhs->dimension) // Matrix size mismatch
		return;
	for (unsigned short i = 0; i < dimension; i++) { // Over columns
		for (unsigned short j = 0; j < dimension; j++) { // Over rows
			elements[dimension * i + j] = rhs->elements[dimension * i + j];
		}
	}
}

void Matrix::__multiply(const Matrix* rhs) {
	if (dimension != rhs->dimension) // Matrix size mismatch
		return;
	Matrix oldMatrix(*this);
	for (unsigned short i = 0; i < dimension; i++) { // Over columns
		for (unsigned short j = 0; j < dimension; j++) { // Over rows
			elements[dimension * i + j] = 0;
			for (unsigned short k = 0; k < dimension; k++) {
				elements[dimension * i + j] += oldMatrix.elements[dimension * i + k] * rhs->elements[dimension * k + j];
			}
		}
	}
}

void Matrix::__multiply(const Matrix* in, Matrix* out) const {
	if (dimension != in->dimension) // Matrix size mismatch
		return;
	for (unsigned short i = 0; i < dimension; i++) { // Over columns
		for (unsigned short j = 0; j < dimension; j++) { // Over rows
			out->elements[dimension * i + j] = 0.f;
			for (unsigned short k = 0; k < dimension; k++) {
				out->elements[dimension * i + j] += elements[dimension * i + k] * in->elements[dimension * k + j];
			}
		}
	}
}

void Matrix::__multiply(const float& rhs) {
	for (unsigned short i = 0; i < dimension; i++) { // Over columns
		for (unsigned short j = 0; j < dimension; j++) { // Over rows
			elements[dimension * i + j] *= rhs;
		}
	}
}

void Matrix::__multiply(const float& in, Matrix* out) const {
	for (unsigned short i = 0; i < dimension; i++) { // Over columns
		for (unsigned short j = 0; j < dimension; j++) { // Over rows
			out->elements[dimension * i + j] = elements[dimension * i + j] * in;
		}
	}
}

void Matrix::__divide(const float& rhs) {
	for (unsigned short i = 0; i < dimension; i++) { // Over columns
		for (unsigned short j = 0; j < dimension; j++) { // Over rows
			elements[dimension * i + j] /= rhs;
		}
	}
}

void Matrix::__divide(const float& in, Matrix* out) const {
	for (unsigned short i = 0; i < dimension; i++) { // Over columns
		for (unsigned short j = 0; j < dimension; j++) { // Over rows
			out->elements[dimension * i + j] = elements[dimension * i + j] / in;
		}
	}
}

void Matrix::__add(const Matrix* rhs) {
	if (dimension != rhs->dimension) // Matrix size mismatch
		return;
	for (unsigned short i = 0; i < dimension; i++) { // Over columns
		for (unsigned short j = 0; j < dimension; j++) { // Over rows
			elements[dimension * i + j] += rhs->elements[dimension * i + j];
		}
	}
}

void Matrix::__add(const Matrix* in, Matrix* out) const {
	if (dimension != in->dimension) // Matrix size mismatch
		return;
	for (unsigned short i = 0; i < dimension; i++) { // Over columns
		for (unsigned short j = 0; j < dimension; j++) { // Over rows
			out->elements[dimension * i + j] = elements[dimension * i + j] + in->elements[dimension * i + j];
		}
	}
}

void Matrix::__subtract(const Matrix* rhs) {
	if (dimension != rhs->dimension) // Matrix size mismatch
		return;
	for (unsigned short i = 0; i < dimension; i++) { // Over columns
		for (unsigned short j = 0; j < dimension; j++) { // Over rows
			elements[dimension * i + j] -= rhs->elements[dimension * i + j];
		}
	}
}

void Matrix::__subtract(const Matrix* in, Matrix* out) const {
	if (dimension != in->dimension) // Matrix size mismatch
		return;
	for (unsigned short i = 0; i < dimension; i++) { // Over columns
		for (unsigned short j = 0; j < dimension; j++) { // Over rows
			out->elements[dimension * i + j] = elements[dimension * i + j] - in->elements[dimension * i + j];
		}
	}
}

void Matrix::__transform(Vector* vec) const {
	if (dimension != vec->dimension) // Matrix size mismatch
		return;
	Vector temp(*vec);
	for (unsigned short i = 0; i < dimension; i++) {
		vec->elements[i] = 0.f;
		for (unsigned short j = 0; j < dimension; j++) {
			vec->elements[i] += elements[dimension * i + j] * temp.elements[j];
		}
	}
}

void Matrix::__transform(const Vector* in, Vector* out) const {
	if (dimension != in->dimension) // Matrix size mismatch
		return;
	for (unsigned short i = 0; i < dimension; i++) {
		out->elements[i] = 0.f;
		for (unsigned short j = 0; j < dimension; j++) {
			out->elements[i] += elements[dimension * i + j] * in->elements[j];
		}
	}
}

void Matrix::__transpose(Vector* vec) const {
	if (dimension != vec->dimension) // Matrix size mismatch
		return;
	Vector temp(*vec);
	for (unsigned short i = 0; i < dimension; i++) {
		vec->elements[i] = 0.f;
		for (unsigned short j = 0; j < dimension; j++) {
			vec->elements[i] += elements[dimension * j + i] * temp.elements[j];
		}
	}
}

void Matrix::__transpose(const Vector* in, Vector* out) const {
	if (dimension != in->dimension) // Matrix size mismatch
		return;
	for (unsigned short i = 0; i < dimension; i++) {
		out->elements[i] = 0.f;
		for (unsigned short j = 0; j < dimension; j++) {
			out->elements[i] += elements[dimension * j + i] * in->elements[j];
		}
	}
}

/////////////////////////////////////////////////////////////////////
// class Matrix2
/////////////////////////////////////////////////////////////////////

void Matrix2::setRotation(const float& theta) {
	elements[0] = std::cos(theta); elements[1] = std::sin(theta);
	elements[2] = -elements[1];    elements[3] = elements[0];
}

Matrix2 Matrix2::getRotationMatrix(const float& theta) {
	return Matrix2(
		std::cos(theta),  std::sin(theta),
		-std::sin(theta), std::cos(theta)
	);
}

/////////////////////////////////////////////////////////////////////
// class Matrix3
/////////////////////////////////////////////////////////////////////

void Matrix3::setRotation(const float& theta, const float& phi, const float& psi) {
	// Pitch-Roll-Yaw convention
	// Multiply the three individual rotation matrices into the full rotation matrix
	(*this) = (getYawMatrix(psi) * (getRollMatrix(phi) * getPitchMatrix(theta)));
}

Matrix3 Matrix3::getYawMatrix(const float& angle) {
	if (angle == 0)
		return identityMatrix3;
	float sin_theta = std::sin(angle);
	float cos_theta = std::cos(angle);
	Matrix3 mat(cos_theta, 0, -sin_theta,
		0, 1, 0,
		sin_theta, 0, cos_theta);
	return mat;
}

Matrix3 Matrix3::getRollMatrix(const float& angle) {
	if (angle == 0)
		return identityMatrix3;
	float sin_phi = std::sin(angle);
	float cos_phi = std::cos(angle);
	Matrix3 mat(cos_phi, sin_phi, 0,
		-sin_phi, cos_phi, 0,
		0, 0, 1);
	return mat;
}

Matrix3 Matrix3::getPitchMatrix(const float& angle) {
	if (angle == 0)
		return identityMatrix3;
	float sin_psi = std::sin(angle);
	float cos_psi = std::cos(angle);
	Matrix3 mat(1, 0, 0,
		0, cos_psi, sin_psi,
		0, -sin_psi, cos_psi);
	return mat;
}

Matrix3 Matrix3::getFPSCameraMatrix(const float& pitch, const float& yaw) {
	float sinPitch = std::sin(pitch);
	float cosPitch = std::cos(pitch);
	float sinYaw = std::sin(yaw);
	float cosYaw = std::cos(yaw);
	return Matrix3(
		cosYaw,  sinYaw * sinPitch, -sinYaw * cosPitch,
		0.f,     cosPitch,           sinPitch,
		sinYaw, -cosYaw * sinPitch,  cosYaw * cosPitch
	);
}

/////////////////////////////////////////////////////////////////////
// class Matrix4
/////////////////////////////////////////////////////////////////////

void Matrix4::setSubMatrixColumn(const unsigned short& col, const Vector3& vec, const float& last/* = 0.f*/) {
	for (unsigned short i = 0; i < 3; i++) { // Get column from input matrix and copy to this
		elements[col + i * 4] = vec[i];
	}
	elements[col + 12] = last; // Set the remaining value in the column
}

Matrix4& Matrix4::setViewMatrixFPS(const Vector3& pos, const Matrix3& rot) {
	Vector3 uX, uY, uZ;
	rot.getColumn(0, &uX);
	rot.getColumn(1, &uY);
	rot.getColumn(2, &uZ);
	setSubMatrixColumn(0, uX, -uX.dot(pos));
	setSubMatrixColumn(1, uY, -uY.dot(pos));
	setSubMatrixColumn(2, uZ, -uZ.dot(pos));
	setSubMatrixColumn(3, zeroVector, 1.f);
	return (*this);
}

Matrix4 Matrix4::getPerspectiveMatrix(const float& fov, const float& aspectRatio, const float& nearPlane, const float& farPlane) {
	float cotan = 1.f / std::tan(fov / 2.f);
	return Matrix4(
		cotan / aspectRatio, 0.f, 0.f, 0.f,
		0.f, cotan, 0.f, 0.f,
		0.f, 0.f, (farPlane + nearPlane) / (nearPlane - farPlane), -1.f,
		0.f, 0.f, 2.f * farPlane * nearPlane / (nearPlane - farPlane), 0.f
	);
}

Matrix4 Matrix4::getOrthographicMatrix(const float& left, const float& right, const float& bottom, const float& top, const float& nearPlane/*=-1.f*/, const float& farPlane/*=1.f*/) {
	float Tx = -(right + left) / (right - left);
	float Ty = -(top + bottom) / (top - bottom);
	float Tz = -(farPlane + nearPlane) / (farPlane - nearPlane);
	return Matrix4(
		2.f / (right - left), 0.f, 0.f, 0.f,
		0.f, 2.f / (top - bottom), 0.f, 0.f,
		0.f, 0.f, 2.f / (nearPlane - farPlane), 0.f,
		Tx, Ty, Tz, 1.f
	);
}

Matrix4 Matrix4::concatenate(const Matrix4* P, const Matrix4* V, const Matrix4* M) {
	return (*M) * ((*V) * (*P));
}

Matrix4 Matrix4::getViewMatrixLookAt(const Vector3& pos, const Vector3& target, const Vector3& up/* = unitVectorY*/) {
	// Point the Z axis unit vector at the point in space
	Vector3 uZ = (pos - target).normalize();

	// Get the X-axis unit vector by computing the cross product of the Y and Z vectors (right vector)
	Vector3 uX = up.cross(uZ).normalize();

	// Compute the new Y-axis unit vector (up vector)
	Vector3 uY = uZ.cross(uX);

	// Return the camera view transformation matrix
	return Matrix4(
		uX[0], uY[0], uZ[0], 0.f,
		uX[1], uY[1], uZ[1], 0.f,
		uX[2], uY[2], uZ[2], 0.f,
		-uX.dot(pos), -uY.dot(pos), -uZ.dot(pos), 1.f
	);
}

Matrix4 Matrix4::getViewMatrixFPS(const Vector3& pos, const float& pitch, const float& yaw) {
	float sinPitch = std::sin(pitch);
	float cosPitch = std::cos(pitch);
	float sinYaw = std::sin(yaw);
	float cosYaw = std::cos(yaw);
	Vector3 uX(cosYaw, 0.f, sinYaw);
	Vector3 uY(sinYaw * sinPitch, cosPitch, -cosYaw * sinPitch);
	Vector3 uZ(-sinYaw * cosPitch, sinPitch, cosYaw * cosPitch);
	return Matrix4(
		uX[0], uY[0], uZ[0], 0.f,
		uX[1], uY[1], uZ[1], 0.f,
		uX[2], uY[2], uZ[2], 0.f,
		-uX.dot(pos), -uY.dot(pos), -uZ.dot(pos), 1.f
	);
}
