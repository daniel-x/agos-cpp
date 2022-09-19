#include "Compiler_x64.h"
#include "../../goedelnumbering/multiplier/OperationGoedeler.h"
#include <cstring> /* memcpy */
#include <cstdio>

// assemble_raw.sh:
//
//#!/bin/bash
//
//as --version >/dev/null 2>&1 || { echo >&2 "$0: error: assembler 'as' not found. aborting."; exit 1; }
//ld --version >/dev/null 2>&1 || { echo >&2 "$0: error: linker 'ld' not found. aborting."; exit 1; }
//
//tempfile="`tempfile`"
//
//while read line
//do
//    echo -e ".globl _start; _start: $line" | as "$@" -o "$tempfile" || exit 1
//    bin=`ld -Ttext 2000 --oformat binary --output=/dev/stdout "$tempfile" | xxd -p` || exit 1
//    if [[ ! -z "${bin// /}" ]]; then
//        echo "$line # $bin"
//    else
//        echo "$line"
//    fi
//done
//
//rm "$tempfile"

namespace agos {

Compiler_x64* Compiler_x64::getInstance() {
	static Compiler_x64 INSTANCE;
	return &INSTANCE;
}

Compiler_x64::Compiler_x64() {
}

size_t Compiler_x64::getCodeSize(const Machine& machine) {
	return 0 + PF.LIST[PF.Id::MACHINE_PROL].size
			+ PF.instructionCodeSize * machine.getInstrCount()
			+ PF.LIST[PF.Id::LAST_INSTR_EPIL].size
			+ PF.LIST[PF.Id::MACHINE_EPIL].size //
	;
}

void Compiler_x64::compileMachine(const Machine& src, ExecutableMemory& dst) {
	dst.setSize(getCodeSize(src));

	dst.fill(0xff); // 0xff: bad opcode, used as a marker for debugging

	uint8_t* code = dst.begin();

	// printf("-----------------------------\n");
	// printf("%#018lx: machine prologue\n", (uint64_t) code);

	code = compileMachinePrologue(src, code);

	const Program& srcProgram = src.getProgram();
	for (size_t instrIdx = 0; instrIdx < srcProgram.size(); instrIdx++) {
		// printf("%#018lx: instr %ld prologue\n", (uint64_t) code, instrIdx);
		code = compileInstrPrologue(src, instrIdx, code);
		//printf("%#018lx: instr %ld body\n", (uint64_t) code, instrIdx);
		code = compileInstr(src, instrIdx, code);
	}

	//printf("%#018lx: last instr epilogue\n", (uint64_t) code);

	code = compileLastInstrEpilogue(src, code);

	//printf("%#018lx: machine epilogue\n", (uint64_t) code);

	code = compileMachineEpilogue(src, code);

	//printf("%#018lx: end\n", (uint64_t) code);
}

void Compiler_x64::populate(PrototypeFuncs_x64::Id funcId, uint8_t* begin,
		size_t placeholderIdx, int64_t v) {
	uint16_t offset = PF.LIST[funcId].placeholderOffset[placeholderIdx];

	if (offset == -1) {
		printf(
				"%s:%d in %s: prototype function %s has no placeholder with index %lu",
				__FILE__, __LINE__, __FUNCTION__, PF.LIST[funcId].name,
				placeholderIdx);
	}

	*((uint64_t*) (begin + offset)) = v;
}

uint8_t* Compiler_x64::compileInstr(const Machine& src, size_t instrIdx,
		ExecutableMemory& dst) {

	uint8_t* code = dst.begin() + PF.LIST[PF.MACHINE_PROL].size
			+ PF.instructionCodeSize * instrIdx + PF.LIST[PF.INSTR_PROL].size;

	code = compileInstr(src, instrIdx, code);

	return code;
}

uint8_t* Compiler_x64::compileInstr(const Machine& src, size_t instrIdx,
		uint8_t* dst) {
	Instruction const& instr = src.getProgram()[instrIdx];

	uint8_t* code = compilePrototype((PrototypeFuncs_x64::Id) instr.op.opcode,
			dst);

	uint64_t a;
	switch (instr.op.opcode) {
	case Operation::inc:
	case Operation::dec:
		break;

	case Operation::swp:
		populate(PF.INSTR_SWP, dst, 0, instr.getParam0());
		break;

	case Operation::add:
		populate(PF.INSTR_ADD, dst, 0, instr.getParam0());
		break;

	case Operation::sub:
		populate(PF.INSTR_SUB, dst, 0, instr.getParam0());
		break;

	case Operation::mul:
		populate(PF.INSTR_MUL, dst, 0, instr.getParam0());
		break;

	case Operation::div:
	case Operation::mod:
		a = (src.getInstrCount() - instrIdx) * PF.instructionCodeSize
				- (PF.LIST[PF.INSTR_PROL].size
						+ PF.LIST[PF.INSTR_DIV].placeholderOffset[1])
				+ PF.LIST[PF.LAST_INSTR_EPIL].size;

		// = (1 - 0) * a8 - (49 + 26) + 2b
		// = a8 - 49 + 2b - 26
		// = 5f + 5 = 64

		populate(PF.INSTR_DIV, dst, 0, instr.getParam0());
		populate(PF.INSTR_DIV, dst, 1, a);
		populate(PF.INSTR_DIV, dst, 2, instrIdx);
		populate(PF.INSTR_DIV, dst, 3,
				Machine::State::Phase::ABORTED_DIVZERO.code);
		break;

	case Operation::jmz:
		a = (instr.getParam0() - instrIdx) * PF.instructionCodeSize
				- (PF.LIST[PF.INSTR_PROL].size
						+ PF.LIST[PF.INSTR_JMZ].placeholderOffset[0]);

		populate(PF.INSTR_JMZ, dst, 0, a);
		break;

	case Operation::hlt:
		a = (src.getInstrCount() - instrIdx) * PF.instructionCodeSize
				- (PF.LIST[PF.INSTR_PROL].size
						+ PF.LIST[PF.INSTR_HLT].placeholderOffset[0])
				+ PF.LIST[PF.LAST_INSTR_EPIL].size;

		populate(PF.INSTR_HLT, dst, 0, a);
		populate(PF.INSTR_HLT, dst, 1, instrIdx + 1); // +1: for FINISHED, the instrPtr need to point behind
		populate(PF.INSTR_HLT, dst, 2, Machine::State::Phase::FINISHED.code);
		break;

	default:
		instr.op.throwMethodNotPreparedForOperationException();
		break;
	}

	uint8_t* instrSpaceEnd = dst + PF.specificInstrMaxCodeSize;
	code = addNops(code, instrSpaceEnd);

	return code;
}

uint8_t* Compiler_x64::compileInstrPrologue(const Machine& src, size_t instrIdx,
		uint8_t* dst) {
	uint8_t* code = compilePrototype(PF.INSTR_PROL, dst);

	uint64_t a = (src.getInstrCount() - instrIdx) * PF.instructionCodeSize
			- PF.LIST[PF.INSTR_PROL].placeholderOffset[0]
			+ PF.LIST[PF.LAST_INSTR_EPIL].size;

	populate(PF.INSTR_PROL, dst, 0, a);
	populate(PF.INSTR_PROL, dst, 1, instrIdx);
	populate(PF.INSTR_PROL, dst, 2, Machine::State::Phase::RUNNABLE.code);

	return code;
}

uint8_t* Compiler_x64::addNops(uint8_t* code, uint8_t* codeEnd) {
	for (; code < codeEnd; code++) {
		// nop # 90
		*code = 0x90;
	}

	return code;
}

uint8_t* Compiler_x64::compilePrototype(PrototypeFuncs_x64::Id protoFuncId,
		uint8_t* dst) {

	std::memcpy(dst, PF.LIST[protoFuncId].begin, PF.LIST[protoFuncId].size);
	dst += PF.LIST[protoFuncId].size;

	return dst;
}

uint8_t* Compiler_x64::compileMachinePrologue(Machine const& src,
		uint8_t* dst) {
	uint8_t* code = compilePrototype(PF.MACHINE_PROL, dst);

	populate(PF.MACHINE_PROL, dst, 0, src.getInstrCount());
	populate(PF.MACHINE_PROL, dst, 1, Machine::State::Phase::FINISHED.code);
	populate(PF.MACHINE_PROL, dst, 2, Machine::State::Phase::RUNNABLE.code);
	populate(PF.MACHINE_PROL, dst, 3, PF.instructionCodeSize);

	return code;
}

uint8_t* Compiler_x64::compileLastInstrEpilogue(const Machine& src,
		uint8_t* dst) {
	uint8_t* code = compilePrototype(PF.LAST_INSTR_EPIL, dst);

	populate(PF.LAST_INSTR_EPIL, dst, 0, src.getInstrCount());
	populate(PF.LAST_INSTR_EPIL, dst, 1, Machine::State::Phase::FINISHED.code);

	return code;
}

uint8_t* Compiler_x64::compileMachineEpilogue(const Machine& src,
		uint8_t* dst) {
	uint8_t* code = compilePrototype(PF.MACHINE_EPIL, dst);

	return code;
}

PrototypeFuncs_x64 const& Compiler_x64::PF = *PrototypeFuncs_x64::getInstance();

} /* namespace agos */
