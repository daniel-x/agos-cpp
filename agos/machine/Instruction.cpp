#include "Instruction.h"

#include <sstream>
#include <iostream>

namespace agos {

using std::string;
using std::cout;
using std::endl;

Instruction::Instruction() :
		op(Operation::FIRST), param0(0) {
}

Instruction::Instruction(Operation op) :
		op(op), param0(0) {
}

Instruction::Instruction(Operation op, size_t param0) :
		op(op), param0(param0) {

}

Instruction::Instruction(Operation op, size_t param0, size_t param1) :
		op(op), param0(param0) {
	setParam(1, param1);
}

Instruction::Instruction(Instruction const& src) :
		op(src.op), param0(src.param0) {
}

Instruction::Instruction(Operation op, size_t* params) :
		op(op) {

	size_t paramCount = op.getParamCount();
	for (size_t i = 0; i < paramCount; i++) {
		setParam(i, params[i]);
	}
}

Instruction& Instruction::operator=(Instruction const& src) {
	this->op = src.op;
	this->param0 = src.param0;

	return *this;
}

bool Instruction::operator==(Instruction const& other) const {
	bool interimResult = (this->op == other.op);
	if (!interimResult) {
		return interimResult;
	}

	for (size_t i = 0; i < getParamCount(); i++) {
		bool interimResult = (this->getParam(i) == other.getParam(i));
		if (!interimResult) {
			return interimResult;
		}
	}

	return true;
}

Operation Instruction::getOp() const {
	return op;
}

Operation& Instruction::getOp() {
	return op;
}

size_t Instruction::getParamCount() const {
	return op.getParamCount();
}

Operation::ParamType Instruction::getParamType(size_t idx) const {
	return op.getParamType(idx);
}

size_t Instruction::getParam(size_t idx) const {
	switch (idx) {
	case 0:
		return param0;
	}

	couldntHandleParamIdx(idx);
	return 0; // unreachable
}

void Instruction::setParam(size_t idx, size_t value) {
	switch (idx) {
	case 0:
		param0 = value;
		return;
	}

	couldntHandleParamIdx(idx);
}

void Instruction::couldntHandleParamIdx(size_t idx) const {
	std::ostringstream oss;
	if (idx < 0) {
		oss << "IndexOutOfBoundsException: paramater idx may not be negative: "
				<< idx;
		throw oss.str();
	}
	if (idx >= getParamCount()) {
		oss
				<< "IndexOutOfBoundsException: paramater idx may not be >= paramCount: "
				<< idx << " >= " << getParamCount();
		throw oss.str();
	}

	oss << "UnsupportedOperationException: paramater with index " << idx
			<< "not yet implemented";
	throw oss.str();
}

string Instruction::toIdentifierName(size_t i) {
	const size_t CHAR_AR_LEN = 8;
	char buf[CHAR_AR_LEN];
	int charPos = CHAR_AR_LEN - 1;

	while (i >= 26) {
		buf[charPos--] = (char) ('a' + (i % 26));
		i /= 26;
	}
	buf[charPos] = (char) ('a' + i);

	string result(buf + charPos, (CHAR_AR_LEN - charPos));

	return result;
}

string Instruction::toString() const {
	std::ostringstream oss;

	switch (op.opcode) {
	case Operation::inc:
		oss << "a++;";
		break;
	case Operation::dec:
		oss << "a--;";
		break;

	case Operation::swp:
		oss << "swap(a, " << param0ToString() << ");";
		break;
	case Operation::add:
		oss << "a += " << param0ToString() << ";";
		break;
	case Operation::sub:
		oss << "a -= " << param0ToString() << ";";
		break;
	case Operation::mul:
		oss << "a *= " << param0ToString() << ";";
		break;
	case Operation::div:
		oss << "a /= " << param0ToString() << ";";
		break;
	case Operation::mod:
		oss << "a %= " << param0ToString() << ";";
		break;

	case Operation::jmz:
		oss << "if (a == 0) goto " << param0ToString() << ";";
		break;
	case Operation::hlt:
		oss << "return a;";
		break;

	default:
		op.throwMethodNotPreparedForOperationException();
		return NULL; // unreachable
	}

	return oss.str();
}

string Instruction::paramToString(size_t idx) const {
	return paramToString(getParam(idx), getParamType(0));
}

string Instruction::param0ToString() const {
	return paramToString(param0, op.getParamType(0));
}

string Instruction::paramToString(size_t param,
		Operation::ParamType paramType) {
	std::ostringstream oss;

	switch (paramType) {
	case Operation::INSTRUCTION_INDEX:
		oss << "i" << param;
		return oss.str();
	case Operation::REGISTER_INDEX:
		oss << toIdentifierName(param);
		return oss.str();
	default:
		Operation::throwMethodNotPreparedForParamTypeException(paramType);
		return NULL; // unreachable
	}
}

void Instruction::setOp(Operation op) {
	this->op = op;
}

size_t Instruction::getParam0() const {
	return param0;
}

void Instruction::resetParams() {
	param0 = 0;
}

}
