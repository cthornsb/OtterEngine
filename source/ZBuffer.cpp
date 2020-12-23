#include <algorithm>

#include "ZBuffer.hpp"
#include "pixelTriplet.hpp"

bool BufferBit::set(const float& depth, const pixelTriplet* t) {
	if (depth < zdepth) {
		zdepth = depth;
		tri = t;
		return true;
	}
	return false;
}

bool ZBuffer::set(const unsigned short& x, const unsigned short& y, const float& depth, const pixelTriplet* tri) {
	return pixels[y][x].set(depth, tri);
}

void ZBuffer::reset() {
	for (BufferVector::iterator iter = pixels.begin(); iter != pixels.end(); iter++) { // Over rows
		std::fill(iter->begin(), iter->end(), BufferBit());
	}
}

float ZBuffer::getDepth(const unsigned short& x, const unsigned short& y) const {
	return pixels[y][x].zdepth; 
}

const pixelTriplet* ZBuffer::getTriangle(const unsigned short& x, const unsigned short& y) const {
	return pixels[y][x].tri; 
}