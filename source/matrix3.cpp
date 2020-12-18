#include <iomanip>
#include <sstream>
#include <cmath>

#include "matrix3.hpp"

const matrix3 identityMatrix(1, 0, 0,
                             0, 1, 0,
                             0, 0, 1);

matrix3::matrix3(){ 
	zero(); 
}

matrix3::matrix3(const vector3 &vec){
	zero();
	setRotation(vec);
}

matrix3::matrix3(const float &theta, const float &phi, const float &psi){
	zero();
	setRotation(theta, phi, psi);
}

matrix3::matrix3(const float &a00, const float &a10, const float &a20,
                 const float &a01, const float &a11, const float &a21,
                 const float &a02, const float &a12, const float &a22){
	elements[0][0] = a00; elements[1][0] = a10; elements[2][0] = a20; 
	elements[0][1] = a01; elements[1][1] = a11; elements[2][1] = a21; 
	elements[0][2] = a02; elements[1][2] = a12; elements[2][2] = a22; 
}

matrix3::matrix3(const matrix3 &other){
	for(unsigned int i = 0; i < 3; i++){ // Over columns
		for(unsigned int j = 0; j < 3; j++){ // Over rows
			elements[i][j] = other.elements[i][j];
		}
	}
}

matrix3& matrix3::operator = (const matrix3 &rhs){
	for(unsigned int i = 0; i < 3; i++){ // Over columns
		for(unsigned int j = 0; j < 3; j++){ // Over rows
			elements[i][j] = rhs.elements[i][j];
		}
	}
	return (*this);
}

matrix3 matrix3::operator * (const matrix3 &rhs) const {
	matrix3 retval;
	for(unsigned int i = 0; i < 3; i++){ // Over columns
		for(unsigned int j = 0; j < 3; j++){ // Over rows
			for(unsigned int k = 0; k < 3; k++){
				retval.elements[i][j] += elements[i][k]*rhs.elements[k][j];
			}
		}
	}
	return retval;
}

matrix3 matrix3::operator * (const float &rhs) const {
	matrix3 retval;
	for(unsigned int i = 0; i < 3; i++){ // Over columns
		for(unsigned int j = 0; j < 3; j++){ // Over rows
			retval.elements[i][j] = elements[i][j]*rhs;
		}
	}
	return retval;
}

vector3 matrix3::operator * (const vector3 &rhs) const {
	vector3 retval;
	retval.x = elements[0][0]*rhs.x + elements[0][1]*rhs.y + elements[0][2]*rhs.z;
	retval.y = elements[1][0]*rhs.x + elements[1][1]*rhs.y + elements[1][2]*rhs.z;
	retval.z = elements[2][0]*rhs.x + elements[2][1]*rhs.y + elements[2][2]*rhs.z;
	return retval;
}

matrix3 matrix3::operator / (const float &rhs) const {
	matrix3 retval;
	for(unsigned int i = 0; i < 3; i++){ // Over columns
		for(unsigned int j = 0; j < 3; j++){ // Over rows
			for(unsigned int k = 0; k < 3; k++){
				retval.elements[i][j] += elements[i][k]/rhs;
			}
		}
	}
	return retval;
}

matrix3 matrix3::operator + (const matrix3 &rhs) const {
	matrix3 retval;
	for(unsigned int i = 0; i < 3; i++){ // Over columns
		for(unsigned int j = 0; j < 3; j++){ // Over rows
			retval.elements[i][j] = elements[i][j] + rhs.elements[i][j];
		}
	}
	return retval;
}

matrix3 matrix3::operator - (const matrix3 &rhs) const {
	matrix3 retval;
	for(unsigned int i = 0; i < 3; i++){ // Over columns
		for(unsigned int j = 0; j < 3; j++){ // Over rows
			retval.elements[i][j] = elements[i][j] - rhs.elements[i][j];
		}
	}
	return retval;
}

matrix3& matrix3::operator *= (const matrix3 &rhs){
	matrix3 oldMatrix(*this);
	for(unsigned int i = 0; i < 3; i++){ // Over columns
		for(unsigned int j = 0; j < 3; j++){ // Over rows
			elements[i][j] = 0;
			for(unsigned int k = 0; k < 3; k++){
				elements[i][j] += oldMatrix.elements[i][k]*rhs.elements[k][j];
			}
		}
	}
	return (*this);
}

matrix3& matrix3::operator *= (const float &rhs){
	for(unsigned int i = 0; i < 3; i++){ // Over columns
		for(unsigned int j = 0; j < 3; j++){ // Over rows
			elements[i][j] *= rhs;
		}
	}
	return (*this);
}

matrix3& matrix3::operator /= (const float &rhs){
	for(unsigned int i = 0; i < 3; i++){ // Over columns
		for(unsigned int j = 0; j < 3; j++){ // Over rows
			elements[i][j] /= rhs;
		}
	}
	return (*this);
}

matrix3& matrix3::operator += (const matrix3 &rhs){
	for(unsigned int i = 0; i < 3; i++){ // Over columns
		for(unsigned int j = 0; j < 3; j++){ // Over rows
			elements[i][j] += rhs.elements[i][j];
		}
	}
	return (*this);
}

matrix3& matrix3::operator -= (const matrix3 &rhs){
	for(unsigned int i = 0; i < 3; i++){ // Over columns
		for(unsigned int j = 0; j < 3; j++){ // Over rows
			elements[i][j] -= rhs.elements[i][j];
		}
	}
	return (*this);
}

void matrix3::getRow(const size_t &row, vector3 &vec) const { 
	vec.x = elements[0][row];  vec.y = elements[1][row];  vec.z = elements[2][row]; 
}

vector3 matrix3::getRow(const size_t &row) const { 
	return vector3(elements[0][row], elements[1][row], elements[2][row]); 
}

void matrix3::setRow(const size_t &row, const vector3 &vec){ 
	elements[0][row] = vec.x; elements[1][row] = vec.y; elements[2][row] = vec.z; 
}

void matrix3::setRow(const size_t &row, const float &p1, const float &p2, const float &p3){ 
	elements[0][row] = p1; elements[0][row] = p2; elements[0][row] = p3; 
}

void matrix3::setRotation(const vector3 &vec){
	setRotation(vec.x, vec.y, vec.z);
}

void matrix3::setRotation(const float &theta, const float &phi, const float &psi){
	// Pitch-Roll-Yaw convention
	matrix3 thetaM = getPitchMatrix(theta);
	matrix3 phiM   = getRollMatrix(phi);
	matrix3 psiM   = getYawMatrix(psi);
	
	// Multiply the three individual rotation matrices into the full rotation matrix
	(*this) = psiM*(phiM*thetaM);
}

matrix3 matrix3::getYawMatrix(const float &angle){
	if(angle == 0)
		return identityMatrix;
	float sin_theta = std::sin(angle);
	float cos_theta = std::cos(angle);
	matrix3 mat(cos_theta, 0, -sin_theta,
	                    0, 1,          0,
	            sin_theta, 0,  cos_theta);
	return mat;
}

matrix3 matrix3::getRollMatrix(const float &angle){
	if(angle == 0)
		return identityMatrix;
	float sin_phi = std::sin(angle);
	float cos_phi = std::cos(angle);
	matrix3 mat( cos_phi, sin_phi, 0,
	            -sin_phi, cos_phi, 0,
	                   0,       0, 1);
	return mat;
}

matrix3 matrix3::getPitchMatrix(const float &angle){
	if(angle == 0)
		return identityMatrix;
	float sin_psi = std::sin(angle);
	float cos_psi = std::cos(angle);
	matrix3 mat(1,        0,       0,
	            0,  cos_psi, sin_psi,
	            0, -sin_psi, cos_psi);
	return mat;
}

// transform an input vector by this matrix
// Note: Expects the input vector to be in cartesian coordinates
void matrix3::transform(vector3 &vector) const {
	float x = vector.x, y = vector.y, z = vector.z;
	vector.x = elements[0][0]*x + elements[0][1]*y + elements[0][2]*z;
	vector.y = elements[1][0]*x + elements[1][1]*y + elements[1][2]*z;
	vector.z = elements[2][0]*x + elements[2][1]*y + elements[2][2]*z;
}

// transform an input vector by the transpose of this matrix
// Note: Expects the input vector to be in cartesian coordinates
void matrix3::transpose(vector3 &vector) const {
	float x = vector.x, y = vector.y, z = vector.z;
	vector.x = elements[0][0]*x + elements[1][0]*y + elements[2][0]*z;
	vector.y = elements[0][1]*x + elements[1][1]*y + elements[2][1]*z;
	vector.z = elements[0][2]*x + elements[1][2]*y + elements[2][2]*z;
}

std::string matrix3::dump(){
	std::stringstream stream;
	stream.precision(3);
	stream << std::fixed;
	stream << "[" << std::setw(8) << elements[0][0] << ", " << std::setw(8) << elements[0][1] << ", " << std::setw(8) << elements[0][2] << "]\n";
	stream << "[" << std::setw(8) << elements[1][0] << ", " << std::setw(8) << elements[1][1] << ", " << std::setw(8) << elements[1][2] << "]\n";
	stream << "[" << std::setw(8) << elements[2][0] << ", " << std::setw(8) << elements[2][1] << ", " << std::setw(8) << elements[2][2] << "]\n";
	return stream.str();
}

void matrix3::zero(){
	for(size_t i = 0; i < 3; i++){ 
		for(size_t j = 0; j < 3; j++){
			elements[i][j] = 0.0; 
		}
	}
}

void matrix3::identity(){
	zero(); // Start by zeroing the matrix
	for(size_t i = 0; i < 3; i++){ 
		elements[i][i] = 1.0; // Set the diagonal elements to 1
	}
}
