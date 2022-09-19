#include <string>
#include <iostream>

#include "../../machine/Instruction.h"
#include "InstructionGoedeler.h"

namespace agos {

using std::string;

bool InstructionGoedeler::incr(Instruction& instr, Machine const& machine) {
	if (&instr == NULL) {
		std::cout << "problem in " << __FILE__ << ":" << __LINE__ << std::endl;
	}

	bool carry = true;
	for (size_t i = instr.getParamCount(); i-- != 0 && carry;) {
		Operation::ParamType paramType = instr.getParamType(i);
		size_t paramSpcSz = machine.getParamSpaceSize(paramType);

		size_t param = instr.getParam(i);
		param++;

		if (param == paramSpcSz) {
			param = 0;
		} else {
			carry = false;
		}

		instr.setParam(i, param);
	}

	if (carry) {
		Operation& op = instr.getOp();
		carry = opIncr.incr(op, machine);
		instr.resetParams();
	}

	return carry;
}

//BigInteger InstructionGoedeler::encode(Instruction const& instr,
//		Machine const& machine) const {
//	BigInteger instrSpaceSizeBelow = 0;
//	for (Operation op = Operation::FIRST;; opIncr.incr(op, machine)) {
//		if (op == instr.getOp()) {
//			break;
//		}
//
//		BigInteger opInstrSpaceSize = machine.getInstrSpaceSizeForOp(op);
//		instrSpaceSizeBelow += opInstrSpaceSize;
//
//		if (op.opcode == Operation::LAST) {
//			break;
//		}
//	}
//
//	BigInteger paramAddition = 0;
//	for (size_t i = 0; i < instr.getParamCount(); i++) {
//		size_t param = instr.getParam(i);
//
//		Operation::ParamType paramType = instr.getParamType(i);
//		size_t paramSpcSz = machine.getParamSpaceSize(paramType);
//
//		paramAddition = paramAddition * paramSpcSz + param;
//	}
//
//	return instrSpaceSizeBelow + paramAddition;
//}

//Instruction InstructionGoedeler::decode(BigInteger const& goedelNumber,
//		Machine const& machine) const {
//	BigInteger remaining = goedelNumber;
//
//	Operation op = -1;
//	for (size_t i = 0; i < Operation::COUNT; i++) {
//		op = i;
//
//		BigInteger opInstrSpaceSize = machine.getInstrSpaceSizeForOp(op);
//
//		if (remaining.compareTo(opInstrSpaceSize) >= 0) {
//			remaining -= opInstrSpaceSize;
//		} else {
//			break;
//		}
//	}
//
//	if (op.opcode == -1) {
//		throw string("couldn't determine Operation; probably a bug");
//	}
//
//	size_t paramCount = op.getParamCount();
//	size_t* params = new size_t[paramCount];
//	for (int i = paramCount - 1; i >= 0; i--) {
//		size_t paramSpcSz = machine.getParamSpaceSize(op.getParamType(i));
//
//		BigInteger param = remaining % paramSpcSz;
//		remaining /= paramSpcSz;
//
//		params[i] = param.toLong();
//	}
//
//	Instruction result = Instruction(op, params);
//	delete[] params;
//
//	return result;
//}

} /* namespace agos */
