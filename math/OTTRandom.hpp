#ifndef MATH_OTT_RANDOM_HPP
#define MATH_OTT_RANDOM_HPP

#include <cstdint>

#ifndef uint128_t
	/** 128-bit unsigned integer composed of four 32-bit integers
	  */
	struct uint128_t {
		uint32_t a, b, c, d;
	};
#endif // ifndef uint128_t

namespace ott {

class Random {
public:
	enum class Generator {
		Default,
		XorShift
	};

	/** Default constructor
	  */
	Random() :
		type(Generator::Default)
	{
		this->Seed();
	}

	/** Generator specifier constructor
	  */
	Random(const Generator& t) :
		type(t)
	{
		this->Seed();
	}

	/** Generate a psuedo-random 32-bit integer using the currently selected generator
	  */
	uint32_t operator () () {
		return this->Rand32();
	}

	/** Generate a psuedo-random 32-bit integer in the range [0, N-1] using the currently selected generator
	  */
	uint32_t operator () (const uint32_t& N) {
		return (this->Rand32() % N);
	}

	/** Get psuedo random number generator algorithm type
	  */
	Generator GetGenerator() const {
		return type;
	}

	/** Set psuedo random number generator algorithm type.
	  * Available generators are DEFAULT and XORSHIFT.
	  */
	void SetGenerator(const Generator& t) {
		type = t;
	}

	/** Seed the 32-bit generator
	  */
	void Seed32(const uint32_t& s) {
		nSeed32 = s;
	}

	/** Seed the 64-bit generator
	  */
	void Seed64(const uint64_t& s) {
		nSeed64 = s;
	}

	/** Seed the 128-bit generator
	  */
	void Seed128(const uint128_t& s) {
		nSeed128 = s;
	}

	/** Seed C++ rand() function with the current system time and then use it to generate 
	  * 32, 64, and 128-bit seed values to initialize generators.
	  */
	void Seed();

	/** Seed C++ rand() function with a specified seed and then use it to generate
	  * 32, 64, and 128-bit seed values to initialize generators.
	  */
	void Seed(const uint32_t& s);

	/** Generate a psuedo-random 32-bit integer using the currently selected generator
	  */
	uint32_t Rand32();

	/** Generate a psuedo-random 64-bit integer using the currently selected generator.
	  * Note that not all generators support 64-bit numbers.
	  */
	uint64_t Rand64();

	/** Generate a psuedo-random 128-bit integer using the currently selected generator.
	  * Note that not all generators support 128-bit numbers.
	  */
	uint128_t Rand128();

protected:
	Generator type; ///< Current PRNG generator type being used

	uint32_t nSeed32; ///< 32-bit seed value

	uint64_t nSeed64; ///< 64-bit seed value

	uint128_t nSeed128; ///< 128-bit seed value

	/** Convert two 32-bit integers to a 64-bit integer
	  * @param l Low 32 bits of 64-bit integer
	  * @param h high 32 bits of 64-bit integer
	  */
	uint64_t GetUInt64(const uint32_t& l, const uint32_t& h);

	/** Generate a 32-bit random number using the 32-bit xorshift algorithm, given an input seed integer.
	  * Seed must be initialized to non-zero.
	  */
	uint32_t XorShift32();

	/** Generate a 64-bit random number using the 64-bit xorshift algorithm, given an input seed integer.
	  * Seed must be initialized to non-zero.
	  */
	uint64_t XorShift64();

	/** Generate a 128-bit random number using the 128-bit xorshift algorithm, given an input seed integer.
	  * Seed must be initialized to non-zero.
	  */
	uint128_t XorShift128();
};

} /* namespace ott */

#endif // #ifndef MATH_OTT_RANDOM_HPP
