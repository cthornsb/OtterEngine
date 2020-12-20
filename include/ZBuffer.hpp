#ifndef ZBUFFER_HPP
#define ZBUFFER_HPP

#include <vector>
#include <float.h>

#include "triangle.hpp"

class ZBuffer {
public:
	class BufferBit {
	public:
		BufferBit() :
			zdepth(FLT_MAX),
			tri(0x0)
		{
		}

		bool set(const float& depth, const triangle* t) {
			if (depth < zdepth) {
				zdepth = depth;
				tri = t;
				return true;
			}
			return false;
		}

		float zdepth;

		const triangle* tri;
	};

	ZBuffer() { }

	ZBuffer(const unsigned short& W_, const unsigned short& H_) :
		W(W_),
		H(H_),
		pixels(H_, std::vector<BufferBit>(W_, BufferBit()))
	{
	}

	bool set(const unsigned short& x, const unsigned short& y, const float& depth, const triangle* tri) {
		return pixels[y][x].set(depth, tri);
	}

	float getDepth(const unsigned short& x, const unsigned short& y) const { return pixels[y][x].zdepth; }

	const triangle* getTriangle(const unsigned short& x, const unsigned short& y) const { return pixels[y][x].tri; }

private:
	unsigned short W;
	unsigned short H;

	std::vector<std::vector<BufferBit> > pixels;
};

#endif