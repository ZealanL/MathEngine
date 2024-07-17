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

struct HashBuilder {
	std::vector<byte> bytes;

	HashBuilder() = default;

	template<typename... Args>
	HashBuilder(Args... args) {
		bytes = {};
		//https://stackoverflow.com/questions/7230621/how-can-i-iterate-over-a-packed-variadic-template-argument-list
		([&] { *this += args; } (), ...);
	}

	template<typename T>
	void Add(const T& val) {
		byte* asBytes = (byte*)&val;
		bytes.insert(bytes.end(), asBytes, asBytes + sizeof(T));
	}

	template<typename T>
	HashBuilder& operator +=(const T& val) {
		Add(val);
		return *this;
	}

	int64_t Get() const {
		return Hash::HashBytes(bytes.data(), bytes.size());
	}
};