#include "Int.h"

Int Int::GCD(const Int& a, const Int& b) {
	return Int(std::gcd(a.val, b.val));
}

Int Int::FromString(const std::string& string) {
	return std::stoll(string);
}