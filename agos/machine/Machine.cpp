#include "Machine.h"

#include <cmath>
#include <sstream>
#include <cstring> /* memcmp, memset */
#include <cstdlib> /* realloc, free, exit, NULL */
#include <vector> /* vector */
#include <iostream> /* cout, endl */

#include "../goedelnumbering/multiplier/InstructionGoedeler.h"
#include "../goedelnumbering/multiplier/MachineGoedeler.h"
#include "../stringformat.h"
#include "Instruction.h"
#include "Operation.h"
#include "../compiler/Compiler.h"
#include "../compiler/x64/Compiler_x64.h"

namespace agos {

using std::string;
using std::vector;
using std::cout;
using std::endl;

const Machine::State::Phase Machine::State::Phase::RUNNABLE(0);
const Machine::State::Phase Machine::State::Phase::FINISHED = 1;
const Machine::State::Phase Machine::State::Phase::ABORTED_DIVZERO = 2;
const Machine::State::Phase Machine::State::Phase::ABORTED_INFINITELOOP = 3;

void Machine::State::reset() {
	phase = Machine::State::Phase::RUNNABLE;
	instrPtr = 0;
	memory.fill(0);
}

string Machine::State::toString() const {
	std::ostringstream oss;

	oss << "phase:" << phase.toString();
	oss << " ip:" << instrPtr;

	string s = agos::toString((uint64_t*) memory.begin(),
			(uint64_t*) memory.end());
	oss << " regst:[" << s << "]";

	return oss.str();
}

Machine::State::State(size_t regstCount) :
		instrPtr(0), //
		phase(Machine::State::Phase::RUNNABLE), memory(
				regstCount * sizeof(register_type)) {
}

size_t Machine::State::getRegstCount() const {
	return memory.getSize() / sizeof(register_type);
}

bool Machine::State::operator==(State const& o) const {
	return (phase == o.phase) && (instrPtr == o.instrPtr)
			&& (memory == o.memory);
}

bool Machine::State::operator!=(State const& o) const {
	return (phase != o.phase) || (instrPtr != o.instrPtr)
			|| (memory != o.memory);
}

Machine::Machine() :
		stepCount(0), state(), nextStoreStep(1), program(), opGoed(
				OperationGoedeler::getInstance()) {
}

Machine::Machine(size_t regstCount) :
		stepCount(0), state(regstCount), nextStoreStep(1), program(), opGoed(
				OperationGoedeler::getInstance()) {
}

Machine::Machine(size_t instrCount, size_t regstCount) :
		stepCount(0), state(regstCount), nextStoreStep(1), program(instrCount), opGoed(
				OperationGoedeler::getInstance()) {
}

Machine::Machine(const Program& program, size_t regstCount) :
		stepCount(0), state(regstCount), nextStoreStep(1), program(program), opGoed(
				OperationGoedeler::getInstance()) {
}

void Machine::reset() {
	state.reset();
	stepCount = 0;

	storedState.reset();
	nextStoreStep = 0;
}

bool Machine::containsKnownUselessInstruction() const {
	size_t i = 0;
	for (Program::const_iterator p = program.begin(); p < program.end(); ++p) {
		Instruction const& instr = *p;

		if (isKnownUseless(instr, i)) {
			return true;
		}

		i++;
	}

	return false;
}

bool Machine::isKnownUseless(Instruction const& instr, size_t idx) const {
	// swap(a, a)
	if (instr.op.opcode == Operation::swp && instr.getParam0() == 0) {
		return true;
	}

	if (instr.op.opcode == Operation::jmz) {
		size_t param0 = instr.getParam0();

		// jump to self
		if (param0 == idx) {
			return true;
		}

		// superfluous jump to next
		if (param0 == idx + 1) {
			return true;
		}
	}

	return false;
}

void Machine::compile() const {
	Compiler* compiler = Compiler::getLocalInstance();

	compiler->compileMachine(*this, nativeMachine);
}

void Machine::compileInstr(size_t instrIdx) const {
	Compiler* compiler = Compiler::getLocalInstance();

	compiler->compileInstr(*this, instrIdx, nativeMachine);
}

Machine::State Machine::getState() const {
	return state;
}

void Machine::runNatively(int64_t stepLimit) {
	Compiler::NativeExecutionFunc* nativeFunc =
			(Compiler::NativeExecutionFunc*) nativeMachine.begin();

	// cout << "runNatively() before" << endl;
	nativeFunc(this, stepLimit);
	// cout << "runNatively() after" << endl;
}

void Machine::run(int64_t stepLimit) {
	size_t instrCount = program.size();

	size_t instrPtr = state.instrPtr;
	State::Phase phase = state.phase;
	int64_t* regst = (int64_t*) state.memory.begin();
	int64_t stepCount = this->stepCount;
	stepLimit += stepCount;

	int64_t nextStoreStep = this->nextStoreStep;

	if (phase != State::Phase::RUNNABLE) {
		goto breakRunLoop;
	}

	while (stepCount < stepLimit) {
		if (instrPtr >= instrCount) {
			phase = State::Phase::FINISHED;
			goto breakRunLoop;
		}

		Instruction instr;
		int64_t a;
		int64_t tmp;

		instr = program[instrPtr];

		switch (instr.op.opcode) {
		case Operation::inc:
			regst[0]++;
			instrPtr++;
			break;
		case Operation::dec:
			regst[0]--;
			instrPtr++;
			break;

		case Operation::swp:
			a = instr.getParam0();
			tmp = regst[0];
			regst[0] = regst[a];
			regst[a] = tmp;
			instrPtr++;
			break;
		case Operation::add:
			regst[0] += regst[instr.getParam0()];
			instrPtr++;
			break;
		case Operation::sub:
			regst[0] -= regst[instr.getParam0()];
			instrPtr++;
			break;
		case Operation::mul:
			regst[0] *= regst[instr.getParam0()];
			instrPtr++;
			break;
		case Operation::div:
			a = regst[instr.getParam0()];
			if (a != 0) {
				regst[0] /= a;
				instrPtr++;
			} else {
				phase = State::Phase::ABORTED_DIVZERO;
				goto breakRunLoop;
			}
			break;
		case Operation::mod:
			a = regst[instr.getParam0()];
			if (a != 0) {
				regst[0] %= a;
				instrPtr++;
			} else {
				phase = State::Phase::ABORTED_DIVZERO;
				goto breakRunLoop;
			}
			break;

		case Operation::jmz:
			if (false && stepCount >= nextStoreStep) {
				state.instrPtr = instrPtr;

				if (state == storedState) {
					phase = State::Phase::ABORTED_INFINITELOOP;
					goto breakRunLoop;
				} else {
					storedState = state;

					int64_t nextStoreInterval = (((int64_t) sqrt(
							1 + 8 * stepCount)) - 1) / 2; // reverse little gauss
					nextStoreInterval++;

					nextStoreStep = stepCount + nextStoreInterval;
				}
			}

			instrPtr = (regst[0] == 0) ? instr.getParam0() : instrPtr + 1;
			break;
		case Operation::hlt:
			phase = State::Phase::FINISHED;
			instrPtr++;
			goto breakRunLoop;
			break;

		default:
			instr.op.throwMethodNotPreparedForOperationException();
			break;
		}

		stepCount++;
	}

	breakRunLoop:

	state.instrPtr = instrPtr;
	state.phase = phase;
	this->stepCount = stepCount;
	this->nextStoreStep = nextStoreStep;
}

Program const& Machine::getProgram() const {
	return program;
}

Program& Machine::getProgram() {
	return program;
}

size_t Machine::getRegstCount() const {
	return state.getRegstCount();
}

int64_t Machine::getRegst(size_t idx) const {
	return ((uint64_t*) (state.memory.begin()))[idx];
}

void Machine::setRegst(size_t idx, int64_t value) {
	((uint64_t*) state.memory.begin())[idx] = value;
}

size_t Machine::getInstrCount() const {
	return program.size();
}

void Machine::setInstrCount(size_t count) {
	if (count != program.size()) {
		while (count < getInstrCount()) {
			program.pop_back();
		}

		Instruction firstInstr;
		while (count > getInstrCount()) {
			program.push_back(firstInstr);
		}
	}
}

void Machine::setRegstCount(size_t regstCount) {
	state.setRegstCount(regstCount);
	storedState.setRegstCount(regstCount);
}

void Machine::State::setRegstCount(size_t regstCount) {
	memory.setSize(regstCount * sizeof(register_type));
}

string Machine::State::Phase::toString() const {
	if (code == RUNNABLE.code) {
		return "R";
	} else if (code == FINISHED.code) {
		return "F";

	} else if (code == ABORTED_DIVZERO.code) {
		return "Z";

	} else if (code == ABORTED_INFINITELOOP.code) {
		return "I";
	}

	return strfmt("unknown value for Machine::State::Phase.code: %li", code);
}

//BigInteger slowPow(BigInteger const& base, BigInteger const& exponent) {
//	BigInteger result = 1;
//
//	for (BigInteger i = 0; i < exponent; i++) {
//		result *= base;
//	}
//
//	return result;
//}

//BigInteger operator^(BigInteger const& base, BigInteger const& exponent) {
//	return slowPow(base, exponent);
//}

string Machine::toStringBasic() const {
	std::ostringstream oss;

//	BigInteger instrSpcSz = getInstrSpaceSize();
//	BigInteger progSpcSz = instrSpcSz ^ getInstrCount();
//	BigInteger progGoedNo = ProgramGoedeler::getInstance().encode(
//			this->getProgram(), *this);
//	BigInteger machGoedNo = MachineGoedeler::getInstance().encode(*this);
//
//	oss << "(" << getInstrCount() << ", " << getRegstCount() << ")";
//	oss << " instrSpc:" << instrSpcSz;
//	oss << " progSpc:" << toStringWithSuffix(progSpcSz);
//	oss << " pgNo:" << progGoedNo;
//	oss << " gNo:" << machGoedNo;

	return oss.str();
}

string Machine::toString() const {
	return toStringBasic() + " prog:" + program.toString();
}

string Machine::toStringMultiline() const {
	return toStringBasic() + " prog:" + program.toStringMultiline();
}

string Machine::toStringWithState() const {
	std::ostringstream oss;

	oss << toString();

	oss << " " << state.toString();

	return oss.str();
}

Machine::State::Phase Machine::getPhase() const {
	return state.phase;
}

//BigInteger Machine::getInstrSpaceSize() const {
//	BigInteger result = 0;
//
//	bool carry = false;
//	Operation op(Operation::FIRST);
//	while (!carry) {
//		result += getInstrSpaceSizeForOp(op);
//		carry = opGoed.incr(op, *this);
//	}
//
//	return result;
//}
//
//BigInteger Machine::getInstrSpaceSizeForOp(Operation op) const {
//	BigInteger result;
//
//	size_t paramSpcSz;
//	switch (op.opcode) {
//	case Operation::jmz:
//		paramSpcSz = getParamSpaceSize(Operation::ParamType::INSTRUCTION_INDEX);
//		paramSpcSz -= (2 < paramSpcSz) ? 2 : paramSpcSz; //2: self and next
//		result = paramSpcSz;
//		break;
//
//	case Operation::swp:
//		paramSpcSz = getParamSpaceSize(Operation::ParamType::REGISTER_INDEX);
//		paramSpcSz -= (1 < paramSpcSz) ? 1 : paramSpcSz; //1: swap(a,a) is a nop
//		result = paramSpcSz;
//		break;
//
//	default:
//		result = 1;
//		for (size_t i = 0; i < op.getParamCount(); i++) {
//			Operation::ParamType paramType = op.getParamType(i);
//			paramSpcSz = getParamSpaceSize(paramType);
//			result *= paramSpcSz;
//		}
//	}
//
//	return result;
//}

size_t Machine::getParamSpaceSize(Operation::ParamType paramType) const {
	switch (paramType) {
	case Operation::INSTRUCTION_INDEX:
		return getInstrCount();
	case Operation::REGISTER_INDEX:
		return getRegstCount();
	default:
		Operation::throwMethodNotPreparedForParamTypeException(paramType);
		return -1; // unreachable
	}
}

}
