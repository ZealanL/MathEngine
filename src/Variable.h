#pragma once
#include "Hash.h"

class Variable {
protected:
	std::string name;
	uint64_t hash = 0;
public:

	Variable() = default;
	Variable(std::string name) : name(name) {
		if (!name.empty()) {
			hash = Hash::HashStr(name);
		} else {
			hash = 0;
		}
	}

	uint64_t GetHash() const { return hash; }

	bool operator ==(const Variable& other) const {
		return hash == other.hash;
	}

	bool operator !=(const Variable& other) const {
		return !(*this == other);
	}

	std::string ToString() const {
		if (!name.empty()) {
			return name;
		} else {
			return "[NULL VAR]";
		}
	}
	PRINTABLE(Variable);
};