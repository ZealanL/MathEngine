#include "Int.h"

Int Int::GCD(const Int& a, const Int& b) {
	return Int(std::gcd(a.val, b.val));
}

Int Int::FromString(const std::string& string) {
	return std::stoll(string);
}

Int Int::Sqrt() const {
	// TODO: Fix for giant numbers

	if (val < 0)
		return -1;

	// Perfect roots must end with 0, 1, 4, or 9 in hexidecimal
	// Examples:
	//  0 -> 0x00,  1 -> 0x01,  4 -> 0x04,  9 -> 0x09, 16 -> 0x10,
	// 25 -> 0x19, 36 -> 0x24, 49 -> 0x31, 64 -> 0x40, 81 -> 0x51
	int end = val & 0xF;
	if (end == 0 || end == 1 || end == 4 || end == 9) {
		int64_t squareRoot = sqrt(val);
		if ((squareRoot * squareRoot) == val) {
			return squareRoot;
		} else {
			return -1;
		}
	} else {
		return -1;
	}
}