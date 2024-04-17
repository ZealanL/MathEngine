#pragma once
#include "Int.h"
#include "Variable.h"

struct Value {
	enum {
		TYPE_UNDEFINED,
		TYPE_INT,
		TYPE_VAR
	};
	int type = TYPE_UNDEFINED;

	Int intVal;
	Variable varVal;

	Value() = default;
	Value(const Int& intVal) : type(TYPE_INT), intVal(intVal) {}
	Value(const Variable& varVal) : type(TYPE_VAR), varVal(varVal) {}

	bool Defined() const { return type != TYPE_UNDEFINED; }

	bool IsInt() const { return type == TYPE_INT; }
	bool IsVar() const { return type == TYPE_VAR; }

	bool operator==(const Value& other) const {
		if (type != other.type)
			return false;

		switch (type) {
		case TYPE_UNDEFINED:
			return false;
		case TYPE_INT:
			return intVal == other.intVal;
		case TYPE_VAR:
			return varVal == other.varVal;
		default:
			ERR("Invalid value type: " << type);
			return false;
		}
	}

	std::string ToString() const {
		switch (type) {
		case TYPE_UNDEFINED:
			return "undefined";
		case TYPE_INT:
			return intVal.ToString();
		case TYPE_VAR:
			return varVal.ToString();
		default:
			ERR("Invalid Value type: " << type);
			return {};
		}
	}
	PRINTABLE(Value);
};