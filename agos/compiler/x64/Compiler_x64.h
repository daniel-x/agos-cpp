#ifndef AGOS_COMPILER_COMPILER_X64_H_
#define AGOS_COMPILER_COMPILER_X64_H_

#include <cstdint>
#include "../../machine/Machine.h"
#include "../ExecutableMemory.h"
#include "../Compiler.h"
#include "PrototypeFuncs_x64.h"

namespace agos {

class Compiler_x64: Compiler {
public:
	static Compiler_x64* getInstance();

	virtual void compileMachine(const Machine& src, ExecutableMemory& dst);
	virtual uint8_t* compileInstr(const Machine& src, size_t instrIdx,
			ExecutableMemory& dst);

private:
	uint8_t* compileInstr(const Machine& src, size_t instrIdx, uint8_t* dst);

	static uint8_t* compileMachinePrologue(Machine const& src, uint8_t* dst);
	static uint8_t* compileMachineProloguePrototype(uint8_t* dst);
	static uint8_t* compileLastInstrEpilogue(Machine const& src, uint8_t* dst);
	static uint8_t* compileLastInstrEpiloguePrototype(uint8_t* dst);
	static uint8_t* compileMachineEpilogue(Machine const& src, uint8_t* dst);
	static uint8_t* compileMachineEpiloguePrototype(uint8_t* dst);

	static uint8_t* compileInstrPrologue(const Machine& src, size_t instrIdx,
			uint8_t* dst);

	static uint8_t* compileInstrInc(Machine const& src, uint8_t* dst);
	static uint8_t* compileInstrIncPrototype(uint8_t* dst);
	static uint8_t* compileInstrDec(Machine const& src, uint8_t* dst);
	static uint8_t* compileInstrDecPrototype(uint8_t* dst);
	static uint8_t* compileInstrSwp(Machine const& src, uint8_t* dst,
			uint64_t param0);
	static uint8_t* compileInstrSwpPrototype(uint8_t* dst);
	static uint8_t* compileInstrAdd(Machine const& src, uint8_t* dst,
			uint64_t param0);
	static uint8_t* compileInstrAddPrototype(uint8_t* dst);
	static uint8_t* compileInstrSub(Machine const& src, uint8_t* dst,
			uint64_t param0);
	static uint8_t* compileInstrSubPrototype(uint8_t* dst);
	static uint8_t* compileInstrMul(Machine const& src, uint8_t* dst,
			uint64_t param0);
	static uint8_t* compileInstrMulPrototype(uint8_t* dst);
	static uint8_t* compileInstrDiv(Machine const& src, uint8_t* dst,
			uint64_t param0);
	static uint8_t* compileInstrDivPrototype(uint8_t* dst);
	static uint8_t* compileInstrMod(Machine const& src, uint8_t* dst,
			uint64_t param0);
	static uint8_t* compileInstrModPrototype(uint8_t* dst);
	static uint8_t* compileInstrJmz(Machine const& src, uint8_t* dst,
			uint64_t param0);
	static uint8_t* compileInstrJmzPrototype(uint8_t* dst);
	static uint8_t* compileInstrHlt(Machine const& src, uint8_t* dst);
	static uint8_t* compileInstrHltPrototype(uint8_t* dst);

	static void populate(PrototypeFuncs_x64::Id funcId, uint8_t* begin,
			size_t placeholderIdx, int64_t v);

	static size_t getCodeSize(PrototypeFuncs_x64::PrototypeFunc* protoFunc);
	static uint8_t* compilePrototype(PrototypeFuncs_x64::Id protoFuncId,
			uint8_t* dst);

	static size_t getCodeSize(const Machine& machine);

	typedef int64_t (Int64Func)(int64_t* storageBegin, int64_t* storageEnd,
			size_t maxStepCount);

	static uint8_t* compileInstrBase(const Machine & src, size_t instrIdx,
			uint8_t* dst);
	static uint8_t* compileInstrEpilogue(const Machine& src, uint8_t* dst,
			uint8_t* dstEnd);

	static uint8_t* addNops(uint8_t* code, uint8_t* codeEnd);

	static PrototypeFuncs_x64 const& PF;

	Compiler_x64(Compiler_x64 const&) = delete;
	void operator=(Compiler_x64 const&) = delete;

	Compiler_x64();
};

} /* namespace agos */

#endif /* AGOS_COMPILER_COMPILER_X64_H_ */
