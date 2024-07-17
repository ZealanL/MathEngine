#pragma once
#include "Framework.h"

enum {
	OP_NONE = 0,

	OP_EQ,

	// Unary
	OP_NEG, OP_FAC,
	
	OP_ADD,

	OP_MUL, OP_DIV,

	OP_POW,

	OPERATOR_AMOUNT
};

constexpr int UNARY_OPS[] = {
	OP_NEG
};

constexpr bool IS_OP_UNARY(int op) {
	for (int unaryOp : UNARY_OPS)
		if (op == unaryOp)
			return true;

	return false;
}

constexpr int UNARY_OP_DIRECTION(int op) {
	if (op == OP_FAC) {
		return -1;
	} else {
		return 1;
	}
}

constexpr bool IS_OP_COMMUNATIVE(int op) {
	return op == OP_ADD || op == OP_MUL;
}

constexpr int OP_PREC[OPERATOR_AMOUNT] = {
	0,

	1, 

	// Negation is the same precedence as mul/div, but is not higher than exponentiation (e.g. -a^b)
	// Factorials are the highest
	// Note that in some cases, unary operators bypass precedence due to being placed between another operator and a value
	13, 16, 

	12,

	13, 13,

	15,
};
constexpr int PREC_MAX = 100;

constexpr const char* OP_STR[OPERATOR_AMOUNT] = {
	"",

	"=",

	"-", "!",

	"+",

	"*", "/",

	"^",
};