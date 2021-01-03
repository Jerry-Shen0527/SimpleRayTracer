#pragma once
#include <cstdlib>

inline float random_float() {
	// Returns a random real in [0,1).
	return rand() / (RAND_MAX + 1.0);
}

inline float random_float(float min, float max) {
	// Returns a random real in [min,max).
	return min + (max - min) * random_float();
}

inline int random_int(int min, int max) {
	// Returns a random integer in [min,max].
	return static_cast<int>(random_float(min, max));
}

#define PCG32_MULT 0x5851f42d4c957f2dULL
#define PCG32_DEFAULT_STATE 0x853c49e6748fea9bULL
#define PCG32_DEFAULT_STREAM 0xda3e39cb94b95bdbULL

class RNG
{
public:
	RNG() : state(PCG32_DEFAULT_STATE), inc(PCG32_DEFAULT_STREAM) {}
	RNG(uint64_t sequenceIndex) { SetSequence(sequenceIndex); }

	void SetSequence(uint64_t sequenceIndex);
	uint32_t UniformUInt32();

	uint32_t UniformUInt32(uint32_t b) {
		uint32_t threshold = (~b + 1u) % b;
		while (true) {
			uint32_t r = UniformUInt32();
			if (r >= threshold)
				return r % b;
		}
	}

	Float UniformFloat() {
		return std::min(OneMinusEpsilon, UniformUInt32() * 0x1p-32f);
	}

	uint64_t state, inc;
};

inline uint32_t RNG::UniformUInt32()
{
	uint64_t oldstate = state;
	state = oldstate * PCG32_MULT + inc;
	uint32_t xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
	uint32_t rot = (uint32_t)(oldstate >> 59u);
	return (xorshifted >> rot) | (xorshifted << ((~rot + 1u) & 31));
}
