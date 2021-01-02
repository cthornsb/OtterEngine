#include <iomanip>
#include <sstream>
#include <cmath>

#include "matrix2.hpp"

const matrix2 identityMatrix2(1, 0,
                              0, 1);

matrix2::matrix2(){ 
	zero(); 
}

matrix2::matrix2(const vector2 &vec){
	zero();
	setRotation(vec);
}

matrix2::matrix2(const float &theta){
	zero();
	setRotation(theta);
}

matrix2::matrix2(const float &a00, const float &a10,
                 const float &a01, const float &a11){
	elements[0][0] = a00; elements[1][0] = a10;
	elements[0][1] = a01; elements[1][1] = a11;
}

matrix2::matrix2(const matrix2 &other){
	for(unsigned int i = 0; i < 2; i++){ // Over columns
		for(unsigned int j = 0; j < 2; j++){ // Over rows
			elements[i][j] = other.elements[i][j];
		}
	}
}

matrix2& matrix2::operator = (const matrix2 &rhs){
	for(unsigned int i = 0; i < 2; i++){ // Over columns
		for(unsigned int j = 0; j < 2; j++){ // Over rows
			elements[i][j] = rhs.elements[i][j];
		}
	}
	return (*this);
}

matrix2 matrix2::operator * (const matrix2 &rhs) const {
	matrix2 retval;
	for(unsigned int i = 0; i < 2; i++){ // Over columns
		for(unsigned int j = 0; j < 2; j++){ // Over rows
			for(unsigned int k = 0; k < 2; k++){
				retval.elements[i][j] += elements[i][k]*rhs.elements[k][j];
			}
		}
	}
	return retval;
}

matrix2 matrix2::operator * (const float &rhs) const {
	matrix2 retval;
	for(unsigned int i = 0; i < 2; i++){ // Over columns
		for(unsigned int j = 0; j < 2; j++){ // Over rows
			retval.elements[i][j] = elements[i][j]*rhs;
		}
	}
	return retval;
}

vector2 matrix2::operator * (const vector2 &rhs) const {
	vector2 retval;
	retval.x = elements[0][0]*rhs.x + elements[0][1]*rhs.y;
	retval.y = elements[1][0]*rhs.x + elements[1][1]*rhs.y;
	return retval;
}

matrix2 matrix2::operator / (const float &rhs) const {
	matrix2 retval;
	for(unsigned int i = 0; i < 2; i++){ // Over columns
		for(unsigned int j = 0; j < 2; j++){ // Over rows
			for(unsigned int k = 0; k < 2; k++){
				retval.elements[i][j] += elements[i][k]/rhs;
			}
		}
	}
	return retval;
}

matrix2 matrix2::operator + (const matrix2 &rhs) const {
	matrix2 retval;
	for(unsigned int i = 0; i < 2; i++){ // Over columns
		for(unsigned int j = 0; j < 2; j++){ // Over rows
			retval.elements[i][j] = elements[i][j] + rhs.elements[i][j];
		}
	}
	return retval;
}

matrix2 matrix2::operator - (const matrix2 &rhs) const {
	matrix2 retval;
	for(unsigned int i = 0; i < 2; i++){ // Over columns
		for(unsigned int j = 0; j < 2; j++){ // Over rows
			retval.elements[i][j] = elements[i][j] - rhs.elements[i][j];
		}
	}
	return retval;
}

matrix2& matrix2::operator *= (const matrix2 &rhs){
	matrix2 oldMatrix(*this);
	for(unsigned int i = 0; i < 2; i++){ // Over columns
		for(unsigned int j = 0; j < 2; j++){ // Over rows
			elements[i][j] = 0;
			for(unsigned int k = 0; k < 2; k++){
				elements[i][j] += oldMatrix.elements[i][k]*rhs.elements[k][j];
			}
		}
	}
	return (*this);
}

matrix2& matrix2::operator *= (const float &rhs){
	for(unsigned int i = 0; i < 2; i++){ // Over columns
		for(unsigned int j = 0; j < 2; j++){ // Over rows
			elements[i][j] *= rhs;
		}
	}
	return (*this);
}

matrix2& matrix2::operator /= (const float &rhs){
	for(unsigned int i = 0; i < 2; i++){ // Over columns
		for(unsigned int j = 0; j < 2; j++){ // Over rows
			elements[i][j] /= rhs;
		}
	}
	return (*this);
}

matrix2& matrix2::operator += (const matrix2 &rhs){
	for(unsigned int i = 0; i < 2; i++){ // Over columns
		for(unsigned int j = 0; j < 2; j++){ // Over rows
			elements[i][j] += rhs.elements[i][j];
		}
	}
	return (*this);
}

matrix2& matrix2::operator -= (const matrix2 &rhs){
	for(unsigned int i = 0; i < 2; i++){ // Over columns
		for(unsigned int j = 0; j < 2; j++){ // Over rows
			elements[i][j] -= rhs.elements[i][j];
		}
	}
	return (*this);
}

void matrix2::getRow(const size_t &row, vector2 &vec) const { 
	vec.x = elements[0][row];  vec.y = elements[1][row];
}

vector2 matrix2::getRow(const size_t &row) const { 
	return vector2(elements[0][row], elements[1][row]); 
}

void matrix2::setRow(const size_t &row, const vector2 &vec){ 
	elements[0][row] = vec.x; elements[1][row] = vec.y;
}

void matrix2::setRow(const size_t &row, const float &p1, const float &p2){ 
	elements[0][row] = p1; elements[1][row] = p2;
}

void matrix2::setRotation(const vector2 &vec){
	setRotation(vec.x);
}

void matrix2::setRotation(const float &theta){
	elements[0][0] = std::cos(theta); elements[1][0] = -std::sin(theta);
	elements[0][1] = -elements[1][0]; elements[1][1] = elements[0][0];
}

// transform an input vector by this matrix
// Note: Expects the input vector to be in cartesian coordinates
void matrix2::transformInPlace(vector2 &vector) const {
	float x = vector.x, y = vector.y;
	vector.x = elements[0][0] * x + elements[0][1] * y;
	vector.y = elements[1][0] * x + elements[1][1] * y;
}

vector2 matrix2::transform(const vector2& vector) const {
	vector2 retval(
		elements[0][0] * vector.x + elements[0][1] * vector.y,
		elements[1][0] * vector.x + elements[1][1] * vector.y
	);
	return retval;
}

// transform an input vector by the transpose of this matrix
// Note: Expects the input vector to be in cartesian coordinates
void matrix2::transpose(vector2 &vector) const {
	float x = vector.x, y = vector.y;
	vector.x = elements[0][0] * x + elements[1][0] * y;
	vector.y = elements[0][1] * x + elements[1][1] * y;
}

std::string matrix2::dump() const {
	std::stringstream stream;
	stream.precision(3);
	stream << std::fixed;
	stream << "[" << std::setw(8) << elements[0][0] << ", " << std::setw(8) << elements[0][1] << "]\n";
	stream << "[" << std::setw(8) << elements[1][0] << ", " << std::setw(8) << elements[1][1] << "]\n";
	return stream.str();
}

void matrix2::zero(){
	for(size_t i = 0; i < 2; i++){ 
		for(size_t j = 0; j < 2; j++){
			elements[i][j] = 0.0; 
		}
	}
}

void matrix2::identity(){
	zero(); // Start by zeroing the matrix
	for(size_t i = 0; i < 2; i++){ 
		elements[i][i] = 1.0; // Set the diagonal elements to 1
	}
}
