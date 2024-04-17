#pragma once
#include "Framework.h"

namespace Hash {
	// FNV hash
	int64_t HashBytes(const void* data, size_t size);

	template<typename T, typename = std::enable_if_t<std::is_fundamental<T>::value>>
	int64_t Hash(const T& val) {
		return HashBytes(&val, sizeof(T));
	}

	inline int64_t HashStr(const std::string& str) {
		return HashBytes(str.data(), str.size());
	}
}