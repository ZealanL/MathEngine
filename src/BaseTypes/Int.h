#pragma once
#include "../Framework.h"

// Representation of an integer
// TODO: Support unlimited length
struct Int {
	int64_t val;
	Int() = default;
	Int(int64_t val) : val(val) {}

#define _MAKE_INT_OP(returnType, op) \
	returnType operator##op(const Int& other) const { \
		return val op other.val; \
	}

#define _MAKE_INT_OP_A(op) \
	_MAKE_INT_OP(Int, op) \
	Int& operator##op##=(const Int& other) { \
		*this = *this op other; return *this; \
	}

	_MAKE_INT_OP(bool, == );
	_MAKE_INT_OP(bool, != );
	_MAKE_INT_OP(bool, < );
	_MAKE_INT_OP(bool, <= );
	_MAKE_INT_OP(bool, > );
	_MAKE_INT_OP(bool, >= );

	_MAKE_INT_OP_A(+);
	_MAKE_INT_OP_A(-);
	_MAKE_INT_OP_A(*);
	_MAKE_INT_OP_A(/);
	_MAKE_INT_OP_A(%);

	Int operator-() const { return -val; }

	static Int GCD(const Int& a, const Int& b);

	static Int FromString(const std::string& string);

	std::string ToString() const {
		return std::to_string(val);
	}
	PRINTABLE(Int);
};

// Math overload
inline Int abs(Int val) {
	return (val < 0) ? -val : val;
}