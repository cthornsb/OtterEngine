#ifndef ZBUFFER_HPP
#define ZBUFFER_HPP

#include <vector>
#include <float.h>

class pixelTriplet;

class BufferBit {
public:
	BufferBit() :
		zdepth(FLT_MAX),
		tri(0x0)
	{
	}

	bool set(const float& depth, const pixelTriplet* t);

	float zdepth;

	const pixelTriplet* tri;
};

typedef std::vector<std::vector<BufferBit> > BufferVector;

class ZBuffer {
public:
	ZBuffer() :
		W(0),
		H(0),
		pixels()
	{ 
	}

	ZBuffer(const unsigned short& W_, const unsigned short& H_) :
		W(W_),
		H(H_),
		pixels(H_, std::vector<BufferBit>(W_, BufferBit()))
	{
	}

	bool set(const unsigned short& x, const unsigned short& y, const float& depth, const pixelTriplet* tri);

	void reset();

	float getDepth(const unsigned short& x, const unsigned short& y) const;

	const pixelTriplet* getTriangle(const unsigned short& x, const unsigned short& y) const;

private:
	unsigned short W;
	unsigned short H;

	std::vector<std::vector<BufferBit> > pixels;
};

#endif