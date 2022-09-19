#include "Operation.h"

#include <sstream>
#include <iostream>

namespace agos {

using std::string;

Operation::Operation() :
		opcode((Opcode) 0) {
}

Operation::Operation(Opcode opcode) :
		opcode(opcode) {
}

Operation::Operation(int opcode) :
		opcode((Opcode) opcode) {
}

Operation::operator int() const {
	return opcode;
}

Operation::operator Operation::Opcode() const {
	return opcode;
}

Operation& Operation::operator++() {
	opcode = (Opcode) (opcode + 1);
	return *this;
}

bool Operation::operator==(Operation const& other) const {
	return opcode == other.opcode;
}

string Operation::toString() const {
	switch (opcode) {
	case hlt:
		return "hlt";
	case inc:
		return "inc";
	case dec:
		return "dec";
	case swp:
		return "swp";
	case add:
		return "add";
	case sub:
		return "sub";
	case mul:
		return "mul";
	case div:
		return "div";
	case mod:
		return "mod";
	case jmz:
		return "jmz";
	default:
		throwMethodNotPreparedForOperationException();
		return ""; // unreachable
	}
}

size_t Operation::getParamCount() const {
	size_t result;

	switch (opcode) {
	case hlt:
	case inc:
	case dec:
		result = 0;
		break;
	case swp:
	case add:
	case sub:
	case mul:
	case div:
	case mod:
	case jmz:
		result = 1;
		break;
	default:
		throwMethodNotPreparedForOperationException();
		result = 0; // unreachable
	}

	return result;
}

Operation::ParamType Operation::getParamType(size_t paramIdx) const {
	switch (opcode) {
	case swp:
	case add:
	case sub:
	case mul:
	case div:
	case mod:
		return REGISTER_INDEX;
	case jmz:
		return INSTRUCTION_INDEX;
	default:
		throwMethodNotPreparedForOperationException();
		return (ParamType) -1; // unreachable
	}
}

void Operation::throwMethodNotPreparedForOperationException() const {
	std::ostringstream oss;
	oss << "method is missing implementation for Operation::Code " << opcode;
	throw oss.str();
}

void Operation::throwMethodNotPreparedForParamTypeException(
		ParamType paramType) {
	std::ostringstream oss;
	oss << "method is missing implementation for Operation::ParamType "
			<< paramType;
	throw oss.str();
}

}
