#include "Hash.h"

uint64_t Hash::HashBytes(const void* data, size_t size) {
	// https://gist.github.com/hwei/1950649d523afd03285c

	// https://bigprimes.org/
	constexpr int64_t FNV_PRIME = 3263569254483612721;
	constexpr int64_t OFFSET_BASIS = 1210470885093562381;

	uint8_t* bytes = (uint8_t*)data;

	uint64_t result = OFFSET_BASIS;
	for (size_t i = 0; i < size; i++) {
		result ^= bytes[i];
		result *= FNV_PRIME;
	}

	return result;
}