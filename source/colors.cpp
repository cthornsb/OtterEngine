#include <iostream>

#include "colors.hpp"

sdlColor::sdlColor(const float &value){
	r = convertToUChar(value);
	g = r;
	b = r;
}

sdlColor::sdlColor(const float &red, const float &green, const float &blue){
	r = convertToUChar(red);
	g = convertToUChar(green);
	b = convertToUChar(blue);
}

void sdlColor::toGrayscale(){
	// Based on the sRGB convention
	r = ((unsigned char)(0.2126*r));
	g = ((unsigned char)(0.7152*g));
	b = ((unsigned char)(0.0722*b));
}

void sdlColor::dump() const {
	std::cout << "r=" << (int)r << ", g=" << (int)g << ", b=" << (int)b << std::endl;
}
