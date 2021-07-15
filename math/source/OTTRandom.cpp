#include <ctime>
#include <cstdlib>

#include "OTTRandom.hpp"

uint32_t OTTRandom::rand32() {
	switch (type) {
	case Generator::DEFAULT:
		return (nSeed32 = std::rand());
	case Generator::XORSHIFT:
		return xorshift32();
	default:
		break;
	}
	return 0;
}

uint64_t OTTRandom::rand64() {
	switch (type) {
	case Generator::DEFAULT:
		return (nSeed64 = getUInt64(std::rand(), std::rand()));
	case Generator::XORSHIFT:
		return xorshift64();
	default:
		break;
	}
	return 0;
}

uint128_t OTTRandom::rand128() {
	switch (type) {
	case Generator::DEFAULT:
		nSeed128.a = std::rand();
		nSeed128.b = std::rand();
		nSeed128.c = std::rand();
		nSeed128.d = std::rand();
		return nSeed128;
	case Generator::XORSHIFT:
		return xorshift128();
	default:
		break;
	}
	return uint128_t({ 0, 0, 0, 0 });
}

void OTTRandom::seed() {
	seed((uint32_t)time(0));
}

void OTTRandom::seed(const uint32_t& s) {
	std::srand(s);
	nSeed32 = std::rand();
	nSeed64 = getUInt64(std::rand(), std::rand());
	nSeed128.a = std::rand();
	nSeed128.b = std::rand();
	nSeed128.c = std::rand();
	nSeed128.d = std::rand();
}

uint64_t OTTRandom::getUInt64(const uint32_t& l, const uint32_t& h) {
	return ((uint64_t)l + ((uint64_t)h << 32));
}

uint32_t OTTRandom::xorshift32() {
	// Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs"
	nSeed32 ^= (nSeed32 << 13);
	nSeed32 ^= (nSeed32 >> 17);
	nSeed32 ^= (nSeed32 << 5);
	return nSeed32;
}

uint64_t OTTRandom::xorshift64() {
	nSeed64 ^= (nSeed64 << 13);
	nSeed64 ^= (nSeed64 >> 7);
	nSeed64 ^= (nSeed64 << 17);
	return nSeed64;
}

/* The state array must be initialized to not be all zero */
uint128_t OTTRandom::xorshift128() {
	// Algorithm "xor128" from p. 5 of Marsaglia, "Xorshift RNGs"
	uint32_t t = nSeed128.d;
	t ^= (t << 11);
	t ^= (t >> 8);
	nSeed128.d = nSeed128.c;
	nSeed128.c = nSeed128.b;
	nSeed128.b = nSeed128.a;
	nSeed128.a = (t ^ nSeed128.a ^ (nSeed128.a >> 19));
	return nSeed128;
}
