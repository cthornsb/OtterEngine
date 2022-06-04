#include <ctime>
#include <cstdlib>

#include <math/OTTRandom.hpp>

//#include "OTTRandom.hpp"

uint32_t ott::Random::Rand32() {
	switch (type) {
	case Generator::Default:
		return (nSeed32 = std::rand());
	case Generator::XorShift:
		return this->XorShift32();
	default:
		break;
	}
	return 0;
}

uint64_t ott::Random::Rand64() {
	switch (type) {
	case Generator::Default:
		return (nSeed64 = this->GetUInt64(std::rand(), std::rand()));
	case Generator::XorShift:
		return this->XorShift64();
	default:
		break;
	}
	return 0;
}

uint128_t ott::Random::Rand128() {
	switch (type) {
	case Generator::Default:
		nSeed128.a = std::rand();
		nSeed128.b = std::rand();
		nSeed128.c = std::rand();
		nSeed128.d = std::rand();
		return nSeed128;
	case Generator::XorShift:
		return this->XorShift128();
	default:
		break;
	}
	return uint128_t({ 0, 0, 0, 0 });
}

void ott::Random::Seed() {
	this->Seed((uint32_t)time(0));
}

void ott::Random::Seed(const uint32_t& s) {
	std::srand(s);
	nSeed32 = std::rand();
	nSeed64 = this->GetUInt64(std::rand(), std::rand());
	nSeed128.a = std::rand();
	nSeed128.b = std::rand();
	nSeed128.c = std::rand();
	nSeed128.d = std::rand();
}

uint64_t ott::Random::GetUInt64(const uint32_t& l, const uint32_t& h) {
	return ((uint64_t)l + ((uint64_t)h << 32));
}

uint32_t ott::Random::XorShift32() {
	// Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs"
	nSeed32 ^= (nSeed32 << 13);
	nSeed32 ^= (nSeed32 >> 17);
	nSeed32 ^= (nSeed32 << 5);
	return nSeed32;
}

uint64_t ott::Random::XorShift64() {
	nSeed64 ^= (nSeed64 << 13);
	nSeed64 ^= (nSeed64 >> 7);
	nSeed64 ^= (nSeed64 << 17);
	return nSeed64;
}

/* The state array must be initialized to not be all zero */
uint128_t ott::Random::XorShift128() {
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
